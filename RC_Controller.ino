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

#define toggle_1 32
#define toggle_2 26
#define toggle_3 14
#define toggle_4 27

#define switch_1 22
#define switch_2 36

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
float battery_level = 0;
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

bool sound = true, sound_changed = false, BluetoothMode = false;
int counter = 0, invert_counter = 0;

int mapConstrain(int input, int in_min, int in_max, int out_min, int out_max, int center, int deadzone_low, int deadzone_high, int fine = 0);

void setup() {
  Serial.begin(115200);
  delay(600);
  MP3_Player.begin(9600, SERIAL_8N1, MP3_Player_RX, MP3_Player_TX);

  initSpiffs();
  readSettings();

  pinMode(throttle_in, INPUT);
  pinMode(yaw_in, INPUT);
  pinMode(pitch_in, INPUT);
  pinMode(roll_in, INPUT);
  pinMode(pot1_in, INPUT);
  pinMode(pot2_in, INPUT);
  pinMode(toggle_1, INPUT_PULLUP);
  pinMode(toggle_2, INPUT_PULLUP);
  pinMode(toggle_3, INPUT_PULLUP);
  pinMode(toggle_4, INPUT_PULLUP);
  pinMode(switch_1, INPUT_PULLUP);
  pinMode(switch_2, INPUT_PULLUP);

  pinMode(battery_in, INPUT);
  pinMode(buttons_analog_in, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  if (digitalRead(toggle_1)) {
    while (analogRead(throttle_in) <= 3200 || digitalRead(toggle_1) == 0 || digitalRead(toggle_2) == 0 || digitalRead(toggle_3) == 0 || digitalRead(toggle_4) == 0) {
      MP3_Player_playTrack(1);
      delay(3000);
    }
    BluetoothMode = false;
    beginNRF24();
    resetData();
    MP3_Player_playTrack(8);
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
  //battery read
  battery_level = (double)analogRead(battery_in) / 4096 * 10.0;
  battery_level = round(battery_level * 10) / 10;

  if (battery_level >= 4.5 && battery_level <= 5.5) {
    analogWrite(led, 20);
  } else {
    analogWrite(led, 0);
  }
  buttons();

  if (BluetoothMode) {
    if (bleGamepad.isConnected()) {

      int mapThrottle = mapConstrain(analogRead(throttle_in), 200, 3500, 32767, 0, 16383, 1850, 1850, throttle_fine);
      int mapYaw = mapConstrain(analogRead(yaw_in), 400, 3450, 0, 32767, 16383, 1820, 1960, yaw_fine);
      int mapPitch = mapConstrain(analogRead(pitch_in), 400, 3050, 0, 32767, 16383, 1690, 1890, pitch_fine);
      int mapRoll = mapConstrain(analogRead(roll_in), 280, 3300, 32767, 0, 16383, 1580, 1740, roll_fine);
      int mapPot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 32767, 0, 16383, 2048, 2048);
      int mapPot2 = mapConstrain(analogRead(pot2_in), 0, 4095, 32767, 0, 16383, 2048, 2048);
      int AUX1 = digitalRead(toggle_1);
      int AUX2 = digitalRead(toggle_2);
      int AUX3 = digitalRead(toggle_3);
      int AUX4 = digitalRead(toggle_4);
      int Switch1 = digitalRead(switch_1);
      int Switch2 = digitalRead(switch_2);

      Serial.print("Bluetooth Throttle= "), Serial.print(mapThrottle);
      Serial.print(" Yaw= "), Serial.print(mapYaw);
      Serial.print(" Pitch= "), Serial.print(mapPitch);
      Serial.print(" Roll= "), Serial.print(mapRoll);
      Serial.print(" Pot1= "), Serial.print(mapPot1);
      Serial.print(" Pot2= "), Serial.print(mapPot2);
      Serial.print(" AUX1= "), Serial.print(AUX1);
      Serial.print(" AUX2= "), Serial.print(AUX2);
      Serial.print(" AUX3= "), Serial.print(AUX3);
      Serial.print(" AUX4= "), Serial.print(AUX4);
      Serial.print(" Switch1= "), Serial.print(Switch1);
      Serial.print(" Switch2= "), Serial.println(Switch2);

      handleButtonPress(AUX1, BUTTON_1);
      handleButtonPress(AUX2, BUTTON_2);
      handleButtonPress(AUX3, BUTTON_3);
      handleButtonPress(AUX4, BUTTON_4);

      bleGamepad.setAxes(mapYaw, mapThrottle, mapPitch, mapRoll, mapPot1, mapPot2, 32767, 32767);
    }
  } else {

    data.throttle = mapConstrain(analogRead(throttle_in), 200, 3500, 255, 0, 127, 1750, 1850, throttle_fine);
    data.yaw = mapConstrain(analogRead(yaw_in), 400, 3450, 0, 255, 127, 1820, 1960, yaw_fine);
    data.pitch = mapConstrain(analogRead(pitch_in), 400, 3050, 0, 255, 127, 1690, 1890, pitch_fine);
    data.roll = mapConstrain(analogRead(roll_in), 280, 3300, 255, 0, 127, 1580, 1740, roll_fine);
    data.pot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 255, 0, 127, 2048, 2048);
    data.pot2 = mapConstrain(analogRead(pot2_in), 0, 4095, 255, 0, 127, 2048, 2048);
    data.AUX1 = !digitalRead(toggle_1);
    data.AUX2 = !digitalRead(toggle_2);
    data.AUX3 = !digitalRead(toggle_3);
    data.AUX4 = !digitalRead(toggle_4);

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

    Serial.print(" Throttle= "), Serial.print(data.throttle);
    Serial.print(" Yaw= "), Serial.print(data.yaw);
    Serial.print(" Pitch= "), Serial.print(data.pitch);
    Serial.print(" Roll= "), Serial.print(data.roll);
    Serial.print(" Pot1= "), Serial.print(data.pot1);
    Serial.print(" Pot2= "), Serial.print(data.pot2);
    Serial.print(" AUX1= "), Serial.print(data.AUX1);
    Serial.print(" AUX2= "), Serial.print(data.AUX2);
    Serial.print(" AUX3= "), Serial.print(data.AUX3);
    Serial.print(" AUX4= "), Serial.print(data.AUX4);
    Serial.print(" Switch1= "), Serial.print(digitalRead(switch_1));
    Serial.print(" Switch2= "), Serial.println(digitalRead(switch_2));
    radio.write(&data, sizeof(MyData));
    delay(10);
  }
}