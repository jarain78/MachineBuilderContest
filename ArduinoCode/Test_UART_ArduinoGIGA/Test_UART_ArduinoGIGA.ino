void setup() {
  Serial.begin(115200);    // Inicializa la comunicación USB
  Serial2.begin(115200);   // Inicializa UART1 (TX2/RX2)
  delay(1000);
  Serial.println("🟢 UART1 listo para enviar datos");
}

void loop() {
  // Enviar un mensaje por UART1
  Serial2.println("📤 Hola desde UART1");

  // Verificar si hay datos recibidos en UART1
  if (Serial2.available()) {
    String mensaje = Serial2.readStringUntil('\n');
    Serial.print("📩 Recibido por UART1: ");
    Serial.println(mensaje);
  }

  delay(1000); // Espera 1 segundo
}
