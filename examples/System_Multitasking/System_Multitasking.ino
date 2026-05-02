#include <ArduLora.h>
/***
 *  This example shows multitasking in RUI3 with RAK1904, RAK1910 and LoRaWan join.
***/
//Lorawan

#define TEST_BAND     (RAK_REGION_US915)

#define TEST_DEVEUI   {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08}

#define TEST_APPEUI   {0x0E, 0x0D, 0x0D, 0x01, 0x0E, 0x01, 0x02, 0x0E}

#define TEST_APPKEY   {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C}
//1904

#include <Wire.h>

#include <Adafruit_LIS3DH.h>

#include <Adafruit_Sensor.h>
Adafruit_LIS3DH lis = Adafruit_LIS3DH();// I2C
//1910

#include <TinyGPS.h>
TinyGPS gps;
String tmp_data = "";
int direction_S_N = 0;  //0--S, 1--N
int direction_E_W = 0;  //0--E, 1--W
void direction_parse(String tmp)
{
    if (tmp.indexOf(",E,") != -1)
    {
        direction_E_W = 0;
    }
    else
    {
        direction_E_W = 1;
    }

    if (tmp.indexOf(",S,") != -1)
    {
        direction_S_N = 0;
    }
    else
    {
        direction_S_N = 1;
    }
}

void new_thread1(void) //1904
{
    while (1) {
        delay(2000);
        lis.read();      // get X Y and Z data at once

        /* Or....get a new sensor event, normalized */
        sensors_event_t event;
        lis.getEvent(&event);
        /* Display the results (acceleration is measured in m/s^2) */
        Serial.print("X: "); Serial.print(event.acceleration.x);
        Serial.print(" Y: "); Serial.print(event.acceleration.y);
        Serial.print(" Z: "); Serial.print(event.acceleration.z);
        Serial.println(" m/s^2 ");
        Serial.println();
    }
}

void new_thread2(void)//13007
{
    while (1) {
        digitalWrite(WB_IO4, LOW);
        delay(1000);
        digitalWrite(WB_IO4, HIGH); 
        delay(1000);
    }
}

void new_thread3(void)//lorawan
{
    while (api.lorawan.njs.get() == 0)
    {
        Serial.print("Waiting for Lorawan join...");
        api.lorawan.join();
        delay(10000);
    }
    while (1) {
        uint8_t payload[] = "Thread";

        if (api.lorawan.send(sizeof(payload), payload, 129, true, 1)) {
            Serial.println("Send Success");
        } else {
            Serial.println("Send fail");
        }
        delay(4000);
    }
}

void new_thread4(void)//log
{
    while (1) {
        udrv_serial_log_printf("This is new thread #4 fired every 4s! (%lu)\r\n", millis());
        delay(4000);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("RAKwireless System Multitasking  Example");
    Serial.println("------------------------------------------------------");
    time_t timeout = millis();
    Serial.begin(115200);
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            delay(100);
        }
        else
        {
            break;
        }
    }
  
    //1904
    while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
    Serial.println("LIS3DH test!");
    if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
        Serial.println("Couldnt start");
        while (1) yield();
    }
    Serial.println("LIS3DH found!");
    // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
    Serial.print("Range = "); Serial.print(2 << lis.getRange());
    Serial.println("G");
    // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
    Serial.print("Data rate set to: ");
    switch (lis.getDataRate()) {
        case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
        case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
        case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
        case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
        case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
        case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
        case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;
        case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
        case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
        case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
    }
    // 13007
    pinMode(WB_IO2, OUTPUT);
    digitalWrite(WB_IO2, HIGH);
    pinMode(WB_IO4, OUTPUT);
    //1910
    Serial1.begin(9600);
    while (!Serial1);
    Serial.println("GPS uart init ok!");
    //Lorawan
    uint8_t node_device_eui[8] = TEST_DEVEUI; 
    uint8_t node_app_eui[8] = TEST_APPEUI;
    uint8_t node_app_key[16] = TEST_APPKEY;
  
    api.lorawan.appeui.set(node_app_eui, 8);
    api.lorawan.appkey.set(node_app_key, 16);
    api.lorawan.deui.set(node_device_eui, 8);
    api.lorawan.band.set(TEST_BAND);
    api.lorawan.njm.set(1);
    api.lorawan.join();
  
    if (api.system.scheduler.task.create("new thread #1", new_thread1) == true) {
        Serial.printf("Thread #1 is successfully created.\r\n");
    } else {
        Serial.printf("Thread #1 can't be created.\r\n");
    }
    if (api.system.scheduler.task.create("new thread #2", new_thread2) == true) {
        Serial.printf("Thread #2 is successfully created.\r\n");
    } else {
        Serial.printf("Thread #2 can't be created.\r\n");
    }
    if (api.system.scheduler.task.create("new thread #3", new_thread3) == true) {
        Serial.printf("Thread #3 is successfully created.\r\n");
    } else {
        Serial.printf("Thread #3 can't be created.\r\n");
    }
    if (api.system.scheduler.task.create("new thread #4", new_thread3) == true) {
        Serial.printf("Thread #4 is successfully created.\r\n");
    } else {
        Serial.printf("Thread #4 can't be created.\r\n");
    }
}

void loop()//1910
{
    bool newData = false;
    unsigned long chars;
    unsigned short sentences, failed;

    // For one second we parse GPS data and report some key values
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
        while (Serial1.available())
        {
            char c = Serial1.read();
            tmp_data += c;
            if (gps.encode(c))
                newData = true;
        }
    }
    direction_parse(tmp_data);
    tmp_data = "";

    if (newData)
    {
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);
        if(direction_S_N == 0)
        {
            Serial.print("(S):");
        }
        else
        {
            Serial.print("(N):");
        }
        Serial.print("LAT=");
        Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        if(direction_E_W == 0)
        {
            Serial.print(" (E):");
        }
        else
        {
            Serial.print(" (W):");
        }
        Serial.print("LON=");
        Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        Serial.print(" SAT=");
        Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        Serial.print(" PREC=");
        Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    } 

    gps.stats(&chars, &sentences, &failed);
    Serial.print(" CHARS=");
    Serial.print(chars);
    Serial.print(" SENTENCES=");
    Serial.print(sentences);
    Serial.print(" CSUM ERR=");
    Serial.println(failed);
    if (chars == 0)
        Serial.println(" No characters received from GPS: check wiring ");
    delay(1000);
}




