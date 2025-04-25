
void parse_payload(void)  // Treatment of the content of the payload
{
  uint8_t bytesParsed = 0;  // number of bytes processed
  uint8_t code;
  uint8_t length;  // Current Number of bytes of Value included in DataRow
  uint8_t extendedCodeLevel;
  short int rawValue = 0;  // ECG data


  while (bytesParsed < Uart2_Len) {

    extendedCodeLevel = 0;
    while (Uart2_Buffer[3 + bytesParsed] == EXCODE) {
      extendedCodeLevel++;
      bytesParsed++;
    }
    code = Uart2_Buffer[3 + bytesParsed];
    bytesParsed++;
    if (code >= 0x80) {
      length = Uart2_Buffer[3 + bytesParsed];
      bytesParsed++;
    } else
      length = 1;
    //now we get ExCodeLevel,code and the length of DataValue
    //in fact,Extended Code Level is always 0,so we can ignore it
    switch (code) {
      case 0x80:  //two bytes signed data of raw wave value,big-endian

        if (sig_quality > 0) {
          rawValue = Uart2_Buffer[3 + bytesParsed];
          rawValue <<= 8;
          rawValue |= Uart2_Buffer[4 + bytesParsed];
          y_ecg = rawValue;  // * GAIN_FACTOR;
          y_ecg = map(y_ecg, -32768, 32767, 0, 255);
          ecgBuffer[bdm_x_time] = y_ecg;

          read_gsr();
          readIR();
          readRed();

          Serial.print(y_ecg);
          Serial.print(" ");
          Serial.print(ir_data * 0.2);
          Serial.print(" ");
          Serial.print(red_data * 0.2);
          Serial.print(" ");
          Serial.println(gsr);
          delay(10);


          //Serial.println(notch50.filter(lp.filter(y_ecg)));
          delay(18);
          bdm_x_time = bdm_x_time + 1;

          if (bdm_x_time == dataSize) {
            bdm_x_time = 0;
            resetBuffer();
          }
          //v_ecg = map(y_ecg, -32768, 32767, 0.0, 3.3);
        } else {
          //bdmSerialUSB.println("Poor Quality Signal");
          //bad_signal();
          delay(3000);
        }

        break;
      case 0x02:
        //one byte data of signal quality,0 stands for poor quality while 200 stands for good
        sig_quality = Uart2_Buffer[3 + bytesParsed];

        break;
      case 0x03:  //one byte data of heart rate value

        heart_rate = Uart2_Buffer[3 + bytesParsed];
        //bdmSerialUSB.print("Heart Rate: ");
        //bdmSerialUSB.println(heart_rate);

        break;
    }
    bytesParsed += length;
  }
}


void read_bdm_101() {

  if (bdmSerial.available()) {
    while (bdmSerial.available() > 0) {

      Uart2_Buffer[Uart2_Rx] = bdmSerial.read();  //read bdmSerial
      Uart2_Rx++;

      if (Uart2_Rx < 3)  // Judgment whether the frame header is completed
      {
        if (Uart2_Buffer[Uart2_Rx - 1] != SYNC)  // abnormal
        {
          Uart2_Rx = 0;   // Laid 0
          Uart2_Sta = 0;  // Sign into 0
        }

      } else if (Uart2_Rx == 3)  // Get the PAYLOAD length
        Uart2_Len = Uart2_Buffer[Uart2_Rx - 1];
      else if (Uart2_Rx < 4 + Uart2_Len)  // Receive PayLoad
      {
        checksum += Uart2_Buffer[Uart2_Rx - 1];  // calculate the check value
        //checksum &= 0xFF;
      } else  // Receive a check bit
      {
        Uart2_check = Uart2_Buffer[Uart2_Rx - 1];
        checksum &= 0xFF;
        checksum = ~checksum & 0xFF;
        if (checksum != Uart2_check)  // Verify error, discard the packet
        {
          Uart2_Rx = 0;   // Laid 0
          Uart2_Sta = 0;  // Sign into 0
          checksum = 0;
        } else
          Uart2_Sta = 1;  // Receive completion
      }
      if (Uart2_Sta)  // Detect the logo, explain successful reception
      {
        Uart2_Rx = 0;   // Laid 0
        Uart2_Sta = 0;  // Sign into 0
        checksum = 0;
        parse_payload();  // Call the data resolution function
      }
    }
  }
}
