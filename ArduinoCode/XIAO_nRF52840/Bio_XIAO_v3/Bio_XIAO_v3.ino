#include "IIRFilter.h"
#include "FIRFilter.h"
#include <SoftwareSerial.h>
#include "Ticker.h"

#define ECG_SAMPLE_RATE 512  // Hz


void read_bmd101();

Ticker timer1(read_bmd101, 2, 0, MILLIS);


// 0.5 Hz Butterworth High-pass (2nd order)
const double a_hp[] = { 1, -1.99132254835917, 0.991360035149071 };
const double b_hp[] = { 0.995670645877060, -1.99134129175412, 0.995670645877060 };

// 50 Hz Butterworth Low-pass (3rd order)
const double a_lp[] = { 1, -1.78847140715168, 1.21164264222362, -0.286869532448224 };
const double b_lp[] = { 0.0170377128279652, 0.0511131384838956, 0.0511131384838956, 0.0170377128279652 };

// Notch filter at 60 Hz (1st order)
const double a_n60[] = { 1, -4.37428878700082, 9.28081903459846, -11.5658089363129, 8.98198159089241, -4.09711831321292, 0.906481521898628 };
const double b_n60[] = { 0.952093231830301, -4.23400251180179, 9.13254784186472, -11.5692110129236, 9.13254784186472, -4.23400251180179, 0.952093231830301 };

// Instantiate filters
IIRFilter notch60(b_n60, a_n60);
IIRFilter lp(b_lp, a_lp);
IIRFilter hp(b_hp, a_hp);
SoftwareSerial bdmSerial(7, 6);  // RX, TX

// Buffer and Data Variables
byte myReadBuffer[19] = {};
int16_t parsedADCData = 0;
byte dataType = 0;
byte HR = 0;
bool isDataValid = false;
bool leadOff = true;

void setup() {
  pinMode(18, OUTPUT);
  Serial.begin(115200);
  bdmSerial.begin(57600);  // ESP32 TX2/RX2 = G17/G16
  timer1.start();
}

void loop() {
  timer1.update();
}

void read_bmd101() {
  if (bdmSerial.available() > 0) {
    // Read header and determine packet type
    bdmSerial.readBytes(myReadBuffer, 3);

    if ((myReadBuffer[0] == 0xAA) && (myReadBuffer[1] == 0xAA)) {  //find data header
      byte pLength = myReadBuffer[2];
      if (pLength == 0x04) dataType = 0;  //dataType is adc val.
      if (pLength == 0x12) dataType = 1;  //dataType is quality & HR.
      resetBuffer();
      (dataType == 0) ? bdmSerial.readBytes(myReadBuffer, 5) : bdmSerial.readBytes(myReadBuffer, 19);
      int sum = 0;
      for (byte i = 0; i < 18; i++) sum += myReadBuffer[i];
      byte checksum_calc = ~(sum & 0xff);  // inverse of lowest 8 bit of data_sum.
      (checksum_calc == myReadBuffer[18]) ? isDataValid = true : isDataValid = false;

      /*Serial.print(dataType);
      Serial.print(' ');
      Serial.print(pLength);
      Serial.print(' ');
      Serial.print(isDataValid);
      Serial.print(' ');
      for (int i = 0; i < 16; i++) {
        Serial.write(myReadBuffer[i]);
        Serial.write(' ');
      }
      Serial.println();*/
    }
  }
  if ((isDataValid) && (dataType == 0)) {
    // data ECG ADC Val
    parsedADCData = (myReadBuffer[2] << 8) + myReadBuffer[3];
    if (parsedADCData >= 32768) parsedADCData -= 65536;
    Serial.println(parsedADCData);
    //Serial.println(notch60.filter(lp.filter(parsedADCData)));
  }
  resetBuffer();
  //delay(500);
}

void resetBuffer() {
  memset(myReadBuffer, 0, sizeof(myReadBuffer));
  isDataValid = false;
}