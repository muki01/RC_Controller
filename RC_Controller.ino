#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <BleGamepad.h>
BleGamepad bleGamepad("Flight Controller", "Muki01", 100);

#define MP3_Player Serial2
#define MP3_Player_RX 2
#define MP3_Player_TX 0

//Inputs outputs
#define throttle_in 33
#define yaw_in 4
#define pitch_in 34
#define roll_in 39

#define pot1_in 35
#define pot2_in 25

#define toggle1_in 32
#define toggle2_in 26
#define toggle3_in 14
#define toggle4_in 27

#define switch1_in 22
#define switch2_in 36

#define battery_in 13
#define buttons_analog_in 12
#define led 2

#define ce_Pin 17
#define csnPin 5

const uint64_t pipeOut = 0xE8E8F0F0E1LL;
RF24 radio(ce_Pin, csnPin);

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
  byte AUX3;
  byte AUX4;
  byte pot1;
  byte pot2;
};

MyData data;

//Variables
int Throttle, Yaw, Pitch, Roll, Pot1, Pot2, AUX1, AUX2, AUX3, AUX4, Switch1, Switch2;

float batteryLevel = 0;
int button_read = 0;

int throttle_fine = 0, yaw_fine = 0, pitch_fine = 0, roll_fine = 0;
int throttle_to_send = 0, yaw_to_send = 0, pitch_to_send = 0, roll_to_send = 0, pot1_to_send = 0, pot2_to_send = 0;
bool throttle_inverted = true, yaw_inverted = true, pitch_inverted = true, roll_inverted = true;
bool throttle_decrease = false, yaw_decrease = false, pitch_decrease = false, roll_decrease = false;
bool throttle_increase = false, yaw_increase = false, pitch_increase = false, roll_increase = false;

bool previousAUX1 = LOW;
bool previousAUX2 = LOW;
bool previousAUX3 = LOW;
bool previousAUX4 = LOW;

// Flags for each battery level
bool warningFull = false;      // Full battery warning flag
bool warningHigh = false;      // High battery warning flag
bool warningMedium = false;    // Medium battery warning flag
bool warningLow = false;       // Low battery warning flag
bool warningVeryLow = false;   // Very low battery warning flag
bool warningCritical = false;  // Critical battery warning flag

void melody1();

bool sound = true, BluetoothMode = false;
int counter = 0, invert_counter = 0;

int mapConstrain(int input, int in_min, int in_max, int out_min, int out_max, int center, int deadzone_low, int deadzone_high, int fine = 0);

void setup() {
  Serial.begin(115200);
  delay(700);
  MP3_Player.begin(9600, SERIAL_8N1, MP3_Player_RX, MP3_Player_TX);

  initSpiffs();
  readSettings();

  pinMode(throttle_in, INPUT);
  pinMode(yaw_in, INPUT);
  pinMode(pitch_in, INPUT);
  pinMode(roll_in, INPUT);
  pinMode(pot1_in, INPUT);
  pinMode(pot2_in, INPUT);
  pinMode(toggle1_in, INPUT_PULLUP);
  pinMode(toggle2_in, INPUT_PULLUP);
  pinMode(toggle3_in, INPUT_PULLUP);
  pinMode(toggle4_in, INPUT_PULLUP);
  pinMode(switch1_in, INPUT_PULLUP);
  pinMode(switch2_in, INPUT_PULLUP);

  pinMode(battery_in, INPUT);
  pinMode(buttons_analog_in, INPUT);
  pinMode(led, OUTPUT);

  if (digitalRead(toggle1_in)) {
    while (analogRead(throttle_in) <= 3200 || digitalRead(toggle1_in) == 0 || digitalRead(toggle2_in) == 0 || digitalRead(toggle3_in) == 0 || digitalRead(toggle4_in) == 0) {
      MP3_Player_playTrack(1);
      delay(3000);
    }
    BluetoothMode = false;
    beginNRF24();
    resetData();
    MP3_Player_playTrack(8);
    delay(1600);
  } else {
    MP3_Player_playTrack(9);
    delay(1600);
    BluetoothMode = true;
    beginBluetooth();
  }

  // Serial.print("Throttle_fine= "), Serial.print(throttle_fine);
  // Serial.print(" Yaw_fine= "), Serial.print(yaw_fine);
  // Serial.print(" Pitch_fine= "), Serial.print(pitch_fine);
  // Serial.print(" Roll_fine= "), Serial.print(roll_fine);
  // Serial.print(" Throttle_inverted= "), Serial.print(throttle_inverted);
  // Serial.print(" Yaw_inverted= "), Serial.print(yaw_inverted);
  // Serial.print(" Pitch_inverted= "), Serial.print(pitch_inverted);
  // Serial.print(" Roll_inverted= "), Serial.print(roll_inverted);

  // throttle_fine = -10;
  // yaw_fine = 20;
  // pitch_fine = 9;
  // roll_fine = -20;
}


