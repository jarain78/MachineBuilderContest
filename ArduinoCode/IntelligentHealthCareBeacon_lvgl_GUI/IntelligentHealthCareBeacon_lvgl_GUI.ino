#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"
#include <WiFi.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

// Globales para LVGL y WiFi
lv_obj_t *serial_list;
lv_obj_t *chart;
lv_chart_series_t *series;
lv_obj_t *title;
lv_obj_t *ssid_ta;
String selected_ssid = "";
const int ecg_resolution = 256;
lv_obj_t *temp_label;
lv_obj_t *hr_label;


/*void wifi_connect_event_cb(lv_event_t *e) {
  const char *password = lv_textarea_get_text(ssid_ta);
  WiFi.begin(selected_ssid.c_str(), password);
  Serial.println("Connecting to " + selected_ssid + " with password: " + String(password));
}

void wifi_ssid_select_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  const char *label = lv_label_get_text(lv_obj_get_child(btn, 0));
  selected_ssid = String(label);
  Serial.println("Selected SSID: " + selected_ssid);
}*/



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
  lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Bio-Signals");
  lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Info");

  // ==== TAB 1: ECG Plot ====
  lv_obj_t *frame = lv_obj_create(tab1);
  lv_obj_set_size(frame, 400, 280);
  lv_obj_align(frame, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_set_style_border_width(frame, 2, 0);
  lv_obj_set_style_pad_all(frame, 10, 0);

  lv_obj_t *title = lv_label_create(frame);
  lv_label_set_text(title, "Signal");
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 10, -10);

  // Chart for ECG
  chart = lv_chart_create(frame);
  lv_obj_set_size(chart, 300, 180);
  lv_obj_align(chart, LV_ALIGN_TOP_LEFT, 60, 10);

  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

  // Escala el eje Y entre 0 y 1.0 como 0 a 1000
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
  lv_chart_set_point_count(chart, ecg_resolution);
  lv_chart_set_div_line_count(chart, 5, 5);
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 5, 1, true, 40);
  lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 1, true, 40);

  // Etiquetas
  lv_obj_t *label_x = lv_label_create(frame);
  lv_label_set_text(label_x, "Time");
  lv_obj_align_to(label_x, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 25);

  lv_obj_t *label_y = lv_label_create(frame);
  lv_label_set_text(label_y, "V\no\nl\nt\na\ng\ne");
  lv_label_set_long_mode(label_y, LV_LABEL_LONG_WRAP);
  lv_obj_align_to(label_y, chart, LV_ALIGN_OUT_LEFT_MID, -40, 0);

  // Serie con valores de 0.0 a 1.0 representados como 0 a 1000
  series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
  for (int i = 0; i < ecg_resolution; i++) {
    series->y_points[i] = 500;  // representa 0.5
  }
  lv_chart_refresh(chart);


  // Contenedor para widgets de temperatura y HR
  lv_obj_t *info_container = lv_obj_create(tab1);
  lv_obj_set_size(info_container, LV_PCT(40), 70);
  lv_obj_align(info_container, LV_ALIGN_TOP_RIGHT, -15, -10);
  lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_row(info_container, 0, 0);
  lv_obj_set_style_pad_column(info_container, 0, 0);
  lv_obj_set_scroll_dir(info_container, LV_DIR_NONE);

  // --- Widget de Temperatura ---
  lv_obj_t *temp_box = lv_obj_create(info_container);
  lv_obj_set_size(temp_box, 140, 40);
  lv_obj_set_style_pad_all(temp_box, 2, 0);
  lv_obj_set_flex_flow(temp_box, LV_FLEX_FLOW_ROW);

  lv_obj_t *temp_icon = lv_label_create(temp_box);
  lv_label_set_text(temp_icon, LV_SYMBOL_CHARGE);  // ícono de calor/termómetro
  lv_obj_align(temp_icon, LV_ALIGN_LEFT_MID, 0, 0);

  temp_label = lv_label_create(temp_box);
  lv_label_set_text(temp_label, "-:- °C");
  lv_obj_align(temp_label, LV_ALIGN_RIGHT_MID, 0, 0);

  // --- Widget de HR (frecuencia cardíaca) ---
  lv_obj_t *hr_box = lv_obj_create(info_container);
  lv_obj_set_size(hr_box, 140, 40);
  lv_obj_set_style_pad_all(hr_box, 2, 0);
  lv_obj_set_flex_flow(hr_box, LV_FLEX_FLOW_ROW);

  lv_obj_t *heart_icon = lv_label_create(hr_box);
  lv_label_set_text(heart_icon, LV_SYMBOL_AUDIO);  // se ve como un corazón ♡ en muchas fuentes
  lv_obj_align(heart_icon, LV_ALIGN_LEFT_MID, 0, 0);

  hr_label = lv_label_create(hr_box);
  lv_label_set_text(hr_label, "-:- bpm");
  lv_obj_align(hr_label, LV_ALIGN_RIGHT_MID, 0, 0);

  // Contenedor para los botones de selección de señal
  lv_obj_t *signal_btn_container = lv_obj_create(tab1);
  lv_obj_set_size(signal_btn_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align(signal_btn_container, LV_ALIGN_BOTTOM_LEFT, 10, -10);  // Debajo del gráfico5
  lv_obj_set_flex_flow(signal_btn_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_row(signal_btn_container, 0, 40);
  lv_obj_set_style_pad_column(signal_btn_container, 0, 40);

  // Función para evento de los botones
  auto signal_button_event_cb = [](lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    char *label = lv_label_get_text(lv_obj_get_child(btn, 0));

    String signal = String(label);

    if (signal.equals("ECG")) {
      Serial.println("<CMD>:START:ECG;");
    } else if (signal.equals("FCG")) {
      Serial.println("<CMD>:START:FCG;");
    }else if(signal.equals("PPG")) {
      Serial.println("<CMD>:START:PPG;");
    }else if(signal.equals("Analize Signal")) {
      Serial.println("<CMD>:START:SET_ML;");
    }



    // Aquí podrías cambiar el tipo de señal mostrada si quieres
  };

  // Función para crear botones
  auto add_signal_button = [&](const char *label) {
    lv_obj_t *btn = lv_btn_create(signal_btn_container);
    lv_obj_set_size(btn, 80, 40);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, signal_button_event_cb, LV_EVENT_CLICKED, NULL);
  };

// Función para crear un boton para el analisis de las señal con ML
  auto add_ml_button = [&](const char *label) {
    lv_obj_t *btn = lv_btn_create(signal_btn_container);
    lv_obj_set_size(btn, 150, 40);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, signal_button_event_cb, LV_EVENT_CLICKED, NULL);
  };


  // Crear los tres botones
  add_signal_button("ECG");
  add_signal_button("FCG");
  add_signal_button("PPG");
  
  add_ml_button("Analize Signal");


  // ==== PESTAÑA 2: Lista Serial ====
  serial_list = lv_list_create(tab2);
  lv_obj_set_size(serial_list, 300, 200);
  lv_obj_center(serial_list);

  lv_list_add_text(serial_list, "Datos Serial:");
  lv_list_add_btn(serial_list, LV_SYMBOL_RIGHT, "ECG Inicializado");


  // ==== PESTAÑA 3: Wifi ====
  /*lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "WiFi");
  lv_obj_t *wifi_list = lv_list_create(tab3);
  lv_obj_set_size(wifi_list, 280, 150);
  lv_obj_align(wifi_list, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_list_add_text(wifi_list, "Available Networks:");

  ssid_ta = lv_textarea_create(tab3);
  lv_textarea_set_placeholder_text(ssid_ta, "Password");
  lv_textarea_set_password_mode(ssid_ta, true);
  lv_obj_set_width(ssid_ta, 200);
  lv_obj_align_to(ssid_ta, wifi_list, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

  lv_obj_t *connect_btn = lv_btn_create(tab3);
  lv_obj_align_to(connect_btn, ssid_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
  lv_obj_t *btn_label = lv_label_create(connect_btn);
  lv_label_set_text(btn_label, "Connect");
  lv_obj_center(btn_label);

  static String selected_ssid = "";

  lv_obj_add_event_cb(
    connect_btn, [](lv_event_t *e) {
      const char *password = lv_textarea_get_text(ssid_ta);
      WiFi.begin(selected_ssid.c_str(), password);
      Serial.println("Connecting to " + selected_ssid + "...");
    },
    LV_EVENT_CLICKED, NULL);
  Serial.println("Scanning WiFi...");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    lv_obj_t *btn = lv_list_add_btn(wifi_list, LV_SYMBOL_WIFI, ssid.c_str());

    lv_obj_add_event_cb(connect_btn, wifi_connect_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn, wifi_ssid_select_cb, LV_EVENT_CLICKED, NULL);
  }*/

  // ==== PESTAÑA 4: Robot ====
  lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Robot");

  // Contenedor con flexbox vertical
  // Contenedor con flexbox vertical que se ajusta al contenido
  lv_obj_t *btn_container = lv_obj_create(tab4);
  lv_obj_set_size(btn_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);  // Se adapta al contenido
  lv_obj_align(btn_container, LV_ALIGN_TOP_LEFT, 10, 10);            // Posición en pantalla
  lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);          // Distribución vertical
  lv_obj_set_style_pad_row(btn_container, 10, 0);                    // Espaciado entre botones



  // Función para crear botones con callback
  auto add_robot_button = [&](const char *label, const char *command) {
    lv_obj_t *btn = lv_btn_create(btn_container);
    lv_obj_set_size(btn, 200, 40);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label);
    lv_obj_center(lbl);

    lv_obj_add_event_cb(btn, robot_action_event_cb, LV_EVENT_CLICKED, NULL);
  };

  // Crear 4 botones
  add_robot_button("Action # 1", "ACCION_1");
  add_robot_button("Action # 2", "ACCION_2");
  add_robot_button("Action # 3", "ACCION_3");
  add_robot_button("Action # 4", "ACCION_4");


  lvgl_port_unlock();
}

void loop() {

  read_serial_and_process();

  delay(5);  // Pequeño delay para estabilidad

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
