#include "filterCoef.h"
#include "IIRFilter.h"
#include "FIRFilter.h"
#include <SoftwareSerial.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

#define dataSize 160 * 2

// BDM101
#define EXCODE 0x55
#define SYNC 0xAA
#define GAIN_FACTOR 3

uint8_t Uart2_Buffer[256];  // Receive buffer
uint8_t Uart2_Rx = 0;       // UART2_BUFFER subscript
uint8_t Uart2_Len;          // Data length (including CRC after the third byte)
int checksum = 0;           // According to the check value calculated by PayLoad
uint8_t Uart2_Sta = 0;      // Data frame correct sign
uint8_t Uart2_check;        // Frame of check value
uint8_t sig_quality = 200;  //Signal quality

int ecgBuffer[dataSize];
int gsrBuffer[dataSize];


int bdm_x_time = 0;

uint8_t heart_rate = 0;
int y_ecg;
float v_ecg = 0.0;
int gsr = 0;


// Instantiate filters
IIRFilter notch50(b_n50, a_n50);
IIRFilter lp(b_lp, a_lp);
IIRFilter hp(b_hp, a_hp);

const int numSamples = 512;  // Número de muestras para calcular el promedio
int ecg_signal[numSamples];  // Almacena las muestras de la señal
float dcOffset = 0;

// Variables de captura
unsigned long last_capture_time = 0;
const unsigned long capture_interval = 500;  // 30 segundos
String ecg_data_block = "";


// -------------------------------------------------
// MAX30105
// -------------------------------------------------
MAX30105 particleSensor;
const byte RATE_SIZE = 4;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
int ir_data = 0;

float voltage = 0.0f;

//SoftwareSerial bdmSerial(7, 6);   // RX, TX
SoftwareSerial boron_404X(9, 8);  // RX, TX
#define bdmSerial Serial1

void setup() {
  Serial.begin(115200);
  bdmSerial.begin(57600);
  boron_404X.begin(57600);

  Serial.println("Hola");

  /*init_gsr();
  initMAX();
  config2HearthRate();
  config2PPG();*/
}

void loop() {
  unsigned long now = millis();
  read_bdm_101();
  // Cada 30 segundos, enviar el bloque por boron_404X
  if (now - last_capture_time >= capture_interval) {
    last_capture_time = now;

    if (ecg_data_block.length() > 0) {
      // Enviar bloque al Boron
      boron_404X.println(ecg_data_block);
      Serial.println("📤 Bloque enviado:");
      Serial.println(ecg_data_block);

      ecg_data_block = "";  // Limpiar bloque para la próxima captura
    }
  }

  delay(500);  // Pequeño delay para no saturar


  resetBuffer();
}


void resetBuffer() {
  memset(ecgBuffer, 0, sizeof(ecgBuffer));
  memset(gsrBuffer, 0, sizeof(gsrBuffer));
}