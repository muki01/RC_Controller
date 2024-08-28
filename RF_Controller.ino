#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SPIFFS.h>

//Inputs outputs
#define battery_in 32
#define buttons_analog_in 33
#define toggle_1 25
#define toggle_2 26
#define throttle_in 27
#define yaw_in 14
#define pitch_in 12
#define roll_in 13
#define mode_in 23
#define buzzer 22

#define ce_Pin 21
#define csnPin 19

const uint64_t pipeOut = 0xE8E8F0F0E1LL;  //IMPORTANT: The same as in the receiver!!!
RF24 radio(ce_Pin, csnPin);               // select  CSN  pin

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
};

MyData data;

//Variables
float battery_level = 0;
int button_read = 0;

int throttle_fine = 0, yaw_fine = 0, pitch_fine = 0, roll_fine = 0;
int throttle_to_send = 0, yaw_to_send = 0, pitch_to_send = 0, roll_to_send = 0;
bool throttle_inverted = false, yaw_inverted = true, pitch_inverted = true, roll_inverted = false;
bool throttle_decrease = false, yaw_decrease = false, pitch_decrease = false, roll_decrease = false;
bool throttle_increase = false, yaw_increase = false, pitch_increase = false, roll_increase = false;

bool mode = true;
bool mode_button_pressed = false;
bool sound = true;
int counter = 0;
int invert_counter = 0;
bool sound_changed = false;

void setup() {
  Serial.begin(115200);
  initSpiffs();
  readSettings();

  pinMode(buttons_analog_in, INPUT);
  pinMode(mode_in, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  startMelody();

  //Start everything up
  radio.begin();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();
}


void loop() {
  Serial.print("Throttle_fine= "), Serial.print(throttle_fine);
  Serial.print(" Yaw_fine= "), Serial.print(yaw_fine);
  Serial.print(" Pitch_fine= "), Serial.print(pitch_fine);
  Serial.print(" Roll_fine= "), Serial.print(roll_fine);
  Serial.print(" Throttle_inverted= "), Serial.print(throttle_inverted);
  Serial.print(" Yaw_inverted= "), Serial.print(yaw_inverted);
  Serial.print(" Pitch_inverted= "), Serial.print(pitch_inverted);
  Serial.print(" Roll_inverted= "), Serial.print(roll_inverted);

  //battery read
  battery_level = analogRead(battery_in) / 67.331;  //Voltage divider is 10k and 20K so 1/3


  //Buttons read
  button_read = analogRead(buttons_analog_in);
  Serial.print(" Btn= "), Serial.println(button_read);
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

  //------------------------------------Mode select button------------------------------------
  if (!digitalRead(mode_in) && !mode_button_pressed) {
    mode = !mode;
    mode_button_pressed = true;
    sound ? melody1() : (void)0;
  }

  if (!digitalRead(mode_in) && !sound_changed) {
    if (counter > 20) {
      sound = !sound;
      counter = 0;
      sound_changed = true;
      sound ? melody1() : (void)0;
    }
    counter = counter + 1;
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





  if (digitalRead(mode_in) && mode_button_pressed) {
    mode_button_pressed = false;
    sound_changed = false;
    counter = 0;
    invert_counter = 0;
  }


  //Mode select
  if (!mode) {
    throttle_to_send = map_normal(analogRead(throttle_in), 0, 512, 1023, throttle_inverted);
    yaw_to_send = map_normal(analogRead(yaw_in), 0, 512, 1023, yaw_inverted);
    pitch_to_send = map_normal(analogRead(pitch_in), 0, 512, 1023, pitch_inverted);
    roll_to_send = map_normal(analogRead(roll_in), 0, 512, 1023, roll_inverted);
  }

  if (mode) {
    throttle_to_send = map_exponential(analogRead(throttle_in), throttle_inverted);
    yaw_to_send = map_exponential(analogRead(yaw_in), yaw_inverted);
    pitch_to_send = map_exponential(analogRead(pitch_in), pitch_inverted);
    roll_to_send = map_exponential(analogRead(roll_in), roll_inverted);
  }


  throttle_to_send = throttle_to_send + throttle_fine - 127;
  yaw_to_send = yaw_to_send + yaw_fine - 127;
  pitch_to_send = pitch_to_send + pitch_fine - 127;
  roll_to_send = roll_to_send + roll_fine - 127;


  data.throttle = constrain(throttle_to_send, 0, 255);
  data.yaw = constrain(yaw_to_send, 0, 255);
  data.pitch = constrain(pitch_to_send, 0, 255);
  data.roll = constrain(roll_to_send, 0, 255);
  data.AUX1 = digitalRead(toggle_1);
  data.AUX2 = digitalRead(toggle_2);

  radio.write(&data, sizeof(MyData));
}