#include <Arduino.h>
#include <lx16a-servo.h>

LX16ABus servoBus;

// Left Arm
LX16AServo servo_left_1(&servoBus, 7);
LX16AServo servo_left_2(&servoBus, 8);
LX16AServo servo_left_3(&servoBus, 9);
LX16AServo servo_left_4(&servoBus, 10);
LX16AServo servo_left_5(&servoBus, 11);

float divisor = 4.1;

void setup() {
  servoBus.begin(&Serial2, 18, 19);
  Serial.begin(115200);
  servoBus.retry = 1;
  servoBus.debug(false);
  Serial.println("Formato: LEFT:90,45,120,30,60,0");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("LEFT:")) {
      input.remove(0, 5); // quitar "LEFT:"
      input.trim();

      int angles[5] = {0};
      int index = 0;

      while (input.length() > 0 && index < 6) {
        int commaIndex = input.indexOf(',');
        String angleStr;

        if (commaIndex != -1) {
          angleStr = input.substring(0, commaIndex);
          input = input.substring(commaIndex + 1);
        } else {
          angleStr = input;
          input = "";
        }

        angleStr.trim();
        angles[index] = angleStr.toInt();
        index++;
      }

      if (index == 5) {
        // Validación de rango
        bool valid = true;
        for (int i = 0; i < 5; i++) {
          if (angles[i] < 0 || angles[i] > 240) {
            valid = false;
            break;
          }
        }

        if (valid) {
          servo_left_1.move_time_and_wait_for_sync(angles[0] * 24 * divisor, 500 * divisor);
          servo_left_2.move_time_and_wait_for_sync(angles[1] * 24 * divisor, 500 * divisor);
          servo_left_3.move_time_and_wait_for_sync(angles[2] * 24 * divisor, 500 * divisor);
          servo_left_4.move_time_and_wait_for_sync(angles[3] * 24 * divisor, 500 * divisor);
          servo_left_5.move_time_and_wait_for_sync(angles[4] * 24 * divisor, 500 * divisor);
          servoBus.move_sync_start();
          

          Serial.println("Brazo izquierdo movido.");
        } else {
          Serial.println("Error: todos los ángulos deben estar entre 0 y 240.");
        }
      } else {
        Serial.println("Error: se requieren 6 ángulos separados por comas.");
      }
    }
  }
}
