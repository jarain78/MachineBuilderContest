
void init_gsr() {

  analogReadResolution(12);  // Set analog input resolution to max, 12-bits
}

void read_gsr() {

  gsr = map(analogRead(A2), 0, 4096, 0, 255);
  Serial.println(gsr);
  data_block += String(gsr) + ",";
  delay(20);
}