//Inputs outputs
#define throttle_in 2
#define yaw_in 4
#define pitch_in 34
#define roll_in 39

#define pot1_in 35
#define pot2_in 15
#define toggle_1 32
#define toggle_2 12
#define switch_1 22
#define switch_2 36

#define battery_in 33
#define buttons_analog_in 27
#define buzzer 12
#define led 0


void setup() {
  Serial.begin(115200);

  pinMode(throttle_in, INPUT);
  pinMode(yaw_in, INPUT);
  pinMode(pitch_in, INPUT);
  pinMode(roll_in, INPUT);
  pinMode(pot1_in, INPUT);
  pinMode(pot2_in, INPUT);
  pinMode(toggle_1, INPUT_PULLUP);
  pinMode(toggle_2, INPUT_PULLUP);
  pinMode(switch_1, INPUT_PULLUP);
  pinMode(switch_2, INPUT_PULLUP);

  pinMode(battery_in, INPUT);
  pinMode(buttons_analog_in, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}


void loop() {
  int throttle = analogRead(throttle_in);
  int yaw = analogRead(yaw_in);
  int pitch = analogRead(pitch_in);
  int roll = analogRead(roll_in);
  int pot1 = analogRead(pot1_in);
  int pot2 = analogRead(pot2_in);
  int toggle1 = digitalRead(toggle_1);
  int toggle2 = digitalRead(toggle_2);
  int switch1 = digitalRead(switch_1);
  int switch2 = digitalRead(switch_2);

  Serial.print(" Throttle= "), Serial.print(throttle);
  Serial.print(" Yaw= "), Serial.print(yaw);
  Serial.print(" Pitch= "), Serial.print(pitch);
  Serial.print(" Roll= "), Serial.print(roll);
  Serial.print(" AUX1= "), Serial.print(toggle1);
  Serial.print(" AUX2= "), Serial.print(toggle2);
  Serial.print(" Pot1= "), Serial.print(pot1);
  Serial.print(" Pot2= "), Serial.print(pot2);
  Serial.print(" Switch1= "), Serial.print(switch1);
  Serial.print(" Switch2= "), Serial.println(switch2);
}