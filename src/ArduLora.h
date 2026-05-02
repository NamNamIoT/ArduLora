/*
  ArduLora.h - Library for ArduLora (RAK3172) development.
  Created by NamNamIoT.
  Released into the public domain.
*/

#ifndef ArduLora_h
#define ArduLora_h

#include <Arduino.h>

// ArduLora Hardware Pins Mapping for RUI3
#define ARDULORA_SENSOR_POWER  PB5

// LEDs
#define ARDULORA_LED_YELLOW    PA8
#define ARDULORA_LED_RED       PA9
#define ARDULORA_LED_BLUE      PB2

// Functional Aliases for LEDs
#define ARDULORA_LED_STATUS    ARDULORA_LED_YELLOW
#define ARDULORA_LED_RX        ARDULORA_LED_BLUE
#define ARDULORA_LED_TX        ARDULORA_LED_RED

// Analog Inputs (0-10V range with onboard voltage divider)
#define ARDULORA_AI1           PB4
#define ARDULORA_AI2           PA10

// Battery Monitoring
#define ARDULORA_ADC_BAT       PB3

// I2C Interface
#define ARDULORA_SDA           PA11
#define ARDULORA_SCL           PA12

// RS485 / Modbus Interface (UART1)
#define ARDULORA_RS485_TX      PB6
#define ARDULORA_RS485_RX      PB7

#endif
