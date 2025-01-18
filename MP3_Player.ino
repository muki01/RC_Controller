void MP3_Player_init_TF() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

// Max 30
void MP3_Player_setVolume(int volume) {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, volume, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_playNext() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_playPrevious() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_playTrack(int track) {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, track, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

//   0:Normal   1:Pop   2:Rock   3:Jazz   4:Classic   5:Bass
void MP3_Player_setEQ(int mode) {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x07, 0x00, 0x00, mode, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_repeatTrack(int track) {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x08, 0x00, 0x00, track, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_play() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_pause() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xEF };
  MP3_Player.write(message, sizeof(message));
}

void MP3_Player_sleepMode() {
  byte message[8] = { 0x7E, 0xFF, 0x06, 0x0A, 0x00, 0x00, 0x00, 0xEF };
  MP3_Player.write(message, sizeof(message));
}