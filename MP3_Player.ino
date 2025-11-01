void MP3_Player_playNext() {
  sendCommand(0x01, 0);
}

void MP3_Player_playPrevious() {
  sendCommand(0x02, 0);
}

void MP3_Player_playTrack(int track) {
  sendCommand(0x03, track);
}

void MP3_Player_volumeUp() {
  sendCommand(0x04, 0);
}

void MP3_Player_volumeDown() {
  sendCommand(0x05, 0);
}

// Max 30
void MP3_Player_setVolume(int volume) {
  sendCommand(0x06, volume);
}

//   0:Normal   1:Pop   2:Rock   3:Jazz   4:Classic   5:Bass
void MP3_Player_setEQ(int mode) {
  sendCommand(0x07, mode);
}

void MP3_Player_repeatTrack(int track) {
  sendCommand(0x08, track);
}

void MP3_Player_init_TF() {
  sendCommand(0x09, 2);
}

void MP3_Player_sleepMode() {
  sendCommand(0x0A, 0);
}

void MP3_Player_play() {
  sendCommand(0x0D, 0);
}

void MP3_Player_pause() {
  sendCommand(0x0E, 0);
}



void sendCommand(uint8_t cmd, int parData) {
  uint8_t commandData[10];  //This holds all the command data to be sent
  int checkSum;

  commandData[0] = 0x7E;               //Start of new command
  commandData[1] = 0xFF;               //Version information
  commandData[2] = 0x06;               //Data length (not including parity) or the start and version
  commandData[3] = cmd;                //The command that was sent through
  commandData[4] = 0x00;               //1 = feedback
  commandData[5] = highByte(parData);  //High byte of the data sent over
  commandData[6] = lowByte(parData);   //low byte of the data sent over
  checkSum = -(commandData[1] + commandData[2] + commandData[3] + commandData[4] + commandData[5] + commandData[6]);
  commandData[7] = highByte(checkSum);  //High byte of the checkSum
  commandData[8] = lowByte(checkSum);   //low byte of the checkSum
  commandData[9] = 0xEF;                //End bit

  // Serial.print("Sending:");
  // for (int i = 0; i < sizeof(command); i++) {
  //   if (command[i] < 0x10) Serial.print("0");
  //   Serial.print(command[i], HEX);
  //   Serial.print(" ");
  // }
  // Serial.println();

  MP3_Player.write(commandData, sizeof(commandData));
}
