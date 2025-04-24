#include <Arduino_USBHostMbed5.h>
#include <FATFileSystem.h>
#include <AdvancedADC.h>

USBHostMSD msd;
mbed::FATFileSystem usb("usb");

AdvancedADC adc1(A0);

File wavFile;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!adc1.begin(AN_RESOLUTION_12, 16000, 64, 32)) {
    Serial.println("❌ Error al iniciar ADC");
    while (1);
  }

  // Esperar a que se conecte la memoria USB
  while (!msd.connect()) {
    Serial.println("⌛ Esperando memoria USB...");
    delay(1000);
  }

  // Montar sistema de archivos
  int err = usb.mount(&msd);
  if (err != 0) {
    Serial.println("❌ No se pudo montar la USB");
    while (1);
  }

  // Crear archivo WAV
  wavFile = fopen("/usb/audio.wav", "wb");
  if (!wavFile) {
    Serial.println("❌ No se pudo abrir archivo WAV");
    while (1);
  }

  // Escribir encabezado WAV (PCM mono 16bit 16kHz)
  writeWavHeader(wavFile, 16000, 1, 16);
}

void loop() {
  if (adc1.available()) {
    SampleBuffer buf = adc1.read();
    fwrite(buf.data(), sizeof(int16_t), buf.size(), wavFile);  // Escribir datos de audio
    buf.release();
  }
}

void writeWavHeader(FILE* file, int sampleRate, int channels, int bitsPerSample) {
  uint32_t dataSize = 0xFFFFFFFF; // Tamaño desconocido, usar software para fijarlo después si se desea
  uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
  uint16_t blockAlign = channels * bitsPerSample / 8;

  fwrite("RIFF", 1, 4, file);
  uint32_t chunkSize = dataSize + 36;
  fwrite(&chunkSize, 4, 1, file);
  fwrite("WAVE", 1, 4, file);
  fwrite("fmt ", 1, 4, file);

  uint32_t subChunk1Size = 16;
  uint16_t audioFormat = 1; // PCM
  fwrite(&subChunk1Size, 4, 1, file);
  fwrite(&audioFormat, 2, 1, file);
  fwrite(&channels, 2, 1, file);
  fwrite(&sampleRate, 4, 1, file);
  fwrite(&byteRate, 4, 1, file);
  fwrite(&blockAlign, 2, 1, file);
  fwrite(&bitsPerSample, 2, 1, file);

  fwrite("data", 1, 4, file);
  fwrite(&dataSize, 4, 1, file);  // Tamaño real se puede corregir al final
}
