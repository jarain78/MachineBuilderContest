#include <Arduino.h>
#include <lx16a-servo.h>

bool flagLeftArm = false;
bool flagRightArm = false;

//**************************************************************************
LX16ABus servoBus;

// Left Arm
LX16AServo servo_left_1(&servoBus, 1);
LX16AServo servo_left_2(&servoBus, 2);
LX16AServo servo_left_3(&servoBus, 3);
LX16AServo servo_left_4(&servoBus, 4);
LX16AServo servo_left_5(&servoBus, 5);
LX16AServo servo_left_6(&servoBus, 6);

// Pos-Left Arm
int16_t pos_1 = 0;
int16_t pos_2 = 0;
int16_t pos_3 = 0;
int16_t pos_4 = 0;
int16_t pos_5 = 0;
int16_t pos_6 = 0;

// Righ Arm
LX16AServo servo_right_1(&servoBus, 7);
LX16AServo servo_right_2(&servoBus, 8);
LX16AServo servo_right_3(&servoBus, 9);
LX16AServo servo_right_4(&servoBus, 10);
LX16AServo servo_right_5(&servoBus, 11);
LX16AServo servo_right_6(&servoBus, 12);

// Pos-Right Arm
int16_t pos_7 = 0;
int16_t pos_8 = 0;
int16_t pos_9 = 0;
int16_t pos_10 = 0;
int16_t pos_11 = 0;
int16_t pos_12 = 0;


uint16_t getAngle(int angle, int divisor) {
  return (angle * 24 * divisor);
}

int16_t getPosServoRightArm(int servoIndex) {

  if (servoIndex == 7) {
    return servo_right_1.pos_read();
  } else if (servoIndex == 8) {
    return servo_right_2.pos_read();
  } else if (servoIndex == 9) {
    return servo_right_3.pos_read();
  } else if (servoIndex == 10) {
    return servo_right_4.pos_read();
  } else if (servoIndex == 11) {
    return servo_right_5.pos_read();
  } else if (servoIndex == 12) {
    return servo_right_6.pos_read();
  } else {
    return -1;
  }
}

int16_t getPosServoLeftArm(int servoIndex) {

  if (servoIndex == 1) {
    return servo_left_1.pos_read();
  } else if (servoIndex == 2) {
    return servo_left_2.pos_read();
  } else if (servoIndex == 3) {
    return servo_left_3.pos_read();
  } else if (servoIndex == 4) {
    return servo_left_4.pos_read();
  } else if (servoIndex == 5) {
    return servo_left_5.pos_read();
  } else if (servoIndex == 6) {
    return servo_left_6.pos_read();
  } else {
    return -1;
  }
}

void setup() {
  // Tx pin: 18
  // Rx pin = 19
  servoBus.begin(&Serial2, 18, 19);
  
  Serial.begin(115200);
  servoBus.retry = 1;  // enforce synchronous real time
  servoBus.debug(false);
}

void loop() {
  int16_t servo_l_pos = 0;
  int16_t servo_r_pos = 0;

  for (int servoIndex = 1; servoIndex < 7; servoIndex++) {
    servo_l_pos = getPosServoLeftArm(servoIndex);
    delay(15);
    Serial.print("Servo 1: ");
    Serial.println(servo_l_pos);
  }

  for (int servoIndex = 7; servoIndex < 13; servoIndex++) {

    servo_r_pos = getPosServoRightArm(servoIndex);
    delay(15);
    Serial.print("Servo 2: ");
    Serial.println(servo_r_pos);
  }
}
