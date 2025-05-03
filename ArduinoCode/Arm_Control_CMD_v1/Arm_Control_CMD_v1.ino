#include <Arduino.h>
#include <lx16a-servo.h>

LX16ABus servoBus;

// joint Arm (IDs 7–11)
LX16AServo servo_joint_1(&servoBus, 7);
LX16AServo servo_joint_2(&servoBus, 8);
LX16AServo servo_joint_3(&servoBus, 9);
LX16AServo servo_joint_4(&servoBus, 10);
LX16AServo servo_joint_5(&servoBus, 11);

float divisor = 4.1;

void setup() {
  servoBus.begin(&Serial2, 18, 19);
  Serial.begin(115200);
  servoBus.retry = 1;
  servoBus.debug(false);
  Serial.println("Esperando comandos tipo: <JOINT>:90,45,120,30,60");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int colonIndex = input.indexOf(':');
    if (colonIndex == -1) {
      Serial.println("⚠️ Formato inválido. Usa <CMD>:valores");
      return;
    }

    String cmd = input.substring(0, colonIndex);
    String data = input.substring(colonIndex + 1);
    cmd.trim();
    data.trim();

    if (cmd == "JOINT") {
      int angles[5] = {0};
      int index = 0;

      while (data.length() > 0 && index < 5) {
        int commaIndex = data.indexOf(',');
        String angleStr;

        if (commaIndex != -1) {
          angleStr = data.substring(0, commaIndex);
          data = data.substring(commaIndex + 1);
        } else {
          angleStr = data;
          data = "";
        }

        angleStr.trim();
        angles[index] = angleStr.toInt();
        index++;
      }

      if (index == 5) {
        bool valid = true;
        for (int i = 0; i < 5; i++) {
          if (angles[i] < 0 || angles[i] > 240) {
            valid = false;
            break;
          }
        }

        if (valid) {
          servo_joint_1.move_time_and_wait_for_sync(angles[0] * 24 * divisor, 500 * divisor);
          servo_joint_2.move_time_and_wait_for_sync(angles[1] * 24 * divisor, 500 * divisor);
          servo_joint_3.move_time_and_wait_for_sync(angles[2] * 24 * divisor, 500 * divisor);
          servo_joint_4.move_time_and_wait_for_sync(angles[3] * 24 * divisor, 500 * divisor);
          servo_joint_5.move_time_and_wait_for_sync(angles[4] * 24 * divisor, 500 * divisor);
          servoBus.move_sync_start();

          Serial.println("✅ Movimiento de articulaciones completado.");
        } else {
          Serial.println("❌ Error: ángulos fuera de rango (0–240).");
        }
      } else {
        Serial.println("❌ Error: se requieren 5 ángulos.");
      }
    } else {
      Serial.print("⚠️ Comando no reconocido: ");
      Serial.println(cmd);
    }
  }
}
