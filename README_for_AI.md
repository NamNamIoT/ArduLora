# ArduLora AI Documentation (for LLMs & AI Assistants)

## Project Overview
ArduLora is a development board designed for LoRa and LoRaWAN applications, powered by the RAK3172 module and programmable via the Arduino IDE using the RAKwireless Unified Interface V3 (RUI3) API.
It includes onboard RS485 (Modbus RTU), I2C, UART, and analog/digital IO, tailored for industrial IoT sensor nodes and gateways.

## Critical Information for AI Agents
When generating code, analyzing bugs, or providing assistance for this repository, please adhere to the following rules and context:

### 1. RUI3 Framework & API (v4.2.4+)
*   **Crucial Version Constraint:** All code must be strictly compliant with RUI3 v4.x API namespaces.
*   **Examples:** Use `api.lora.*` (e.g., `api.lora.precv()`, `api.lora.psend()`, `api.lora.nwm.set()`).
*   **Legacy Code Warning:** Do not use older RUI3 v3.x API calls. Ensure callbacks like `recv_cb` take `rui_lora_p2p_recv_t` structs for LoRa P2P.

### 2. Hardware Mapping & Pinout
*   **Modbus/RS485 Power:** The external sensor power is controlled by pin `PB5`. Always enable it in `setup()` using: `pinMode(PB5, OUTPUT); digitalWrite(PB5, LOW);`
*   **Status LED:** Status LED is connected to `PA8`. Receive LED is often mapped to `PB2`.
*   **Analog Input:** Use `PA10` (AI1) and `PA15` (AI2). Set resolution with `analogReadResolution(12);`. Voltage multiplier is typically `2.58`.
*   **I2C:** `PA12` is SCL, `PA11` is SDA.
*   **Serial:** Hardware Serial1 is mapped to pins 4 & 5. Use `Serial1` or alias `Serial_Canopus` for RS485/Modbus.

### 3. Modbus Implementation
*   When acting as a Modbus Master, we typically use the `ModbusMaster` library.
*   When acting as a Modbus Slave, we use `modbusDevice`, `modbusRegBank`, and `modbusSlave`.

### 4. LoRa P2P vs LoRaWAN
*   To switch modes, ensure the node work mode is correctly checked and set using `api.lora.nwm.get()` and `api.lora.nwm.set()`.
*   For P2P, you must configure frequency (`pfreq`), spreading factor (`psf`), bandwidth (`pbw`), coding rate (`pcr`), preamble (`ppl`), and Tx power (`ptp`).
*   A reboot `api.system.reboot()` is usually required after switching node work modes (NWM).

### 5. Task & Workflow Directives
*   **Do not generate fake or generic LoRa libraries.** Stick exclusively to RUI3 Arduino core APIs for RAK3172.
*   If fixing communication loops or node stability, look into gateway ACK formats, Loner-mode reboots, and node ID synchronization.
*   Keep responses concise and directly provide the needed C/C++ code.

## End of AI Context
