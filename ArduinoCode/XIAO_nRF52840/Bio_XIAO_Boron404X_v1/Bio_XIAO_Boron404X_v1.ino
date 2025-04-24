#include "Config.h"

void setup() {
  Serial.begin(115200);
  bdmSerial.begin(57600);
  boron_404X.begin(57600);

  //initMAX();

  Serial.println("🟢 Sistema iniciado. Capturando ECG cada 30s...");
}

void loop() {
  checkSerialRequest();

  if (capturing) {
    if (current_signal_type == "ECG") {
      read_bdm_101();
    } else if (current_signal_type == "PPG") {
      getHeartRate();
    } else if (current_signal_type == "GSR") {
      read_gsr();
    }
    if (millis() - last_capture_time >= capture_interval) {
      finishCapture();
    }
  }
}

void resetBuffer() {
  memset(ecgBuffer, 0, sizeof(ecgBuffer));
  memset(gsrBuffer, 0, sizeof(gsrBuffer));
}

// === VERIFICA PETICIÓN SERIAL ===
/*void checkSerialRequest() {
  if (boron_404X.available()) {
    String req = boron_404X.readStringUntil('\n');
    req.trim();

    if (req.startsWith("<REQ>:")) {
      current_signal_type = req.substring(6);
      startCapture(current_signal_type);
    }
  }
}*/

// === INICIA CAPTURA ===
void startCapture(String type) {
  data_block = "";
  capturing = true;
  last_capture_time = millis();
  last_sample_time = millis();
  Serial.println("🎯 Iniciando captura de: " + type);
}

// === TERMINA Y ENVÍA DATOS ===
void finishCapture() {
  capturing = false;
  if (data_block.endsWith(",")) data_block.remove(data_block.length() - 1);
  sendInChunks(data_block, CHUNK_SIZE);
  data_block = "";
  Serial.println("✅ Captura enviada.");
}

// === ENVÍA DATOS EN PAQUETES ===
void sendInChunks(String full_data, int chunk_size) {
  int total_chunks = (full_data.length() + chunk_size - 1) / chunk_size;
  int chunk_id = 0;
  int start = 0;

  while (start < full_data.length()) {
    int end = min(start + chunk_size, full_data.length());
    String payload = full_data.substring(start, end);

    String packet = "<DATA>:id=" + String(chunk_id);
    packet += ";total=" + String(total_chunks);
    packet += ";payload=" + payload + ";\r";

    boron_404X.println(packet);
    Serial.println(packet);
    delay(1000);

    start = end;
    chunk_id++;
  }
}


void checkSerialRequest() {
  if (boron_404X.available()) {
    String req = boron_404X.readStringUntil('\n');
    req.trim();

    if (req.startsWith("<REQ>:")) {
      current_signal_type = req.substring(6);
      startCapture(current_signal_type);

      Serial.print(current_signal_type);
      // Enviar ACK de confirmación
      boron_404X.println("<ACK>\r");
      Serial.println("📥 ACK enviado a Boron 404X.");
    }
  }
}
