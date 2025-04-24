#include "Config.h"
#include "Test.h"


void setup() {

  Serial.begin(115200);
  delay(4000);
  tf.begin(ecg_model);
}

void loop() {

  for (int i = 0; i < 10; i++) {
    output_data = tf.predictClass(x_test[i]);

    Serial.print("Predicted: ");
    Serial.println(output_data);
    delay(1000);
  }
}
