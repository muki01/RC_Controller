# 🛩️RC Controller
This project is an ESP32-based RC controller that allows you to control RC drones via the NRF24 module and also use it for drone simulators on a PC through Bluetooth.

## Features:
- NRF24 Communication: Control real RC drones wirelessly using the NRF24L01 module.
- Bluetooth Connectivity: Seamlessly connect the controller to your computer for use with drone simulators, providing a realistic flying experience.
- Customizable: Modify and expand the code to suit your specific needs, whether for different types of drones or simulators.
- Cross-Platform Compatibility: Works with various flight simulation software that supports Bluetooth controllers.

## ⚙️Instalation
* Open .ino file
* 📚Instal these Libraries
  ~~~
  - RF24
  - BleGamepad
  ~~~
* Edit the pins for your Board
     ~~~
      #define throttle_in 0
      #define yaw_in 4
      #define pitch_in 34
      #define roll_in 39
      
      #define pot1_in 35
      #define pot2_in 12
      #define toggle_1 32
      #define toggle_2 15
      
      #define battery_in 33
      #define buttons_analog_in 27
      #define buzzer 22
      #define led 2
      
      #define ce_Pin 17
      #define csnPin 5
     ~~~
* Upload the code to your Board
* Upload the data folder to SPIFFS.
  - To upload the data folder, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.
  
## Usage:
* If you want to use the controller with NRF24 for a real Drone, you need to start the controller with AUX1 at HIGH Position
* If you want to use the controller with Bluetooth for a Drone Simulator, you need to start the controller with AUX1 at LOW Position

## Hardware Used
- ESP32 Wroom
- NRF24L01 RF Module
- 2.8 TFT Display
- 12 x Buttons
- 2 x MTS-102 Swich
- 2 x 10K Potatntiometer
- 2 x Joysticks
- Mini Buck Converter Module
- Red Led
- 5V Passive Buzzer

## Photos:
Below are some photos showcasing the ESP32 RC controller setup, wiring, and its usage with both real drones and flight simulators.

- Schematic:
- Photos of the controller: 
- Using with Drone Simulator: 
