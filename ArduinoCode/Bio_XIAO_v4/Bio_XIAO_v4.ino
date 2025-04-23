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

float voltage = 0.0f;

SoftwareSerial bdmSerial(7, 6);   // RX, TX
SoftwareSerial boron_404X(9, 8);  // RX, TX

String data = "0.37,0.37,0.37,0.38,0.39,0.39,0.39,0.39,0.39,0.39,0.4,0.4,0.4,0.4,0.39,0.39,0.39,0.38,0.38,0.37,0.37,0.38,0.4,0.4,0.4,0.4,0.4,0.39,0.39,0.4,0.39,0.4,0.39,0.4,0.4,0.39,0.4,0.4,0.4,0.4,0.39,0.4,0.4,0.39,0.39,0.39,0.39,0.4,0.41,0.41,0.43,0.42,0.44,0.44,0.43,0.41,0.39,0.38,0.38,0.37,0.37,0.37,0.37,0.37,0.38,0.36,0.37,0.44,0.67,0.91,0.51,0.16,0.18,0.3,0.36,0.36,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.38,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.38,0.37,0.37,0.36,0.36,0.37,0.37,0.39,0.39,0.39,0.39,0.39,0.38,0.38,0.38,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.39,0.38,0.38,0.38,0.38,0.38,0.39,0.4,0.4,0.42,0.42,0.42,0.42,0.43,0.41,0.39,0.38,0.37,0.36,0.36,0.36,0.35,0.36,0.36,0.36,0.35,0.39,0.52,0.81,0.91,0.37,0.13,0.23,0.32,0.35,0.35,0.35,0.35,0.35,0.36,0.36,0.36,0.36,0.36,0.37,0.37,0.37,0.37,0.37,0.38,0.38,0.38,0.38,0.38,0.38,0.37,0.37,0.36,0.36,0.35,0.36,0.36,0.37,0.38,0.38,0.38,0.38,0.37,0.37,0.37,0.37,0.37,0.37,0.38,0.38,0.37,0.37,0.38,0.38,0.38,0.37,0.37,0.37,0.37,0.37,0.37,0.38,0.39,0.39,0.41,0.4,0.42,0.41,0.42,0.39,0.38,0.37,0.35,0.35,0.36,0.35,0.35,0.35,0.35,0.35,0.35,0.41,0.64,0.94,0.64,0.19,0.16,0.29,0.34,0.35,0.35,0.35,0.35,0.35,0.35,0.35,0.36,0.35,0.35,0.36,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.37,0.38,0.37";
const int MAX_PAYLOAD_LENGTH = 51;
int block_id = 0;



void send_signal_in_blocks() {
  int total_blocks = (data.length() + MAX_PAYLOAD_LENGTH - 1) / MAX_PAYLOAD_LENGTH;
  for (int i = 0; i < data.length(); i += MAX_PAYLOAD_LENGTH) {
    String payload_part = data.substring(i, min(i + MAX_PAYLOAD_LENGTH, data.length()));

    String message = "<DATA>:id=" + String(block_id) + ";total=" + String(total_blocks) + ";payload=" + payload_part + ";";

    boron_404X.println(message);  // o Particle.publish("my_event", message, PRIVATE);

    block_id++;
    delay(500);  // Pequeño delay para no saturar
  }
  block_id = 0;
}


void setup() {
  Serial.begin(115200);
  bdmSerial.begin(57600);
  boron_404X.begin(57600);


  /*init_gsr();
  initMAX();
  config2HearthRate();
  config2PPG();*/
}

void loop() {




  send_signal_in_blocks();

  /*boron_404X.print();
  boron_404X.print(" ");
  boron_404X.print(ir_data * 0.5);
  boron_404X.print(" ");
  boron_404X.println(gsr);*/


  //read_bdm_101();
  //resetBuffer();
}


void resetBuffer() {
  memset(ecgBuffer, 0, sizeof(ecgBuffer));
  memset(gsrBuffer, 0, sizeof(gsrBuffer));
}