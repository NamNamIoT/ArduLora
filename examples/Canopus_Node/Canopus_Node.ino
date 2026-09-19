// ============================================================================
// RAK3172 Firmware — Canopus LoRa Node (Integrated with ArduLora Library)
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "config.h"
#include "ArduLora.h"
#include "Adafruit_SHT31.h"

// ======================= PIN & TYPE DEFINITIONS ============================
static uint8_t NODE_ID = 255;
static uint8_t NODE_TYPE = 1; // 1: AGRI, 2: WEATHER, 3: GAS
Adafruit_SHT31 sht30;

// ======================= SHT30 DRIVER (Minimal, no library) ================
#define SHT30_ADDR 0x44
static int16_t sensorVal1 = 0; // Temp*100 (AGRI) or ADC raw (other)
static int16_t sensorVal2 = 0; // Hum*100  (AGRI) or ADC raw (other)

bool readSHT30(float &temp, float &hum) {
  Wire.beginTransmission(SHT30_ADDR);
  Wire.write(0x24); // High repeatability, no clock stretch
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) return false;
  delay(15); // SHT30 measurement time
  Wire.requestFrom((uint8_t)SHT30_ADDR, (uint8_t)6);
  if (Wire.available() < 6) return false;
  uint8_t buf[6];
  for (int i = 0; i < 6; i++) buf[i] = Wire.read();
  uint16_t rawT = ((uint16_t)buf[0] << 8) | buf[1];
  uint16_t rawH = ((uint16_t)buf[3] << 8) | buf[4];
  temp = -45.0f + 175.0f * (float)rawT / 65535.0f;
  hum  = 100.0f * (float)rawH / 65535.0f;
  return true;
}

void readSensors() {
  ArduLora.sensorPower(true);  // Power ON sensors (sets PB5 LOW)
  delay(10);                   // Warm-up delay
  if (NODE_TYPE == 1) {
    // AGRI: SHT30 I2C
    float t = 0, h = 0;
    if (readSHT30(t, h)) {
      sensorVal1 = (int16_t)(t * 100); // e.g. 2531 = 25.31°C
      sensorVal2 = (int16_t)(h * 100); // e.g. 6520 = 65.20%
    } else {
      Serial.println("[SENSOR] SHT30 read failed");
    }
  } else {
    // Other types: ADC on ARDULORA_AI1 (PA10), ARDULORA_AI2 (PA15)
    sensorVal1 = (int16_t)analogRead(ARDULORA_AI1);
    sensorVal2 = (int16_t)analogRead(ARDULORA_AI2);
  }
  ArduLora.sensorPower(false); // Power OFF sensors (sets PB5 HIGH)
  Serial.printf("[SENSOR] Type=%u Val1=%d Val2=%d\r\n", NODE_TYPE, sensorVal1, sensorVal2);
}

// ======================= SYSTEM CONSTANTS ===================================
#define SYNC_SLOT_MS 1000UL         // Fixed slot reserved for Beacon
static uint32_t SLOT_MS = 1000UL;    // TX slot duration (depends on SF)

inline uint32_t getSlotMsFromSF(uint16_t sfVal) {
  if (sfVal == 12) return 5000UL;
  if (sfVal == 10) return 2000UL;
  return 1000UL; // For SF7, SF8, SF9
}

#define DISCOVERY_WINDOW_MS SYNC_DISCOVERY_MS // Discovery window for new Nodes (5s)
#define IDLE_AFTER_CHAIN_MS 1000UL  // Idle time after all slots complete
#define CALC_TOTAL_CYCLE(n) (SYNC_SLOT_MS + ((uint32_t)(n) * SLOT_MS) + DISCOVERY_WINDOW_MS + IDLE_AFTER_CHAIN_MS)

// Radio Settings (initialized from config.h)
uint32_t myFreq = LORA_FREQUENCY;
uint16_t sf = LORA_SF;
uint16_t bw = LORA_BW;
uint16_t cr = LORA_CR;
uint16_t preamble = LORA_PREAMBLE;
uint16_t txPower = LORA_TX_POWER;

