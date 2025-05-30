void beginNRF24() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
};

void beginBluetooth() {
  BleGamepadConfiguration bleGamepadConfig;
  //bleGamepadConfig.setControllerType(CONTROLLER_TYPE_MULTI_AXIS); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepadConfig.setWhichAxes(true, true, false, true, true, false, true, true);  // X,Y,Z,RX,RY,RZ,Slider1,Slider2
  bleGamepadConfig.setButtonCount(4);
  bleGamepadConfig.setHatSwitchCount(0);
  bleGamepad.begin(&bleGamepadConfig);
};

void battery() {
  float battery = (double)analogRead(battery_in) / 4096.0 * 9.6;
  smoothedBatteryLevel.add(battery);
  batteryLevel = smoothedBatteryLevel.get();

  if (batteryLevel >= 8.1 && !warningFull) {
    Serial.print("Battery is full. Ready to go! ");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(12);
    bleGamepad.setBatteryLevel(100);
    warningFull = true;  // Set the warning flag to true
  } else if (batteryLevel >= 7.6 && batteryLevel < 8.1 && !warningHigh) {
    Serial.print("Battery level is high. No need to charge. ");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(13);
    bleGamepad.setBatteryLevel(80);
    warningHigh = true;  // Set the warning flag to true
  } else if (batteryLevel >= 7.3 && batteryLevel < 7.6 && !warningMedium) {
    Serial.print("Battery level is medium. Consider charging soon. ");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(14);
    bleGamepad.setBatteryLevel(60);
    warningMedium = true;  // Set the warning flag to true
  } else if (batteryLevel >= 6.9 && batteryLevel < 7.3 && !warningLow) {
    Serial.print("Warning! Battery level is low. Charge when possible. ");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(15);
    bleGamepad.setBatteryLevel(40);
    warningLow = true;  // Set the warning flag to true
  } else if (batteryLevel >= 6.7 && batteryLevel < 6.9 && !warningVeryLow) {
    Serial.print("Critical warning! Battery is very low. Charge immediately! ");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(10);
    bleGamepad.setBatteryLevel(20);
    warningVeryLow = true;  // Set the warning flag to true
  } else if (batteryLevel < 6.7 && !warningCritical) {
    Serial.print("Battery is about to die! Shutting down soon!");
    Serial.println(batteryLevel);
    MP3_Player_playTrack(11);
    bleGamepad.setBatteryLevel(10);
    warningCritical = true;  // Set the warning flag to true
  }
}

void printData() {
  Serial.print(" Throttle= "), Serial.print(Throttle);
  Serial.print(" Yaw= "), Serial.print(Yaw);
  Serial.print(" Pitch= "), Serial.print(Pitch);
  Serial.print(" Roll= "), Serial.print(Roll);
  Serial.print(" Pot1= "), Serial.print(Pot1);
  Serial.print(" Pot2= "), Serial.print(Pot2);
  Serial.print(" AUX1= "), Serial.print(AUX1);
  Serial.print(" AUX2= "), Serial.print(AUX2);
  Serial.print(" AUX3= "), Serial.print(AUX3);
  Serial.print(" AUX4= "), Serial.print(AUX4);
  Serial.print(" Switch1= "), Serial.print(Switch1);
  Serial.print(" Switch2= "), Serial.print(Switch2);
  Serial.print(" Battery= "), Serial.println(batteryLevel);
}

void buttons() {
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
  }
  if (button_read < 380 && button_read > 320 && !throttle_increase) {
    throttle_fine = throttle_fine - 1;
    throttle_increase = true;
    updateValue("throttle_fine", throttle_fine);
  }

  //------------------------------------YAW buttons------------------------------------
  if (button_read < 260 && button_read > 200 && !yaw_decrease) {
    yaw_fine = yaw_fine + 1;
    yaw_decrease = true;
    updateValue("yaw_fine", yaw_fine);
  }
  if (button_read < 120 && button_read > 50 && !yaw_increase) {
    yaw_fine = yaw_fine - 1;
    yaw_increase = true;
    updateValue("yaw_fine", yaw_fine);
  }

  //------------------------------------PITCH buttons------------------------------------
  if (button_read < 610 && button_read > 550 && !pitch_decrease) {
    pitch_fine = pitch_fine + 1;
    pitch_decrease = true;
    updateValue("pitch_fine", pitch_fine);
  }
  if (button_read < 690 && button_read > 630 && !pitch_increase) {
    pitch_fine = pitch_fine - 1;
    pitch_increase = true;
    updateValue("pitch_fine", pitch_fine);
  }

  //------------------------------------ROLL buttons------------------------------------
  if (button_read < 820 && button_read > 760 && !roll_decrease) {
    roll_fine = roll_fine + 1;
    roll_decrease = true;
    updateValue("roll_fine", roll_fine);
  }
  if (button_read < 760 && button_read > 700 && !roll_increase) {
    roll_fine = roll_fine - 1;
    roll_increase = true;
    updateValue("roll_fine", roll_fine);
  }

  //Invert channels
  //------------------------------------THROTTLE INVERT------------------------------------
  if (button_read < 500 && button_read > 430) {
    if (invert_counter > 30) {
      throttle_inverted = !throttle_inverted;
      invert_counter = 0;
      updateValueBool("throttle_inverted", throttle_inverted);
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
      delay(1500);
    }
    invert_counter = invert_counter + 1;
  }
}

void resetData() {
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
  data.AUX3 = 0;
  data.AUX4 = 0;
  data.pot1 = 0;
  data.pot2 = 0;
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
  if (input)
    bleGamepad.press(button);
  else
    bleGamepad.release(button);
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