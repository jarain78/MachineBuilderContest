// === Librerías ===
//#include "filterCoef.h"  // Descomentar si se usan filtros personalizados
#include "IIRFilter.h"
#include "FIRFilter.h"
#include <SoftwareSerial.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

// === Definiciones Globales ===
#define dataSize 160 * 2          // Tamaño del buffer para ECG y GSR
#define capture_interval 30000UL  // Intervalo de captura (30 segundos)
#define sample_interval 5         // Intervalo entre muestras (ms)
#define signal_pin A0             // Pin analógico para ECG o GSR
#define CHUNK_SIZE 150             // Cantidad de datos por paquete

// === Constantes BDM101 ===
#define EXCODE 0x55
#define SYNC 0xAA
#define GAIN_FACTOR 3

// === Buffers de datos ===
int ecgBuffer[dataSize];
int gsrBuffer[dataSize];

// === Variables de Protocolo BDM101 ===
uint8_t Uart2_Buffer[256];  // Buffer de recepción
uint8_t Uart2_Rx = 0;
uint8_t Uart2_Len;
uint8_t Uart2_check;
uint8_t Uart2_Sta = 0;
int checksum = 0;
uint8_t sig_quality = 200;
int bdm_x_time = 0;
int heart_rate = 0;
// === Variables de Señal y Filtros ===
int y_ecg = 0;
float v_ecg = 0.0;
int gsr = 0;

// === Variables de Ritmo Cardíaco (MAX30105) ===
MAX30105 particleSensor;
const byte RATE_SIZE = 4;  // Tamaño del buffer de promedio de BPM
byte rates[RATE_SIZE];     // Buffer de BPM
byte rateSpot = 0;
long lastBeat = 0;  // Tiempo del último latido

float beatsPerMinute = 0.0;
int beatAvg = 0;
int ir_data = 0;

// === Señal Actual y Datos Capturados ===
String current_signal_type = "";
String data_block = "";
String ecg_data_block = "";

// === Comunicación Serial ===
//SoftwareSerial bdmSerial(7, 6);   // RX, TX
SoftwareSerial boron_404X(9, 8);  // RX, TX
#define bdmSerial Serial1 

// === Variables de Captura ===
unsigned long last_capture_time = 0;
unsigned long last_sample_time = 0;
bool capturing = false;
