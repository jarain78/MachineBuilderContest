
void initRobot() {
  servoBus.begin(&Serial1, 18, 19);          
  Serial.begin(115200);
  servoBus.retry = 1;  // enforce synchronous real time
  servoBus.debug(true);
}

