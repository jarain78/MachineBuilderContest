#include "Config.h"

void setup() {
  
  Serial.begin(115200);

  Wire.begin(0x29);              // Inicializa la comunicación I2C como esclavo
  Wire.onReceive(receiveData);  // Configura la función de callback para recibir datos

  Serial.println("XIAO Slave Communication");
  Serial.print("I2C Adress: ");
  Serial.println(0x29);
}

void loop() {
  // Realiza operaciones adicionales según sea necesario
}

// Recibe la imagen en base64 ok
void receiveData(int numBytes) {
  int index = 0;
  while (Wire.available()) {
    char c = Wire.read();
    buffer[index] = c;
    index++;
  }

  // Reconstituir la cadena base64 a partir de los paquetes de datos recibidos
  String imageDataBase64 = String(buffer).substring(0, index);

  Serial.println(imageDataBase64);

  // Decodificar la cadena base64 para obtener la imagen original
  //std::vector<uint8_t> imageData = base64::decode(imageDataBase64);

  // Aquí puedes trabajar con los datos de la imagen recibida (por ejemplo, mostrarla en una pantalla)
}
