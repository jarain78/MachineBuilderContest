// Bio-XIAO nRF52840
// v4
// Jueves Diciembre 19 2024

#include "Arduino.h"
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
int red_data = 0;

float voltage = 0.0f;


void read_gsr();


SoftwareSerial bdmSerial(7, 6);  // RX, TX


void setup() {
  Serial.begin(115200);
  bdmSerial.begin(57600);
  init_gsr();
  initMAX();
  config2HearthRate();
  config2PPG();
}

void loop() {

  read_bdm_101();
  resetBuffer();
}


void resetBuffer() {
  memset(ecgBuffer, 0, sizeof(ecgBuffer));
  memset(gsrBuffer, 0, sizeof(gsrBuffer));
}