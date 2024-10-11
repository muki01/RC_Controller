#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <BleGamepad.h>
BleGamepad bleGamepad("Flight Controller", "Muki01", 100);

//Inputs outputs
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

bool sound = true, sound_changed = false, BluetoothMode = false;
int counter = 0, invert_counter = 0;

int mapConstrain(int input, int in_min, int in_max, int out_min, int out_max, int center, int deadzone_low, int deadzone_high, int fine = 0);

void setup() {
  Serial.begin(115200);
  initSpiffs();
  readSettings();

  pinMode(throttle_in, INPUT);
  pinMode(yaw_in, INPUT);
  pinMode(pitch_in, INPUT);
  pinMode(roll_in, INPUT);
  pinMode(pot1_in, INPUT);
  //pinMode(pot2_in, INPUT);
  pinMode(toggle_1, INPUT_PULLUP);
  pinMode(toggle_2, INPUT_PULLUP);

  pinMode(battery_in, INPUT);
  pinMode(buttons_analog_in, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  startMelody();

  if (digitalRead(toggle_1)) {
    BluetoothMode = false;
    radio.begin();
    radio.setAutoAck(false);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(pipeOut);
    resetData();
  } else {
    BluetoothMode = true;
    bleGamepad.begin();
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

  if (BluetoothMode) {
    if (bleGamepad.isConnected()) {

      int mapThrottle = mapConstrain(analogRead(throttle_in), 1400, 3200, 0, 32767, 16383, 2400, 2400, throttle_fine);
      int mapYaw = mapConstrain(analogRead(yaw_in), 400, 3500, 0, 32767, 16383, 1650, 2250, yaw_fine);
      int mapPitch = mapConstrain(analogRead(pitch_in), 500, 3300, 0, 32767, 16383, 1600, 2200, pitch_fine);
      int mapRoll = mapConstrain(analogRead(roll_in), 300, 3300, 32767, 0, 16383, 1200, 1800, roll_fine);
      int mapPot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 32767, 0, 16383, 2048, 2048);
      int AUX1 = digitalRead(toggle_1);
      int AUX2 = digitalRead(toggle_2);

      Serial.print("Bluetooth Throttle= "), Serial.print(mapThrottle);
      Serial.print(" Yaw= "), Serial.print(mapYaw);
      Serial.print(" Pitch= "), Serial.print(mapPitch);
      Serial.print(" Roll= "), Serial.print(mapRoll);
      Serial.print(" AUX1= "), Serial.print(AUX1);
      Serial.print(" AUX2= "), Serial.print(AUX2);
      Serial.print(" Pot1= "), Serial.println(mapPot1);

      if (!AUX1)
        bleGamepad.press(BUTTON_1);
      else
        bleGamepad.release(BUTTON_1);

      if (!AUX2)
        bleGamepad.press(BUTTON_2);
      else
        bleGamepad.release(BUTTON_2);

      bleGamepad.setAxes(mapYaw, mapThrottle, mapPitch, mapRoll, mapPot1, 32767, 32767, 32767);
    }
  } else {

    //analogRead(buttons_analog_in);
    //Serial.print("Btn= "), Serial.print(button_read);

    //Reset buttons
    if (button_read > 820) {
      yaw_decrease = false;
      throttle_decrease = false;
      pitch_decrease = false;
      roll_decrease = false;
      yaw_increase = false;
      throttle_increase = false;
      pitch_increase = false;
      roll_increase = false;
    }

    //------------------------------------THROTTLE buttons------------------------------------
    if (button_read < 500 && button_read > 430 && !throttle_decrease) {
      throttle_fine = throttle_fine + 1;
      throttle_decrease = true;
      updateValue("throttle_fine", throttle_fine);
      sound ? melody1() : (void)0;
    }
    if (button_read < 380 && button_read > 320 && !throttle_increase) {
      throttle_fine = throttle_fine - 1;
      throttle_increase = true;
      updateValue("throttle_fine", throttle_fine);
      sound ? melody1() : (void)0;
    }

    //------------------------------------YAW buttons------------------------------------
    if (button_read < 260 && button_read > 200 && !yaw_decrease) {
      yaw_fine = yaw_fine + 1;
      yaw_decrease = true;
      updateValue("yaw_fine", yaw_fine);
      sound ? melody1() : (void)0;
    }
    if (button_read < 120 && button_read > 50 && !yaw_increase) {
      yaw_fine = yaw_fine - 1;
      yaw_increase = true;
      updateValue("yaw_fine", yaw_fine);
      sound ? melody1() : (void)0;
    }

    //------------------------------------PITCH buttons------------------------------------
    if (button_read < 610 && button_read > 550 && !pitch_decrease) {
      pitch_fine = pitch_fine + 1;
      pitch_decrease = true;
      updateValue("pitch_fine", pitch_fine);
      sound ? melody1() : (void)0;
    }
    if (button_read < 690 && button_read > 630 && !pitch_increase) {
      pitch_fine = pitch_fine - 1;
      pitch_increase = true;
      updateValue("pitch_fine", pitch_fine);
      sound ? melody1() : (void)0;
    }

    //------------------------------------ROLL buttons------------------------------------
    if (button_read < 820 && button_read > 760 && !roll_decrease) {
      roll_fine = roll_fine + 1;
      roll_decrease = true;
      updateValue("roll_fine", roll_fine);
      sound ? melody1() : (void)0;
    }
    if (button_read < 760 && button_read > 700 && !roll_increase) {
      roll_fine = roll_fine - 1;
      roll_increase = true;
      updateValue("roll_fine", roll_fine);
      sound ? melody1() : (void)0;
    }

    //Invert channels
    //------------------------------------THROTTLE INVERT------------------------------------
    if (button_read < 500 && button_read > 430) {
      if (invert_counter > 30) {
        throttle_inverted = !throttle_inverted;
        invert_counter = 0;
        updateValueBool("throttle_inverted", throttle_inverted);
        sound ? melody1() : (void)0;
        delay(1500);
      }
      invert_counter = invert_counter + 1;
    }

    //------------------------------------YAW INVERT------------------------------------
    if (button_read < 260 && button_read > 200) {
      if (invert_counter > 30) {
        yaw_inverted = !yaw_inverted;
        invert_counter = 0;
        updateValueBool("yaw_inverted", yaw_inverted);
        sound ? melody1() : (void)0;
        delay(1500);
      }
      invert_counter = invert_counter + 1;
    }

    //------------------------------------PITCH INVERT------------------------------------
    if (button_read < 610 && button_read > 550) {
      if (invert_counter > 30) {
        pitch_inverted = !pitch_inverted;
        invert_counter = 0;
        updateValueBool("pitch_inverted", pitch_inverted);
        sound ? melody1() : (void)0;
        delay(1500);
      }
      invert_counter = invert_counter + 1;
    }

    //------------------------------------ROLL INVERT------------------------------------
    if (button_read < 820 && button_read > 760) {
      if (invert_counter > 30) {
        roll_inverted = !roll_inverted;
        invert_counter = 0;
        updateValueBool("roll_inverted", roll_inverted);
        sound ? melody1() : (void)0;
        delay(1500);
      }
      invert_counter = invert_counter + 1;
    }

    data.throttle = mapConstrain(analogRead(throttle_in), 1400, 3200, 255, 0, 127, 2400, 2400, throttle_fine);
    data.yaw = mapConstrain(analogRead(yaw_in), 400, 3500, 0, 255, 127, 1650, 2250, yaw_fine);
    data.pitch = mapConstrain(analogRead(pitch_in), 500, 3300, 0, 255, 127, 1600, 2200, pitch_fine);
    data.roll = mapConstrain(analogRead(roll_in), 300, 3300, 255, 0, 127, 1200, 1800, roll_fine);
    data.pot1 = mapConstrain(analogRead(pot1_in), 0, 4095, 255, 0, 127, 2048, 2048);
    //data.pot2 = mapConstrain(analogRead(pot1_in), 0, 4095, 255, 0, 127, 2048, 2048);
    data.AUX1 = !digitalRead(toggle_1);
    data.AUX2 = data.pot1;  //digitalRead(toggle_2);

    Serial.print(" Throttle= "), Serial.print(data.throttle);
    Serial.print(" Yaw= "), Serial.print(data.yaw);
    Serial.print(" Pitch= "), Serial.print(data.pitch);
    Serial.print(" Roll= "), Serial.print(data.roll);
    Serial.print(" AUX1= "), Serial.print(data.AUX1);
    Serial.print(" AUX2= "), Serial.print(data.AUX2);
    Serial.print(" Pot1= "), Serial.println(data.pot1);
    //Serial.print(" Pot2= "), Serial.print(data.pot1);

    radio.write(&data, sizeof(MyData));
  }
}