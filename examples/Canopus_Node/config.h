// ============================================================================
// Canopus Shared Configuration — config.h
// Shared constants across Gateway, Node, and Canopus firmware modules.
// Any change here automatically propagates to all modules at compile time.
// ============================================================================
#ifndef LORAONE_CONFIG_H
#define LORAONE_CONFIG_H

#include <stdint.h>

// ── FIRMWARE VERSION ────────────────────────────────────────────────────────
#define LORAONE_FW_VERSION_MAJOR  1
#define LORAONE_FW_VERSION_MINOR  1
#define LORAONE_FW_VERSION_PATCH  0
#define LORAONE_PROTOCOL_VERSION  2

// ── RADIO PARAMETERS ────────────────────────────────────────────────────────
#define LORA_FREQUENCY      868100000UL   // Hz (868.1 MHz)
#define LORA_SF             7             // Spreading Factor (7–12)
#define LORA_BW             0             // 0 = 125 kHz (RUI3 encoding)
#define LORA_CR             0             // 0 = 4/5 coding rate
#define LORA_PREAMBLE       8             // Preamble symbols
#define LORA_TX_POWER       22            // dBm

// ── TDMA TIMING ─────────────────────────────────────────────────────────────
#define SLOT_MS                 1000UL    // Duration of each node TX slot
#define SYNC_DISCOVERY_MS       5000UL    // Discovery window after chain
#define IDLE_AFTER_CHAIN_MS     1000UL    // Idle gap after last slot

// Total cycle = (N × SLOT_MS) + SYNC_DISCOVERY_MS + IDLE_AFTER_CHAIN_MS
#define CALC_TOTAL_CYCLE(n) \
  (((uint32_t)(n) * SLOT_MS) + SYNC_DISCOVERY_MS + IDLE_AFTER_CHAIN_MS)

// ── NODE LIMITS ─────────────────────────────────────────────────────────────
#define MAX_NODES               15        // Maximum node IDs supported (1–15)
#define DEFAULT_TOTAL_NODES     10        // Default if no Flash config found

// ── AIRTIME CONSTANTS (measured, SF-dependent) ──────────────────────────────
#define SF10_SYNC_AIRTIME       150UL     // ms — SYNC_START/END frame @ SF10
#define SF10_DATA_AIRTIME       395UL     // ms — DATA frame @ SF10
#define SF12_SYNC_AIRTIME       600UL     // ms — SYNC frame @ SF12
#define SF12_DATA_AIRTIME       1650UL    // ms — DATA frame @ SF12

// ── NODE TIMING PARAMETERS ──────────────────────────────────────────────────
#define ACK_TIMEOUT_MS          3000UL    // Max wait for GW_ACK after TX
#define SLOT_GUARD_MS           500UL     // Must finish TX before slot end
#define SYNC_STALE_MARGIN_MS    5000UL    // Extra margin before declaring sync lost
#define STAGED_RF_PRE_MS        15000UL   // Pre-wake RX window for beacon hunting
#define WAKEUP_MARGIN_MS        1000UL    // Wake-up margin before slot
#define NODE1_TX_DELAY_MS       300UL     // Node 1 extra delay to avoid SYNC collision
#define REBOOT_TIMEOUT_MS       86400000UL // 24h watchdog reboot

// ── GATEWAY TIMING PARAMETERS ───────────────────────────────────────────────
#define GW_RX_WINDOW_MS         30000UL   // Gateway RX listen window
#define GW_RX_ASSERT_INTERVAL   5000UL    // RX re-assert check interval
#define GW_STATUS_INTERVAL_MS   5000UL    // Status broadcast interval
#define GW_TANK_SEND_INTERVAL   10000UL   // Tank voltage report interval

// ── DRIFT CORRECTION ────────────────────────────────────────────────────────
#define DRIFT_CORRECTION_PCT    95        // Apply 95% of measured drift
#define DRIFT_MAX_ACCEPTABLE    60000L    // Ignore drift > 60s (likely wrap)
#define DRIFT_LATE_THRESHOLD    2000      // Drift > 2000ms = GW_LATE alert

// ── QUEUE / BUFFER SIZES ────────────────────────────────────────────────────
#define DOWNLINK_QUEUE_LEN      16        // Gateway → Node command queue
#define RECOVERY_QUEUE_LEN      30        // Chain recovery queue
#define URGENT_REPEAT_COUNT     3         // Urgent command repeat broadcasts
#define URGENT_REPEAT_INTERVAL  2000UL    // ms between urgent repeats

// ── SENSOR TYPE CODES (shared between Node TX and Canopus RX) ──────────────────
#define TYPE_TEMPERATURE        0x01
#define TYPE_HUMIDITY           0x02
#define TYPE_WIND_SPEED         0x03
#define TYPE_WIND_DIRECTION     0x04
#define TYPE_RAIN               0x05
#define TYPE_ENGINE_VOLT_1      0x06
#define TYPE_ENGINE_VOLT_2      0x07
#define TYPE_CANOPUS_VOLT       0x08
#define TYPE_TANK_LEVEL         0x09
#define TYPE_DIESEL_LEVEL       0x0A
#define TYPE_SYNC_ADV           0x0F      // SyncAdvisory forwarded to ESP32

// ── FLASH STORAGE OFFSETS (RAK3172 api.system.flash) ────────────────────────
// Ensure Gateway and Node never collide on the same flash region
#define FLASH_OFFSET_TOTAL_NODES  0       // 1 byte — syncTotalNodes
#define FLASH_OFFSET_PRE_WAKE     4       // 4 bytes — stagedRfPreMs (uint32_t)
#define FLASH_OFFSET_NODE_ID      10      // 1 byte — NODE_ID

// ── BATTLE MODE ─────────────────────────────────────────────────────────────
#define CFG_BROADCAST_INTERVAL  10000UL   // ms — config broadcast in battle mode

// ── MQTT (Canopus default topic paths) ──────────────────────────────────────
#define MQTT_TOPIC_DATA         "CANOPUS/DATA"
#define MQTT_TOPIC_CMD          "CANOPUS/CMD"
#define MQTT_TOPIC_URGENT       "CANOPUS/URGENT"
#define MQTT_DEFAULT_PORT       1883
#define MQTT_KEEPALIVE_SEC      60

#endif // LORAONE_CONFIG_H