void loop() {
  battery();
  buttons();

  if (BluetoothMode) {
    if (bleGamepad.isConnected()) {
      Throttle = mapConstrain(analogRead(throttle_in), 200, 3500, 0, 32767, 16383, 1850, 1850, throttle_fine);
      Yaw = mapConstrain(analogRead(yaw_in), 400, 3450, 0, 32767, 16383, 1820, 1960, yaw_fine);
      Pitch = mapConstrain(analogRead(pitch_in), 400, 3050, 0, 32767, 16383, 1690, 1890, pitch_fine);
      Roll = mapConstrain(analogRead(roll_in), 280, 3300, 32767, 0, 16383, 1580, 1740, roll_fine);
      Pot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 32767, 0, 16383, 2048, 2048);
      Pot2 = mapConstrain(analogRead(pot2_in), 0, 4095, 32767, 0, 16383, 2048, 2048);
      AUX1 = !digitalRead(toggle1_in);
      AUX2 = !digitalRead(toggle2_in);
      AUX3 = !digitalRead(toggle3_in);
      AUX4 = !digitalRead(toggle4_in);
      Switch1 = digitalRead(switch1_in);
      Switch2 = digitalRead(switch2_in);
      printData();

      handleButtonPress(AUX1, BUTTON_1);
      handleButtonPress(AUX2, BUTTON_2);
      handleButtonPress(AUX3, BUTTON_3);
      handleButtonPress(AUX4, BUTTON_4);

      bleGamepad.setAxes(Yaw, Throttle, 32767, Roll, Pitch, 32767, Pot1, Pot2);
    }
  } else {

    data.throttle = Throttle = mapConstrain(analogRead(throttle_in), 200, 3500, 255, 0, 127, 1750, 1850, throttle_fine);
    data.yaw = Yaw = mapConstrain(analogRead(yaw_in), 400, 3450, 0, 255, 127, 1820, 1960, yaw_fine);
    data.pitch = Pitch = mapConstrain(analogRead(pitch_in), 400, 3050, 0, 255, 127, 1690, 1890, pitch_fine);
    data.roll = Roll = mapConstrain(analogRead(roll_in), 280, 3300, 255, 0, 127, 1580, 1740, roll_fine);
    data.pot1 = Pot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 255, 0, 127, 2048, 2048);
    data.pot2 = Pot2 = mapConstrain(analogRead(pot2_in), 0, 4095, 255, 0, 127, 2048, 2048);
    data.AUX1 = AUX1 = !digitalRead(toggle1_in);
    data.AUX2 = AUX2 = !digitalRead(toggle2_in);
    data.AUX3 = AUX3 = !digitalRead(toggle3_in);
    data.AUX4 = AUX4 = !digitalRead(toggle4_in);
    Switch1 = digitalRead(switch1_in);
    Switch2 = digitalRead(switch2_in);
    printData();

    if (data.AUX1 != previousAUX1) {
      if (data.AUX1 == HIGH) {
        MP3_Player_playTrack(2);
      } else {
        MP3_Player_playTrack(3);
      }
      previousAUX1 = data.AUX1;
    }
    if (data.AUX2 != previousAUX2) {
      if (data.AUX2 == HIGH) {
        MP3_Player_playTrack(6);
      } else {
        MP3_Player_playTrack(7);
      }
      previousAUX2 = data.AUX2;
    }
    if (data.AUX3 != previousAUX3) {
      if (data.AUX3 == HIGH) {
        MP3_Player_playTrack(4);
      } else {
        //MP3_Player_playTrack(1);
      }
      previousAUX3 = data.AUX3;
    }
    if (data.AUX4 != previousAUX4) {
      if (data.AUX4 == HIGH) {
        MP3_Player_playTrack(5);
      } else {
        //MP3_Player_playTrack(4);
      }
      previousAUX4 = data.AUX4;
    }

    radio.write(&data, sizeof(MyData));
    delay(10);
  }
}