float humd = 0.0;    
float temp = 0.0; 
// Dynamic State
static uint32_t nodeRxIntervalMs = CALC_TOTAL_CYCLE(1);
volatile uint32_t stagedRfPreMs = STAGED_RF_PRE_MS;
static uint8_t syncTotalNodes = 1;

static void applySF(uint16_t newSF) {
  sf = newSF;
  SLOT_MS = getSlotMsFromSF(sf);
  api.lora.psf.set(sf);
  nodeRxIntervalMs = CALC_TOTAL_CYCLE(syncTotalNodes);
}
static bool hasStar = false;
volatile uint32_t syncStartReceivedMs = 0;
volatile int32_t timingOffset = 0;
volatile bool syncStartPending = false;
volatile bool nodeTxDone = false;
volatile bool preciseSlotPending = false;
volatile uint32_t preciseSlotMs = 0;
volatile bool cfgAckPending = false;
volatile uint8_t cfgAckTotal = 0;
volatile bool gotGwAck = false;
volatile bool cadBusy = false;
volatile bool pendingReboot = false;
char pendingAckCmd[16] = "";
volatile uint32_t lastAliveMs = 0;
static uint8_t missedSyncCycles = 0;
static bool preWakeRxOpened = false;
static uint32_t txSeqCounter = 0;
static bool inDiscoveryWindow = false;
static uint8_t maxIdSeen = 0;
static uint32_t lastLonerTxMs = 0;

// ======================= UTILS =============================================

void safeFlashSet(uint32_t offset, uint8_t *data, uint32_t len) {
  uint8_t current[16];
  if (len > 16)
    return;
  if (api.system.flash.get(offset, current, len)) {
    if (memcmp(current, data, len) == 0)
      return;
  }
  api.system.flash.set(offset, data, len);
}

uint32_t getSyncAirtime() { return (sf == 12) ? SF12_SYNC_AIRTIME : SF10_SYNC_AIRTIME; }
uint32_t getDataAirtime() { return (sf == 12) ? SF12_DATA_AIRTIME : SF10_DATA_AIRTIME; }

// ======================= CALLBACKS =========================================

