#include "Config.h"

// Función para mover el servo LX16A
void moveServo(uint8_t id, uint16_t position) {
  uint8_t pos_low = position & 0xFF;
  uint8_t pos_high = (position >> 8) & 0xFF;
  uint8_t checksum = ~(id + 7 + 3 + pos_low + pos_high) & 0xFF;

  uint8_t packet[] = {0x55, 0x55, id, 7, 3, pos_low, pos_high, 0, 0};
  packet[7] = checksum;

  lx16aSerial.write(packet, 8);
}

// Función para procesar los mensajes
void parseMessage(const String& msg) {
  if (msg.startsWith("<CMD>:")) {
    String data = msg.substring(6); // Saltar "<CMD>:"

    if (data.startsWith("GOTO=")) {
      int eq = data.indexOf('=');
      int sc = data.indexOf(';');
      int value = data.substring(eq + 1, sc).toInt();
      Serial.print("Mover servo a posición: ");
      Serial.println(value);
      moveServo(1, value); // ID del servo = 1
      Serial.println("<ACK>:GOTO_OK;");
    }
    else if (data.startsWith("ML_DATA=[")) {
      int start = data.indexOf('[') + 1;
      int end = data.indexOf(']');
      String values = data.substring(start, end);

      int i = 0;
      while (values.length() > 0 && i < 50) {
        int comma = values.indexOf(',');
        String value_str;
        if (comma == -1) {
          value_str = values;
          values = "";
        } else {
          value_str = values.substring(0, comma);
          values = values.substring(comma + 1);
        }
        ml_data[i++] = value_str.toFloat();
      }
      Serial.println("Datos de ML almacenados.");
      Serial.println("<ACK>:ML_DATA_OK;");
    }
  }
  else if (msg.startsWith("<ML_RESULT>:")) {
    String data = msg.substring(12);

    int label_start = data.indexOf("LABEL=\"") + 7;
    int label_end = data.indexOf('\"', label_start);
    ml_label = data.substring(label_start, label_end);

    int prob_start = data.indexOf("PROB=") + 5;
    int prob_end = data.indexOf(';', prob_start);
    if (prob_end == -1) prob_end = data.length();
    ml_prob = data.substring(prob_start, prob_end).toFloat();

    Serial.print("Resultado ML: ");
    Serial.print(ml_label);
    Serial.print(" (");
    Serial.print(ml_prob, 2);
    Serial.println(")");
    Serial.println("<ACK>:ML_RESULT_OK;");
  }
}

void setup() {
  Serial.begin(115200);
  lx16aSerial.begin(115200);

  Serial.println("Arduino listo para recibir comandos...");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseMessage(serialBuffer);
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
}
