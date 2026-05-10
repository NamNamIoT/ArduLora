# 📜 ArduLora Rev 1.0 Technical Documentation

This document contains the pinout mapping and detailed schematic diagrams for the ArduLora board.

## 📍 1. Pinout Mapping

### I/O Pin Definitions
| **Pin Name** | **Note** |  
| :---: | :---: |    
| **PA8** | Yellow LED |  
| **PA9** | Red LED |  
| **PB2** | Blue LED |   
| **PA11** | I2C_SDA |  
| **PA12** | I2C_SCL |  
| **PB4** | Analog Input 1 (0-10V) |  
| **PA10** | Analog Input 2 (0-10V) |  
| **PB5** | Sensor Power Enable (3.3V) | 
| **PB6** | UART1_TX (RS485) |  
| **PB7** | UART1_RX (RS485) |  
| **PB3** | Battery Voltage Sensing (ADC) |  

---

## 🛠️ 2. Detailed Schematic Diagrams

### A. DC-DC Buck Converter
![Power DC-DC](https://github.com/user-attachments/assets/11891231-d21e-432b-a219-bd5268276307)

### B. LDO & Power Switching
![Power Management](https://github.com/user-attachments/assets/c1b4a85e-1af6-4898-9c54-8dcfe4d7d719)

### C. RAK3172 Core & LoRa
![RAK3172 Core](https://github.com/user-attachments/assets/37ed4d57-cd90-4551-8ea8-199e3161b97c)

### D. USB-Serial Interface (CH340E)
![USB Serial](https://github.com/user-attachments/assets/207afa6b-7e68-4a57-9d7b-0bd2b55825ed)

### E. Analog Input Buffer (Op-amp)
![Analog Buffer](https://github.com/user-attachments/assets/7af5b59e-ce39-4cf9-8780-0fce058aa041)

### F. RS485 Interface
![RS485 Interface](https://github.com/user-attachments/assets/df3c94c4-d5a1-41df-ac1f-044c727259d2)

---
