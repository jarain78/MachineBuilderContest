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

// Right Arm
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

void setup() {
  // Tx pin: 18
  // Rx pin = 19
  servoBus.begin(&Serial2, 18, 19);

  Serial.begin(115200);  // Para comunicarse con el monitor serial
  servoBus.retry = 1;    // Tiempo real
  servoBus.debug(false);

  Serial.println("Ingrese un ángulo entre 0 y 240 grados:");
}

void loop() {
  static float divisor = 4.1;

  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');  // Leer la línea completa hasta salto de línea
    inputString.trim();                                 // Eliminar espacios extra

    if (inputString.length() > 0) {
      float angle_input = inputString.toFloat();  // Convertir el texto a número

      if (angle_input >= 0.0 && angle_input <= 240.0) {
        uint16_t angle = angle_input * 24 * divisor;

        // Mover el servo
        servo_left_1.move_time_and_wait_for_sync(angle, 10 * divisor);
        int32_t pos = servo_left_1.pos_read();
        Serial.print("Servo Pos: ");
        Serial.println(pos);
        servoBus.move_sync_start();

        Serial.print("Moviendo a ángulo: ");
        Serial.println(angle_input);
      } else {
        Serial.println("Error: ángulo fuera de rango (0 - 240).");
      }
    }
  }
}
