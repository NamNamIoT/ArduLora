<img width="1891" height="715" alt="Image" src="https://github.com/user-attachments/assets/1521528e-bbab-47a0-b030-8e4a245bc831" />

> [!NOTE]
> **AI & LLM Assistants:** Please read [README_for_AI.md](./docs/README_for_AI.md) for critical repository context, API rules, and project guidelines before generating code or modifying files.

🛒 **Where to buy:**  
[<img src="https://img.shields.io/badge/Tindie-Buy_on_Tindie-17AEB9?style=for-the-badge&logo=tindie&logoColor=white" alt="Buy on Tindie" height="40" style="border: 2px solid #333; border-radius: 4px;">](https://www.tindie.com/stores/thanhnamlt5/?ref=offsite_badges&utm_source=sellers_thanhnamlt5&utm_medium=badges&utm_campaign=badge_large) [<img src="https://img.shields.io/badge/Elecrow-Buy_on_Elecrow-009A44?style=for-the-badge" alt="Buy on Elecrow" height="40" style="border: 2px solid #333; border-radius: 4px;">](https://www.elecrow.com/ardulora.html) [<img src="https://img.shields.io/badge/IC_Day_Roi-Buy_on_IC_Day_Roi-FF8C00?style=for-the-badge" alt="Buy on IC Day Roi" height="40" style="border: 2px solid #333; border-radius: 4px;">](https://icdayroi.com/ardulora-board-phat-trien-lora-voi-arduino-ide-day-du-cong-rs485-i2c-uart)

🎫 **Tindie Discount code:** `canopus`  

<p align="left">
<a href="https://fb.com/kuem0912" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/facebook.svg" alt="kuem0912" height="30" width="40" /></a>
<a href="https://wa.me/84969809444" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/whatsapp.svg" alt="84969809444" height="30" width="40" /></a>  
</p>

# ArduLora Board — The Ultimate LoRa & LoRaWAN Development Platform

**ArduLora** is a compact, ultra-low-power, Arduino-compatible development board specifically optimized for long-range **LoRa** and **LoRaWAN** communication. 

Powered by the robust **RAK3172** module (STM32WLE5CC core) and running the official **RUI3 (v4.2.4+)** framework, ArduLora is the perfect solution for engineering smart agriculture nodes, industrial IoT telemetry, remote monitoring stations, and low-power sensor nodes.

---

## ✨ Key Features & Advantages

*   📡 **Premium LoRa/LoRaWAN Radio**: Supports long-range Peer-to-Peer (P2P) radio and full LoRaWAN Class A, B, and C standard protocols for global deployments.
*   🔌 **Industrial RS485 Port**: Onboard RS485 transceiver supports hardware Modbus RTU (Master/Slave) to easily poll industrial-grade PLC networks and sensors.
*   ⚡ **Advanced Power Management**: 
    *   Onboard high-efficiency DC-DC buck converter supporting wide input voltages (**5V to 24V DC**).
    *   Ultra-low deep sleep current consumption for extended battery runtime.
    *   Dedicated GPIO-controlled power gate pin (`PB5`) to completely shut down external sensors/transceivers during sleep.
*   🌡️ **Integrated Sensor Interfaces**:
    *   Dual high-voltage Analog Inputs (`PA10`/`AI1` and `PA15`/`AI2`) with voltage scaling (0-10V input ready).
    *   Hardware I2C port (`PA11`/SDA, `PA12`/SCL) with onboard pull-up resistors.
*   🗺️ **GPS Support**: Plug-and-play compatibility with external ATGM336H GPS modules.
*   🧑‍💻 **Modern Programming Ecosystem**: Code natively inside the Arduino IDE using standard Arduino functions combined with RAKwireless Unified Interface V3 (RUI3) APIs.

---

## 📊 Hardware Specifications

| Parameter | Specifications |
| --- | --- |
| **Core Module** | RAKwireless RAK3172 (STM32WLE5CC @ 32 MHz) |
| **LoRa Protocol** | LoRaWAN v1.0.3 (Class A, B, C) and LoRa P2P |
| **Frequency Bands** | EU868, US915, AS923, AU915, KR920, IN865, etc. |
| **Input Voltage** | 5V – 24V DC via power terminal, or 5V USB-C |
| **Sensor Power Control** | Pin `PB5` (Active LOW to supply VCC power) |
| **Analog Input Channels** | 2x Analog Inputs (0-10V measurement range) |
| **Digital Output LEDs** | 3x Onboard programmable LEDs (Yellow `PA8`, Red `PA9`, Blue `PB2`) |
| **Communication Ports** | 1x RS485 (Modbus RTU), 1x I2C, 1x Serial UART |

---

## 🗺️ Board Information & Layout

#### Version Hardware Revisions

🏷️ **Rev 1.0 [July-2025] Layout:**

<img width="550" height="850" alt="image" src="https://github.com/user-attachments/assets/a699d479-abec-4219-8a04-b773fc4f8c56" style="max-width: 100%; height: auto; object-fit: contain;">  
	   
🏷️ **Rev 1.1 [Sept-2025] Layout:**

<img width="550" height="850" alt="image" src="https://github.com/user-attachments/assets/76875bfb-e99e-4296-a595-90021a524fcc" style="max-width: 100%; height: auto; object-fit: contain;">

---

## 📖 Quick Start & Documentation Portal

Get up and running quickly by browsing our detailed guides:

1.  🚀 **[Software Setup Guide](./docs/Software_setup.md)**: Steps to install the Arduino IDE BSP, add RAK3172 module support, configure COM ports, and flash your first test code.
2.  🧑‍💻 **[Programming & Examples Guide](./docs/Examples_Guide.md)**: Comprehensive examples containing code snippets, flowchart diagrams, and explanations for:
    *   Digital I/O & Blinking LED
    *   Analog Input Readings
    *   Modbus Master/Slave (RS485)
    *   I2C Bus Scanner & Sensor Reading (SHT3x, BH1750)
    *   LoRa P2P Sender & Receiver
    *   MCU System Deep Sleep, Time, and Timer tasks
    *   GPS ATGM336H Integration
3.  📡 **[LoRaWAN Configuration Guide](./docs/LoraWan_guide.md)**: Walkthrough on joining LoRaWAN networks using OTAA and ABP modes.
4.  ⚙️ **[Hardware Schematics & Pinout](./docs/Source_hardware.md)**: Schematic diagrams and detailed pinout maps.
5.  🔗 **[RFM95 / SX1278 (Ra-02) Interfacing](./docs/Rak2Ra02.md)**: Guide on communicating between ArduLora and standard ESP32/Arduino LoRa modules.
6.  🤖 **[LLM/AI Assistant Reference](./docs/README_for_AI.md)**: Documentation context and API namespaces for developers utilizing AI coding assistants.

---

## 📦 Easy Library Installation

You can download the **ArduLora** helper library directly from the official **Arduino Library Manager**:

1.  Open Arduino IDE and go to **Sketch > Include Library > Manage Libraries...**
2.  Type `ArduLora` into the search bar.
3.  Click **Install**.

<img width="778" height="272" alt="Library Installation" src="https://github.com/user-attachments/assets/89599189-bafa-4e14-b41a-62ea9a7a0e7e" />  

Include it at the top of your sketches:
```cpp
#include <ArduLora.h>
```

---

🛒 **Get your ArduLora board at:** [**Tindie**](https://www.tindie.com/stores/thanhnamlt5/?ref=offsite_badges&utm_source=sellers_thanhnamlt5&utm_medium=badges&utm_campaign=badge_large) | [**Elecrow**](https://www.elecrow.com/ardulora.html) | [**IC Day Roi**](https://icdayroi.com/ardulora-board-phat-trien-lora-voi-arduino-ide-day-du-cong-rs485-i2c-uart)  

[![GitHub Repo stars](https://img.shields.io/badge/share%20on-facebook-1976D2?logo=facebook)](https://www.facebook.com/sharer/sharer.php?u=https://github.com/NamNamIoT/ArduLora)  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/NamNamIoT/ArduLora/blob/main/LICENSE) 
