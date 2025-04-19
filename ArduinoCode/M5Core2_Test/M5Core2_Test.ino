#include <M5Core2.h>

// Blanco --> Rojo
// Amarillo --> Gris

// Pines de comunicación (UART2)
#define CMD_RX 14  // RX2
#define CMD_TX 13  // TX2

HardwareSerial cmdSerial(2);  // UART2 para recibir comandos
String serialBuffer = "";

void showCommand(const String& cmd) {
  M5.Lcd.setTextColor(WHITE, BLACK);       // Texto blanco, fondo negro
  M5.Lcd.fillRect(0, 30, 320, 30, BLACK);  // Limpia el área
  M5.Lcd.setCursor(10, 30);
  M5.Lcd.print("Cmd: ");
  M5.Lcd.println(cmd);
}

void showAction(const String& action) {
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.fillRect(0, 70, 320, 30, BLACK);  // Limpia el área
  M5.Lcd.setCursor(10, 70);
  M5.Lcd.print("Action: ");
  M5.Lcd.println(action);
}

// Función que interpreta y simula acciones basadas en el comando
void parseMessage(const String& msg) {
  if (msg.startsWith("<CMD>:")) {
    String data = msg.substring(6);  // elimina "<CMD>:"

    if (data.startsWith("GOTO=")) {
      int value = data.substring(5, data.indexOf(';')).toInt();
      Serial.printf("[INFO] Simulando movimiento a posición %d\n", value);
      showAction("Move to " + String(value));
    } else if (data.startsWith("ML_DATA=[")) {
      Serial.println("[INFO] Datos ML recibidos");
      showAction("Received ML Data");
    }
  } else if (msg.startsWith("<ML_RESULT>:")) {
    String data = msg.substring(12);

    int label_start = data.indexOf("LABEL=\"") + 7;
    int label_end = data.indexOf('\"', label_start);
    String label = data.substring(label_start, label_end);

    int prob_start = data.indexOf("PROB=") + 5;
    float prob = data.substring(prob_start, data.indexOf(';', prob_start)).toFloat();

    Serial.printf("[INFO] Resultado ML: %s (%.2f)\n", label.c_str(), prob);
    showAction("ML: " + label + " (" + String(prob * 100, 1) + "%)");
  }
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  cmdSerial.begin(115200, SERIAL_8N1, CMD_RX, CMD_TX);

  // Inicializar pantalla
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.println("Waiting for commands...");
}

void loop() {
  while (cmdSerial.available()) {
    char c = cmdSerial.read();
    if (c == '\n') {
      showCommand(serialBuffer);
      parseMessage(serialBuffer);
      serialBuffer = "";  // Limpiar
    } else {
      serialBuffer += c;
    }
  }
  delay(5);  // Pequeño delay para estabilidad
}
