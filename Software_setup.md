#### 🧑🏼‍💻 Software setup  

- Download and install the [Arduino IDE](https://www.arduino.cc/en/Main/Software).

> **⚠️ Warning**  
> **For Windows 10 and up users**:
> If your Arduino IDE is installed from the Microsoft App Store, you need to reinstall your Arduino IDE by getting it from the Arduino official website.
> The Arduino app from the Microsoft App Store has problems using third-party Board Support Packages.

- Add RAK3172_Canopus as a supported board in Arduino IDE by updating Board Manager URLs in **Preferences** settings of Arduino IDE with the JSON URL below.
```json
https://raw.githubusercontent.com/RAKWireless/RAKwireless-Arduino-BSP-Index/main/package_rakwireless.com_rui_index.json
```
After that, you can then add **RAKwireless RUI STM32 Boards** via Arduino board manager.

#### Software

The default firmware of RAK3172_Canopus is based on RUI3, which allows you to develop your own custom firmware to connect sensors and other peripherals to it. To develop using your custom firmware using the Arduino IDE, you need to first add RAKwireless RUI STM32 boards to the Arduino board manager, which will be discussed in this guide. You can then use [RUI3 APIs] (https://docs.rakwireless.com/RUI3/Arduino-API/) for your intended application.

##### RAK3172_Canopus board Support Package in Arduino IDE

If you don't have an Arduino IDE yet, you can download it on the [Arduino official website](https://www.arduino.cc/en/Main/Software) and follow the installation procedure in the [miscellaneous section](/Product-Categories/wisduo/RAK3172-Module/Quickstart/#arduino-installation) of this document.
  
Once the Arduino IDE has been successfully installed, you can now configure the IDE to add the RAK3172_Canopus to its board selection by following these steps.

1. Open Arduino IDE and go to **File** > **Preferences**.

![preferences](https://user-images.githubusercontent.com/49629370/224521061-7ff3624f-7f93-479d-98b3-5c6e0dc08208.png)

2. To add the RAK3172_Canopus to your Arduino Boards list, edit the **Additional Board Manager URLs**.

![additional-boards](https://user-images.githubusercontent.com/49629370/224521079-1eee507e-22ed-4f4e-b52d-965619bf090e.png)

3. Copy the URL below and paste it on the field. If there are other URLs already there, just add them on the next line. After adding the URL, click **OK**.

```json
https://raw.githubusercontent.com/RAKWireless/RAKwireless-Arduino-BSP-Index/main/package_rakwireless.com_rui_index.json
```

![preferences-url](https://user-images.githubusercontent.com/49629370/224521108-5f18b3a2-7775-48fd-b8c5-f71ffb2e427c.png)

4. Restart the Arduino IDE.

5. Open the Boards Manager from the Tools Menu.

![boards-manager](https://user-images.githubusercontent.com/49629370/224521119-0c5ceeaf-b944-413b-9f04-d7f7e5b9ca50.png)

6. Write `RAK` in the search bar. This will show the available RAKwireless module boards that you can add to your Arduino Board list.

7. Click on the area highlighted in blue to select **RAKwireless RUI STM32 Boards**. Install the latest version of the  **RAKwireless RUI STM32 Boards** by clicking on **Install** button.

![rui3-stm32](https://user-images.githubusercontent.com/49629370/224521130-6419c6b9-d8fc-41d8-ac7e-e9a03db0fd59.png)

##### Configure RAK3172_Canopus on Boards Manager

8. Once the BSP is installed, select  **Tools** > **Boards Manager** > **RAKWireless RUI STM32 Modules** > **WisDuo RAK3172 Evaluation Board**. 

The RAK3172_Canopus board uses the RAK3172 WisDuo module.
![rui-stm32](https://user-images.githubusercontent.com/49629370/224521144-d1ec1a5f-5c67-46f1-8f99-5da1b4b57a23.png)

9. Install RAK3172_Canopus library

⚠️ WARNING
**_Both RAK Wisblock Examples_** and **_RAK3172_Canopus Examples_** support for RAK3172_Canopus board  
The RAK3172_Canopus library will only focus on sampleApp for common sensors and devices using the modbus RTU interface.  
The RAK Wisblock library will focus on Lora and lora Wan applications.  

Download https://github.com/NamNamIoT/RAK3172_CANOPUS and Extract to Arduino\libraries
![Extra](https://user-images.githubusercontent.com/49629370/224536933-6abbdef3-832f-401c-98c7-cdeeca470cff.PNG)


Find RAK3172_Canopus library:


![Canopus_example](https://user-images.githubusercontent.com/49629370/224537325-ce897674-e104-43c6-900c-542c1bdc90f9.PNG)

##### RAK3172 COM Port on Device Manager

Connect the RAK3172 via USB type C and check COM Port using Windows **Device Manager**. Double-click the reset button if the module is not detected.

![rui-port](https://user-images.githubusercontent.com/49629370/224521157-ca08067c-cce1-434d-bd27-10ac67277c7b.png)  
#### At here , you are ready 👌 to execute the sample code for the RAK3172_CANOPUS board  

### Compile an Example with Arduino LED Breathing

1. After completing the steps on adding your RAK3172_Canopus to the Arduino IDE, you can now try to run a simple program to test your setup. You need to add two LEDs to the bare minimum schematic of the RAK3172 module.

<rk-img
  src="/assets/images/wisduo/rak3172-module/quickstart/rak3172_bare_minimum_schematic_led.png"
  width="90%"
  caption="RAK3172 with two LEDs"
/>

2. Launch Arduino IDE and configure WisDuo RAK3172 Evaluation Board on board selection.

3. Connect the RAK3172_Canopus via USB type C.

4. Open the **Tools** Menu and select a COM port. **COM28** is currently used.
![select-port](https://user-images.githubusercontent.com/49629370/224521257-10db6f5c-5e77-4e4c-b020-58a330de9ba4.png)

5. You can see the serial monitor icon and click it to connect the COM port.
![serial-mon](https://user-images.githubusercontent.com/49629370/224521270-9a2f7282-18f3-4334-a568-788f2e5a7bf0.png)

6. Open **Arduino_Led_Breathing** example code.

![led-example](https://user-images.githubusercontent.com/49629370/224521281-3bb48679-7d97-43c2-a011-3373e572af73.png)

7. Click on the **Verify** icon to check if you have successfully compiled the example code.

![verify-code](https://user-images.githubusercontent.com/49629370/224521295-0991bbb8-ab1a-44a7-b6fd-228f8f26dc27.png)

8. Click the **Upload** icon to send the compiled firmware to your RAK3172 module.

:::tip 📝 NOTE:
RAK3172_Canopus should automatically go to BOOT mode when the firmware is uploaded via Arduino IDE.

If BOOT mode is not initiated, pull to ground the RESET pin twice (or double click the reset button if available) to force BOOT mode.
:::

![upload-code](https://user-images.githubusercontent.com/49629370/224521302-282b1a09-dc2c-43ae-a24a-9be1c7050a3d.png)

9. If the upload is successful, you will see the **Upgrade Complete** message.

![dev-prog](https://user-images.githubusercontent.com/49629370/224521311-344542b3-d8ec-4008-a721-58e47bf241ce.png)

10. After the Device Programmed is completed, you will see that LEDs are blinking.

