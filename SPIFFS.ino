void initSpiffs() {
  if (!SPIFFS.begin()) {
    while (1) {
      //BlinkLed(1000, 100);
    }
  }
}

void readSettings() {
  if (SPIFFS.exists("/settings.conf")) {
    File configFile = SPIFFS.open("/settings.conf", "r");
    while (configFile.available()) {
      String line = configFile.readStringUntil('\n');
      if (line.startsWith("throttle_fine=")) throttle_fine = line.substring(14).toInt();
      else if (line.startsWith("yaw_fine=")) yaw_fine = line.substring(9).toInt();
      else if (line.startsWith("pitch_fine=")) pitch_fine = line.substring(11).toInt();
      else if (line.startsWith("roll_fine=")) roll_fine = line.substring(10).toInt();

      else if (line.startsWith("throttle_inverted=")) throttle_inverted = (line.substring(18) == "true");
      else if (line.startsWith("yaw_inverted=")) yaw_inverted = (line.substring(13) == "true");
      else if (line.startsWith("pitch_inverted=")) pitch_inverted = (line.substring(15) == "true");
      else if (line.startsWith("roll_inverted=")) roll_inverted = (line.substring(14) == "true");
    }
    configFile.close();
  }
}

void updateSetting(const String& key, const String& value) {
  if (SPIFFS.exists("/settings.conf")) {
    String updatedSettings = "";
    File configFile = SPIFFS.open("/settings.conf", "r");
    while (configFile.available()) {
      String line = configFile.readStringUntil('\n');
      if (line.startsWith(key)) {
        updatedSettings += key + "=" + value + "\n";
      } else {
        updatedSettings += line + "\n";
      }
    }
    configFile.close();
    configFile = SPIFFS.open("/settings.conf", "w");
    configFile.print(updatedSettings);
    configFile.close();
  }
}

void updateValue(String option, int value) {
  String valueString = String(value);
  updateSetting(option, valueString);
}

void updateValueBool(String option, bool value) {
  String valueString = value ? "true" : "false";
  updateSetting(option, valueString);
}