void recv_cb(rui_lora_p2p_recv_t data) {
  if (data.BufferSize == 0)
    return;

  char buf[256];
  uint16_t len = (data.BufferSize < 255) ? data.BufferSize : 255;
  memcpy(buf, data.Buffer, len);
  buf[len] = '\0';

  if (buf[0] == '*') {
    char *p = buf + 1;
    char *end = strchr(p, '#');
    if (end)
      *end = '\0';
    char *frame = p;

    missedSyncCycles = 0;
    lastAliveMs = millis();

    if (strncmp(frame, "CMD,", 4) == 0) {
      int targetId;
      char cmdBody[64];
      if (sscanf(frame, "CMD,%d,%63[^#]", &targetId, cmdBody) >= 2) {
        if (targetId == NODE_ID) {
          if (strcmp(cmdBody, "REBOOT") == 0) {
            strcpy(pendingAckCmd, "OK");
            pendingReboot = true;
          } else if (strncmp(cmdBody, "SET_SLEEP,", 10) == 0) {
            uint32_t val = (uint32_t)atoi(cmdBody + 10);
            if (val >= 1000) {
              nodeRxIntervalMs = val;
              uint8_t n = (uint8_t)(val / SLOT_MS);
              if (n >= 1 && n <= 15) safeFlashSet(0, &n, 1);
            }
            strcpy(pendingAckCmd, "OK");
          } else {
            strcpy(pendingAckCmd, "OK");
          }
        }
      }
    } else if (strncmp(frame, "CFG_NODES,", 10) == 0) {
      uint8_t n = (uint8_t)atoi(frame + 10);
      if (n >= 1 && n <= 15) {
        safeFlashSet(FLASH_OFFSET_TOTAL_NODES, &n, 1);
        syncTotalNodes = n; // Sync RAM variable
        nodeRxIntervalMs = CALC_TOTAL_CYCLE(n);
        cfgAckPending = true;
        cfgAckTotal = n;
      }
    } else if (strncmp(frame, "SYNC_START", 10) == 0) {
      inDiscoveryWindow = false;
      preWakeRxOpened = false;

      char *saveptr;
      char *token = strtok_r(frame + 10, ",", &saveptr);
      if (token != NULL) {
        uint8_t total = (uint8_t)atoi(token);
        if (total > 0 && total <= 15) {
          if (total != syncTotalNodes) {
            syncTotalNodes = total;
            Serial.printf("[CFG] Total Nodes updated to: %u\r\n", syncTotalNodes);
          }
          nodeRxIntervalMs = CALC_TOTAL_CYCLE(total);
        }
        // Parse CycleCounter (skip)
        char *cycleToken = strtok(NULL, ",");
        // Parse SF
        char *sfToken = strtok(NULL, ",");
        if (sfToken) {
          uint16_t rxSF = (uint16_t)atoi(sfToken);
          if ((rxSF == 7 || rxSF == 10 || rxSF == 12) && rxSF != sf) {
            applySF(rxSF);
            safeFlashSet(8, (uint8_t *)&rxSF, 2);
            Serial.printf("[CFG] SF updated to %u via Beacon\r\n", rxSF);
          }
        }
      }

      uint32_t now = millis();
      syncStartReceivedMs = now - getSyncAirtime();
      syncStartPending = true;

      if (missedSyncCycles > 0) {
        timingOffset = 0;
        missedSyncCycles = 0;
        Serial.printf("[SYNC] Beacon recovered after LONER. Offset reset.\r\n");
      }

      Serial.println("\r\n[SYNC] Beacon detected. Resyncing TDMA...");
      Serial.printf("[SYNC] Anchor Time: %lu ms\r\n", syncStartReceivedMs);

    } else if (strncmp(frame, "DATA,", 5) == 0) {
      uint8_t oid;
      int f2, f3, f4, f5, f6, isStar;
      if (sscanf(frame, "DATA,%hhu,%d,%d,%d,%d,%d,%d", &oid, &f2, &f3, &f4, &f5, &f6,
                 &isStar) >= 7) {
        if (oid != NODE_ID) {
          Serial.printf("[RX] Node %u DATA (S1=%d S2=%d Vbat=%d Star=%d) RSSI=%d SNR=%d\r\n",
                        oid, f2, f3, f4, isStar, data.Rssi, data.Snr);
          if (isStar == 1 && !syncStartPending) {
            uint32_t now = millis();
            uint32_t neighborSlotOffset = (uint32_t)(oid - 1) * SLOT_MS;
            uint32_t estimatedAnchor =
                now - neighborSlotOffset - getDataAirtime();
            syncStartReceivedMs = estimatedAnchor;
            syncStartPending = true;
            missedSyncCycles = 0;
            Serial.printf("[OVERHEAR] Synced from Star Node %u! Anchor inferred: "
                          "%lu ms\r\n",
                          oid, syncStartReceivedMs);
          }
        }
      }
    } else if (strncmp(frame, "GW_ACK,", 7) == 0) {
      char *saveptr;
      char *token = strtok_r(frame + 7, ",", &saveptr);
      if (token != NULL) {
        uint8_t ackId = (uint8_t)atoi(token);
        if (ackId == NODE_ID) {
          gotGwAck = true;
          while ((token = strtok_r(NULL, ",", &saveptr)) != NULL) {
            char *eq = strchr(token, '=');
            if (eq) {
              *eq = '\0';
              char *key = token;
              char *val = eq + 1;
              if (strcmp(key, "OK") == 0) {
                hasStar = (val[0] == '1');
              } else if (strcmp(key, "NODES") == 0) {
                uint8_t n = (uint8_t)atoi(val);
                if (n >= 1 && n <= 15) {
                  safeFlashSet(FLASH_OFFSET_TOTAL_NODES, &n, 1);
                  syncTotalNodes = n; // Sync RAM variable
                  nodeRxIntervalMs = CALC_TOTAL_CYCLE(n);
                }
              } else if (strcmp(key, "SF") == 0) {
                uint16_t rxSF = (uint16_t)atoi(val);
                if ((rxSF == 7 || rxSF == 10 || rxSF == 12) && rxSF != sf) {
                  applySF(rxSF);
                  safeFlashSet(8, (uint8_t *)&rxSF, 2);
                  Serial.printf("[CFG] SF updated to %u via GW_ACK\r\n", rxSF);
                }
              } else if (strcmp(key, "DFT") == 0) {
                int32_t drift = atol(val);
                int32_t cycleMs = (int32_t)nodeRxIntervalMs;
                if (cycleMs <= 0)
                  cycleMs = 45000;
                int32_t smartDrift = drift % cycleMs;
                if (smartDrift > cycleMs / 2)
                  smartDrift -= cycleMs;
                if (smartDrift < -cycleMs / 2)
                  smartDrift += cycleMs;

                if (abs(smartDrift) < 60000) {
                  if (syncStartPending) {
                    int32_t adjustment = smartDrift;
                    timingOffset += adjustment;
                    if (timingOffset > (int32_t)SLOT_MS)
                      timingOffset = (int32_t)SLOT_MS;
                    if (timingOffset < -(int32_t)SLOT_MS)
                      timingOffset = -(int32_t)SLOT_MS;
                    Serial.printf("[SYNC] GW Drift: %+ld ms | SmartAdj: %+ld ms "
                                  "| Offset: %+ld ms\r\n",
                                  (long)drift, (long)adjustment,
                                  (long)timingOffset);
                  } else {
                    uint32_t now = millis();
                    uint32_t mySlotOffset = (uint32_t)(NODE_ID - 1) * SLOT_MS;
                    syncStartReceivedMs = (now - mySlotOffset) - smartDrift;
                    Serial.printf(
                        "[SYNC] LONER SMART SYNC! New Anchor: %lu ms\r\n",
                        syncStartReceivedMs);
                  }
                }
              } else if (strcmp(key, "ADV") == 0) {
                int adv = atoi(val);
                if (adv == 8) {
                  hasStar = true;
                  Serial.printf(
                      "[ADV] GW Adv: %d/15 | PERFECT -> Keep timing\r\n", adv);
                } else if (adv >= 1 && adv <= 15) {
                  const char *dir = (adv < 8) ? "EARLY -> Should wake later"
                                              : "LATE -> Should wake earlier";
                  Serial.printf(
                      "[ADV] GW Advisory: %d/15 | %s | Offset: %+ld ms\r\n", adv,
                      dir, (long)timingOffset);
                }
              } else if (strcmp(key, "CMD") == 0) {
                if (strcmp(val, "REBOOT") == 0) {
                  pendingReboot = true;
                } else if (strncmp(val, "SET_SLEEP", 9) == 0) {
                  char *comma = strchr(val, ',');
                  if (comma) {
                    uint32_t sv = (uint32_t)atoi(comma + 1);
                    if (sv >= 1000) {
                      nodeRxIntervalMs = sv;
                      uint8_t sn = (uint8_t)(sv / SLOT_MS);
                      if (sn >= 1 && sn <= 15) safeFlashSet(0, &sn, 1);
                    }
                  }
                } else if (strncmp(val, "SET_PREWAKE", 11) == 0) {
                  char *comma = strchr(val, ',');
                  if (comma) {
                    uint32_t pw = (uint32_t)atoi(comma + 1);
                    if (pw >= 300 && pw <= 60000) {
                      stagedRfPreMs = pw;
                      safeFlashSet(4, (uint8_t *)&pw, 4);
                    }
                  }
                }
                Serial.printf("[CMD] Received: %s\r\n", val);
              }
            }
          }
        }
      }
    } else if (strncmp(frame, "CFG_SF,", 7) == 0) {
      uint16_t newSF = (uint16_t)atoi(frame + 7);
      if (newSF == 7 || newSF == 10 || newSF == 12) {
        applySF(newSF);
        safeFlashSet(8, (uint8_t *)&newSF, 2);
        Serial.printf("[CFG] SF=%u via LoRa. SLOT_MS=%lu ms\r\n", newSF, (unsigned long)SLOT_MS);
      }
    } else if (strncmp(frame, "CFG_PREWAKE,", 12) == 0) {
      uint32_t pw = (uint32_t)atoi(frame + 12);
      if (pw >= 300 && pw <= 60000) {
        stagedRfPreMs = pw;
        safeFlashSet(4, (uint8_t *)&pw, 4);
        Serial.printf("[CFG] PreWake updated: %u ms\r\n", pw);
      }
    } else if (strncmp(frame, "SYNC_END", 8) == 0) {
      inDiscoveryWindow = true;
      if (!syncStartPending) {
        uint32_t now = millis();
        uint32_t chainingDuration = (uint32_t)syncTotalNodes * SLOT_MS;
        syncStartReceivedMs = now - SYNC_SLOT_MS - chainingDuration - getSyncAirtime();
        syncStartPending = true;
        missedSyncCycles = 0;
        Serial.printf(
            "[SYNC] Recovered from SYNC_END! Anchor inferred: %lu ms\r\n",
            syncStartReceivedMs);
      } else {
        syncStartPending = false;
      }
    }
  }
}

