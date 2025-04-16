#include <Wire.h>

#define I2C_ADDRESS 0x29      // Dirección I2C del esclavo
#define DEVICE_ID   0x42      // ID del dispositivo que se enviará al maestro
#define ADC_PIN     A0        // Pin del ADC a usar

volatile uint8_t current_register = 0;
volatile uint16_t adc_value = 0;

void setup() {
  Wire.begin(I2C_ADDRESS);           // Inicia el Arduino como esclavo I2C
  Wire.onReceive(receiveEvent);      // Configura la función para recibir comandos
  Wire.onRequest(requestEvent);      // Configura la función para enviar datos

  Serial.begin(115200);
  analogReadResolution(10);          // 10-bit ADC (0-1023)
  Serial.println("Arduino I2C listo con ID y ADC");
}

void loop() {
  // Leer constantemente el valor del ADC
  adc_value = analogRead(ADC_PIN);
  delay(10);
}

// Se llama cuando el maestro escribe (por ejemplo, para seleccionar registro)
void receiveEvent(int bytes) {
  if (bytes > 0) {
    current_register = Wire.read(); // Guarda el registro solicitado
  }
}

// Se llama cuando el maestro solicita datos
void requestEvent() {
  switch (current_register) {
    case 0x01:  // Solicitud del ID del dispositivo
      Wire.write(DEVICE_ID);
      break;

    case 0x02:  // Solicitud del valor del ADC (2 bytes, MSB primero)
      Wire.write((uint8_t)(adc_value >> 8));   // Enviar byte alto
      Wire.write((uint8_t)(adc_value & 0xFF)); // Enviar byte bajo
      break;

    default:    // Registro desconocido
      Wire.write((uint8_t)0x00);
      break;
  }
}
