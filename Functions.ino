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


int mapConstrain(int input, int in_min, int in_max, int out_min, int out_max, int center, int deadzone_low, int deadzone_high, int fine) {
  int mappedValue;
  if (input >= deadzone_low && input <= deadzone_high) {
    return center;
  } else if (input < deadzone_low) {
    mappedValue = map(input, in_min, deadzone_low, out_min, center);
  } else if (input > deadzone_low) {
    mappedValue = map(input, in_max, deadzone_high, out_max, center);
  }
  mappedValue = mappedValue + fine;
  return constrain(mappedValue, min(out_min, out_max), max(out_min, out_max));
}

float lowPassFilter(float previousValue, float newValue, float alpha) {
  return (alpha * newValue) + ((1 - alpha) * previousValue);
}

void handleButtonPress(int input, int button) {
  if (!input)
    bleGamepad.press(button);
  else
    bleGamepad.release(button);
}



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

void BlinkLed(int time, int count) {
  for (int i = 1; i <= count; i++) {
    digitalWrite(led, LOW);
    delay(time);
    digitalWrite(led, HIGH);
    if (i != count) {
      delay(time);
    }
  }
}