void send_cb(void) { nodeTxDone = true; }

bool robustP2PSend(const char *frame, uint32_t maxEndTs = 0) {
  size_t frameLen = strlen(frame);
  for (int retry = 0; retry < 2; retry++) {
    uint32_t now = millis();
    if (maxEndTs > 0 && (now >= maxEndTs))
      return false;
    api.lora.precv(0);
    delay(50);
    nodeTxDone = false;
    if (ArduLora.sendP2P((uint8_t *)frame, frameLen)) {
      uint32_t waitStart = millis();
      uint32_t txTimeout = getDataAirtime() + 500; // Airtime + margin
      while (!nodeTxDone && (millis() - waitStart < txTimeout))
        delay(5);
      if (nodeTxDone)
        return true;
    }
    delay(50);
  }
  return false;
}

void setup() {
  Serial.begin(115200, RAK_CUSTOM_MODE);
  Serial.println("\r\n=== Canopus TDMA Node Starting ===");

  // Initialize ArduLora library class
  ArduLora.begin();

  uint8_t storedId = 255;
  if (api.system.flash.get(FLASH_OFFSET_NODE_ID, &storedId, 1))
    NODE_ID = storedId;
  else
    api.system.flash.set(FLASH_OFFSET_NODE_ID, &NODE_ID, 1);
  Serial.printf("NODE_ID: %u\r\n", NODE_ID);

  uint8_t n = 0;
  if (api.system.flash.get(FLASH_OFFSET_TOTAL_NODES, &n, 1) && n >= 1 && n <= 15) {
    syncTotalNodes = n;
    nodeRxIntervalMs = CALC_TOTAL_CYCLE(n);
  } else {
    n = 1;
    syncTotalNodes = n;
    nodeRxIntervalMs = CALC_TOTAL_CYCLE(n);
  }
  Serial.printf("Cycle Config: %u nodes · %lu ms/slot · Total Cycle: %u ms\r\n", 
                n, SLOT_MS, nodeRxIntervalMs);

  uint32_t p = 0;
  if (api.system.flash.get(FLASH_OFFSET_PRE_WAKE, (uint8_t *)&p, 4) && p >= 1000 && p <= 60000)
    stagedRfPreMs = p;
  else
    stagedRfPreMs = STAGED_RF_PRE_MS;

  uint16_t storedSF = 0;
  if (api.system.flash.get(8, (uint8_t *)&storedSF, 2) && (storedSF == 7 || storedSF == 10 || storedSF == 12)) {
    sf = storedSF;
    SLOT_MS = getSlotMsFromSF(sf);
    nodeRxIntervalMs = CALC_TOTAL_CYCLE(syncTotalNodes);
  }
  Serial.printf("SF Config: SF%u -> SLOT_MS=%lu ms\r\n", sf, (unsigned long)SLOT_MS);
  Serial.printf("Pre-wake Config: %u ms\r\n", stagedRfPreMs);

  // Power sensors ON for physical initialization
  ArduLora.sensorPower(true);
  Wire.begin();
  if (!sht30.begin(0x44)) {
    Serial.println("Erreur initialisation SHT30");
  } else {
    Serial.println("SHT30 : OK ");
  }
  ArduLora.sensorPower(false); // Turn OFF to save power after boot init
  delay(100);

  randomSeed(analogRead(ARDULORA_AI1) + (uint32_t)NODE_ID);

  // CRITICAL: Ensure P2P mode and parameters. Reboot if switching nwm.
  if (!ArduLora.configLoraP2P(myFreq, sf, bw, cr, preamble, txPower)) {
    Serial.println("[RADIO] configLoraP2P FAILED!");
  }
  // Disable encryption
  api.lora.encry.set(0);

  Serial.printf("Radio Config: %.3f MHz, SF%d, BW%d, CR4/%d, PWR%d dBm\r\n",
                myFreq / 1e6, sf, bw, (cr == 0 ? 5 : (cr == 1 ? 6 : (cr == 2 ? 7 : 8))), txPower);
  api.lora.registerPRecvCallback(recv_cb);
  api.lora.registerPSendCallback(send_cb);
  lastAliveMs = millis();

  // Boot announcement — send DATA immediately so Gateway knows we're alive
  if (NODE_ID != 255) {
    delay(500 + (uint32_t)NODE_ID * 200); // Stagger by ID to avoid collision
    readSensors();
    uint16_t vbat_cv = (uint16_t)(api.system.bat.get() * 100);
    char bootData[80];
    snprintf(bootData, sizeof(bootData), "*DATA,%u,%u,%d,%d,%u,0,0,0,%u,%u#",
             NODE_ID, NODE_TYPE, sensorVal1, sensorVal2, vbat_cv, syncTotalNodes, sf);
    Serial.println("[BOOT] Sending boot announcement...");
    if (robustP2PSend(bootData)) {
      Serial.println("[BOOT] Announcement sent. Listening for GW_ACK...");
      gotGwAck = false;
      api.lora.precv(ACK_TIMEOUT_MS);
      ArduLora.deepSleep(ACK_TIMEOUT_MS);
      if (gotGwAck) {
        Serial.println("[BOOT] Gateway responded! Synced.");
      } else {
        Serial.println("[BOOT] No GW_ACK — will sync via Beacon.");
      }
    }
  }
}

