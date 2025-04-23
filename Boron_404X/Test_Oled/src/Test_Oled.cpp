/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Usa Wire (I2C) del Boron
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C es la dirección I2C común
    Log.error("SSD1306 allocation failed");
    for(;;); // Halt
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,10);
  display.println("Hello Boron!");
  display.display();
}

void loop() {
  // Puedes actualizar aquí si quieres
}
