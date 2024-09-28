void resetData() {
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.

  data.throttle = 127;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
}


// int map_normal(int val, int lower, int middle, int upper, bool reverse) {
//   val = constrain(val, lower, upper);
//   if (val < middle)
//     val = map(val, lower, middle, 0, 128);
//   else
//     val = map(val, middle, upper, 128, 255);
//   return (reverse ? 255 - val : val);
// }


// // Returns a corrected value for a joystick position that takes into account
// // the values of the outer extents and the middle of the joystick range.
// int map_exponential(int val, bool reverse) {
//   val = constrain(val, 0, 1023);
//   float cube = ((pow((val - 512), 3) / 520200) + 258.012) / 2;
//   return (reverse ? 255 - cube : cube);
// }



void startMelody() {
  digitalWrite(buzzer, HIGH);
  delay(40);
  digitalWrite(buzzer, LOW);
  delay(40);
  digitalWrite(buzzer, HIGH);
  delay(40);
  digitalWrite(buzzer, LOW);
}

void melody1() {
  digitalWrite(buzzer, HIGH);
  delay(50);
  digitalWrite(buzzer, LOW);
}