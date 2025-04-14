#include <Wire.h>

#define NRF9151_ADDR 0x68
#define WHOAMI_REG   0x75

void setup() {
  Serial.begin(115200);
  Wire.begin();              // Inicia como maestro
  Wire.setClock(100000);     // Establece velocidad a 100 kHz
  delay(100);

  Serial.println("Reading WHOAMI register from nRF9151...");

  Wire.beginTransmission(NRF9151_ADDR);
  Wire.write(WHOAMI_REG);
  Wire.endTransmission(false);

  Wire.requestFrom(NRF9151_ADDR, 1);

  if (Wire.available()) {
    uint8_t whoami = Wire.read();
    Serial.print("WHOAMI: 0x");
    Serial.println(whoami, HEX);
  } else {
    Serial.println("No data received.");
  }
}

void loop() {
  // no-op
}
