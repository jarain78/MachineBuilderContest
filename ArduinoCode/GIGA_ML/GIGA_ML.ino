#include "Config.h"
#include "Test.h"


void initRobot();
uint16_t getAngle(int angle);
void pose_1();
void pose_2();
void pose_3();


void setup() {

  Serial.begin(115200);
  delay(4000);
  //tf.begin(ecg_model);

  initRobot();
}

void loop() {

  for (int i = 0; i < 10; i++) {
    output_data = tf.predictClass(x_test[i]);

    Serial.print("Predicted: ");
    Serial.println(output_data);
    delay(1000);
  }

  /*pose_1();
  delay(3000);
  pose_2();
  delay(3000);
  pose_3();
  delay(3000);
  pose_1();
  delay(6000);*/
}



void initRobot() {
  servoBus.begin(&Serial2, 18, 19);
  Serial.begin(115200);
  servoBus.retry = 1;  // enforce synchronous real time
  servoBus.debug(false);
}

uint16_t getAngle(int angle) {
  return (angle * 10 * divisor);
}


void pose_1() {
  uint16_t servo_angle_1 = getAngle(angles[0][0]);
  uint16_t servo_angle_2 = getAngle(angles[0][1]);
  uint16_t servo_angle_3 = getAngle(angles[0][2]);
  uint16_t servo_angle_4 = getAngle(angles[0][3]);
  uint16_t servo_angle_5 = getAngle(angles[0][4]);


  joint_1.move_time_and_wait_for_sync(servo_angle_1, 500 * divisor);
  joint_2.move_time_and_wait_for_sync(servo_angle_2, 500 * divisor);
  joint_3.move_time_and_wait_for_sync(servo_angle_3, 500 * divisor);
  joint_4.move_time_and_wait_for_sync(servo_angle_4, 500 * divisor);
  joint_5.move_time_and_wait_for_sync(servo_angle_5, 500 * divisor);

  servoBus.move_sync_start();
}



void pose_2() {
  uint16_t servo_angle_1 = getAngle(angles[1][0]);
  uint16_t servo_angle_2 = getAngle(angles[1][1]);
  uint16_t servo_angle_3 = getAngle(angles[1][2]);
  uint16_t servo_angle_4 = getAngle(angles[1][3]);
  uint16_t servo_angle_5 = getAngle(angles[1][4]);


  joint_1.move_time_and_wait_for_sync(servo_angle_1, 500 * divisor);
  joint_2.move_time_and_wait_for_sync(servo_angle_2, 500 * divisor);
  joint_3.move_time_and_wait_for_sync(servo_angle_3, 500 * divisor);
  joint_4.move_time_and_wait_for_sync(servo_angle_4, 500 * divisor);
  joint_5.move_time_and_wait_for_sync(servo_angle_5, 500 * divisor);

  servoBus.move_sync_start();
}


void pose_3() {
  uint16_t servo_angle_1 = getAngle(angles[2][0]);
  uint16_t servo_angle_2 = getAngle(angles[2][1]);
  uint16_t servo_angle_3 = getAngle(angles[2][2]);
  uint16_t servo_angle_4 = getAngle(angles[2][3]);
  uint16_t servo_angle_5 = getAngle(angles[2][4]);


  joint_1.move_time_and_wait_for_sync(servo_angle_1, 500 * divisor);
  joint_2.move_time_and_wait_for_sync(servo_angle_2, 500 * divisor);
  joint_3.move_time_and_wait_for_sync(servo_angle_3, 500 * divisor);
  joint_4.move_time_and_wait_for_sync(servo_angle_4, 500 * divisor);
  joint_5.move_time_and_wait_for_sync(servo_angle_5, 500 * divisor);

  servoBus.move_sync_start();
}
