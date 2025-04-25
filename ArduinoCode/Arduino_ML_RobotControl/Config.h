#include <SoftwareSerial.h>

// Configura el puerto para el servo LX16A
#define LX16A_RX 8
#define LX16A_TX 9
SoftwareSerial lx16aSerial(LX16A_RX, LX16A_TX);

String serialBuffer = "";
float ml_data[50]; // Buffer para datos de ML
String ml_label = "";
float ml_prob = 0.0;