void loop() {
  // --- Serial command handler (zero-String, char-buffer only) ---
  static char serialBuf[64];
  static uint8_t serialIdx = 0;
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialIdx > 0) {
        serialBuf[serialIdx] = '\0';
        // Trim trailing whitespace
        while (serialIdx > 0 && (serialBuf[serialIdx-1] == ' ' || serialBuf[serialIdx-1] == '\r'))
          serialBuf[--serialIdx] = '\0';

        static bool setIdDone = false;
        if (!setIdDone && strncmp(serialBuf, "SET_ID=", 7) == 0) {
          setIdDone = true;
          uint8_t newId = (uint8_t)atoi(serialBuf + 7);
          if (newId > 0 && newId < 255) {
            api.system.flash.set(FLASH_OFFSET_NODE_ID, &newId, 1);
            Serial.printf("[CFG] ID saved %u. Rebooting...\r\n", newId);
            delay(500);
            ArduLora.reboot();
          } else {
            Serial.printf("[CFG] Invalid ID %u (must be 1-254)\r\n", newId);
            setIdDone = false;
          }
        } else if (strncmp(serialBuf, "SET_TOTAL_NODES=", 16) == 0) {
          uint8_t newTotal = (uint8_t)atoi(serialBuf + 16);
          if (newTotal >= 1 && newTotal <= 15) {
            syncTotalNodes = newTotal;
            nodeRxIntervalMs = CALC_TOTAL_CYCLE(newTotal);
            api.system.flash.set(FLASH_OFFSET_TOTAL_NODES, &syncTotalNodes, 1);
            Serial.printf("[CFG] Total Nodes set to %u. New Interval: %lu ms. Re-syncing...\r\n",
                          syncTotalNodes, (unsigned long)nodeRxIntervalMs);
            missedSyncCycles = 1;
            syncStartReceivedMs = millis() - nodeRxIntervalMs + 14000;
            gotGwAck = false;
            preWakeRxOpened = false;
          } else {
            Serial.printf("[CFG] Invalid Total Nodes %u (must be 1-15)\r\n", newTotal);
          }
        }
        serialIdx = 0;
      }
    } else {
      if (serialIdx < sizeof(serialBuf) - 1) {
        serialBuf[serialIdx++] = c;
      }
    }
  }

  if (pendingAckCmd[0] != '\0') {
    char ack[32];
    snprintf(ack, sizeof(ack), "*ACK,%u,%s#", NODE_ID, pendingAckCmd);
    Serial.printf("[TX] Sending ACK for command: %s\r\n", pendingAckCmd);
    robustP2PSend(ack);
    pendingAckCmd[0] = '\0';
  }

  if (pendingReboot) {
    delay(1000);
    ArduLora.reboot();
  }

  if (cfgAckPending) {
    cfgAckPending = false;
    uint32_t stagger = 500 + (uint32_t)(NODE_ID - 1) * 100;
    delay(stagger);
    char cfgFrame[32];
    snprintf(cfgFrame, sizeof(cfgFrame), "*CFG_ACK,%u,%u#", NODE_ID,
             cfgAckTotal);
    Serial.printf("[TX] Sending CFG_ACK (Total: %u)\r\n", cfgAckTotal);
    robustP2PSend(cfgFrame);
  }

  uint32_t now = millis();
  uint32_t referenceMs =
      (syncStartReceivedMs > 0) ? syncStartReceivedMs : lastAliveMs;
  if ((int32_t)(now - referenceMs) >
      (int32_t)(nodeRxIntervalMs + SYNC_STALE_MARGIN_MS)) {
    syncStartReceivedMs = 0;
    syncStartPending = false;
    missedSyncCycles++;
    lastAliveMs = now;
  }

  uint8_t requiredCycles = (NODE_ID == 1) ? 2 : 3;
  if (missedSyncCycles >= requiredCycles) {
    uint32_t nowMs = millis();
    // Send Loner frame periodically (once per cycle to avoid congestion)
    if (nowMs - lastLonerTxMs >= nodeRxIntervalMs || lastLonerTxMs == 0) {
      lastLonerTxMs = nowMs;
      uint32_t lbt = random(1000, DISCOVERY_WINDOW_MS);
      delay(lbt);
      if (NODE_ID == 255) {
        char chipId[17];
        strncpy(chipId, ArduLora.getChipID().c_str(), sizeof(chipId) - 1);
        chipId[sizeof(chipId) - 1] = '\0';
        char join[64];
        snprintf(join, sizeof(join), "*JOIN,%s,0#", chipId);
        Serial.println("[TX] Attempting Loner JOIN...");
        robustP2PSend(join);
      } else {
        char data[80];
        readSensors();
        uint16_t vbat_cv = (uint16_t)(api.system.bat.get() * 100);
        snprintf(data, sizeof(data), "*DATA,%u,%u,%d,%d,%u,0,0,0,%u,%u#", NODE_ID, NODE_TYPE, sensorVal1, sensorVal2, vbat_cv, syncTotalNodes, sf);
        Serial.println("[TX] Attempting Loner DATA send...");
        if (robustP2PSend(data)) {
          Serial.print("[TX] Data sent successfully :");
          Serial.println(data);
          gotGwAck = false;
          api.lora.precv(ACK_TIMEOUT_MS);
          ArduLora.deepSleep(ACK_TIMEOUT_MS);
        }
      }
      if (gotGwAck) {
        // GW ACK received -> sync recovery Rx staging
        missedSyncCycles = 1;
        syncStartReceivedMs = millis() - nodeRxIntervalMs + 14000;
        gotGwAck = false; 
        preWakeRxOpened = false;
        Serial.println("[LONER] GW_ACK received. Timeline staged for sync recovery RX.");
      }
    }
    
    // Ensure Rx is enabled to look for Beacon/Overhear
    if (!preWakeRxOpened) {
      api.lora.precv(65535); // Continuous RX mode
      preWakeRxOpened = true;
      Serial.println("[SYNC] Mất đồng bộ! Node THỨC LIÊN TỤC ở chế độ RX để dò Beacon/Overhear...");
    }
    delay(100);
    return;
  }

  if (syncStartPending) {
    uint32_t expectedAt =
        syncStartReceivedMs + (uint32_t)NODE_ID * SLOT_MS + 200 - timingOffset;
    int32_t timeUntilSlot = (int32_t)(expectedAt - millis());
    if (timeUntilSlot > 5) {
      api.lora.precv(0);
      delay(50);
      Serial.printf("[SLEEP] Shallow Sleep (Slot Wait) for %u ms...\r\n",
                    (uint32_t)timeUntilSlot);
      ArduLora.deepSleep((uint32_t)timeUntilSlot);
      Serial.println("[WAKE] Woke up from Slot Sleep.");
    }
    if (timeUntilSlot > -(int32_t)(SLOT_MS / 2)) {
      readSensors();
      char data[80];
      uint16_t vbat_cv = (uint16_t)(api.system.bat.get() * 100);
      snprintf(data, sizeof(data), "*DATA,%u,%u,%d,%d,%u,%u,%u,%ld,%u,%u#", NODE_ID, NODE_TYPE,
               sensorVal1, sensorVal2, vbat_cv, txSeqCounter++, hasStar ? 1 : 0, (long)timingOffset, syncTotalNodes, sf);
      Serial.printf("[TX] Sending Scheduled DATA (Seq: %u, Star: %d)...\r\n", txSeqCounter, hasStar ? 1 : 0);
      if (robustP2PSend(data, expectedAt + (SLOT_MS - SLOT_GUARD_MS))) {
        Serial.print("[TX] Data sent successfully :");
        Serial.println(data);
        hasStar = false;
        gotGwAck = false;
        api.lora.precv(ACK_TIMEOUT_MS);
        ArduLora.deepSleep(ACK_TIMEOUT_MS);
      }
    } else {
      syncStartReceivedMs += nodeRxIntervalMs;
    }
    syncStartPending = false;
  }

  if (hasStar && !syncStartPending) {
    uint32_t mySlotTime = syncStartReceivedMs + nodeRxIntervalMs +
                          (uint32_t)NODE_ID * SLOT_MS + 200 - timingOffset;
    if (mySlotTime > millis()) {
      uint32_t totalSleep = mySlotTime - millis();
      uint32_t preWake = 300;
      if (totalSleep > preWake) {
        api.lora.precv(0);
        delay(50);
        Serial.printf("[SLEEP] Deep Sleep (Star Skip) for %u ms...\r\n",
                      totalSleep - preWake);
        ArduLora.deepSleep(totalSleep - preWake);
        Serial.println("[WAKE] Woke up from Star Skip.");
        syncStartPending = true;
        syncStartReceivedMs = syncStartReceivedMs + nodeRxIntervalMs;
        preWakeRxOpened = false;
        return;
      }
    }
  }

  now = millis();
  uint32_t elapsed = now - syncStartReceivedMs;
  if (elapsed < nodeRxIntervalMs) {
    uint32_t sleepDur = nodeRxIntervalMs - elapsed;
    uint32_t preWake = (missedSyncCycles > 0 || !gotGwAck) ? stagedRfPreMs : 5000;
    if (sleepDur > preWake) {
      api.lora.precv(0);
      delay(50);
      Serial.printf("[SLEEP] Deep Sleep (Beacon Wait) for %u ms...\r\n",
                    sleepDur - preWake);
      ArduLora.deepSleep(sleepDur - preWake);
      Serial.println("[WAKE] Woke up from Beacon Wait.");
      api.lora.precv(preWake + SYNC_STALE_MARGIN_MS);
      preWakeRxOpened = true;
      return;
    } else {
      if (!preWakeRxOpened) {
        api.lora.precv(sleepDur + SYNC_STALE_MARGIN_MS);
        preWakeRxOpened = true;
        return;
      } else {
        delay(100);
        return;
      }
    }
  } else {
    // If not synchronized initial status, open RX continuously
    if (!preWakeRxOpened) {
      api.lora.precv(65535); // Continuous RX mode
      preWakeRxOpened = true;
      Serial.println("[SYNC] Chưa có đồng bộ ban đầu! Node THỨC LIÊN TỤC ở chế độ RX để dò Beacon/Overhear...");
    }
    delay(100);
    return;
  }

  if ((int32_t)(now - lastAliveMs) > (int32_t)REBOOT_TIMEOUT_MS)
    ArduLora.reboot();
}
