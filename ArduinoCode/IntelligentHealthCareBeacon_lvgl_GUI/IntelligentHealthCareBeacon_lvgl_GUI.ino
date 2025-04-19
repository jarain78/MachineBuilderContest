#include "Config.h"

void setup() {
  Serial.begin(115200);

  Serial.println("Initializing board");
  Board *board = new Board();
  board->init();

#if LVGL_PORT_AVOID_TEARING_MODE
  auto lcd = board->getLCD();
  lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
  auto lcd_bus = lcd->getBus();
  if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
  }
#endif
#endif

  assert(board->begin());

  Serial.println("Initializing LVGL");
  lvgl_port_init(board->getLCD(), board->getTouch());

  Serial.println("Creating UI");
  lvgl_port_lock(-1);

  // Crear pestañas
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
  tab1 = lv_tabview_add_tab(tabview, "Bio-Signals");
  tab2 = lv_tabview_add_tab(tabview, "Info");

  //=== Tab 1 ====
  Tab_1();
  Tab_2();
  Tab_3();
  Tab_4();
  lvgl_port_unlock();

  // Initialize SPI
  SPI.setHwCs(false);
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_SS);

}

void loop() {

  read_serial_and_process();

  //delay(5);  // Pequeño delay para estabilidad

  // Simular ECG: usar Serial, ADC o datos grabados
  /*static uint32_t t = 0;
  static uint16_t last_value = 1650;

  // Simulación: señal tipo ECG (ruido + latido)
  uint16_t ecg_value = 1650 + 400 * sin(2 * PI * t / 50.0);  // onda senoidal como simulacro
  t++;

  // Si tienes ADC, reemplaza aquí:
  // uint16_t ecg_value = analogRead(ECG_PIN);

  // Actualizar gráfico
  lvgl_port_lock(-1);
  lv_chart_set_next_value(chart, series, ecg_value);
  lvgl_port_unlock();

  // Leer Serial y mostrar en pestaña Info
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    lvgl_port_lock(-1);
    lv_list_add_btn(serial_list, LV_SYMBOL_FILE, data.c_str());
    lvgl_port_unlock();
  }


  // Simulación o lectura real
  float temperatura = 36.5 + 0.5 * sin(2 * PI * millis() / 3000.0);  // simula oscilación
  int pulso = 70 + 10 * sin(2 * PI * millis() / 1500.0);             // simula HR

  // Actualización en LVGL
  lvgl_port_lock(-1);
  lv_label_set_text_fmt(temp_label, "%.1f °C", temperatura);
  lv_label_set_text_fmt(hr_label, "%d bpm", pulso);
  lvgl_port_unlock();


  delay(20);  // ajusta este delay para el "ritmo cardíaco"
  */
}
