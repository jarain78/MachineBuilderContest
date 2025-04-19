#include <vector>  // Necesario para usar std::vector

extern lv_obj_t* chart;
extern lv_chart_series_t* series;
extern lv_obj_t* temp_label;
extern lv_obj_t* hr_label;

String serialBuffer = "";

// Función para actualizar el chart automáticamente
void update_chart_auto_scale(lv_obj_t* chart, lv_chart_series_t* series, const std::vector<float>& data) {
  if (data.empty()) return;

  int min_val = 100000;
  int max_val = -100000;
  int n_points = data.size();

  lv_chart_set_point_count(chart, n_points);

  for (int i = 0; i < n_points; i++) {
    int scaled_val = data[i] * 1000.0f;  // Escalar
    series->y_points[i] = scaled_val;

    if (scaled_val < min_val) min_val = scaled_val;
    if (scaled_val > max_val) max_val = scaled_val;
  }

  // Ajuste de seguridad
  if (min_val == max_val) {
    min_val -= 10;
    max_val += 10;
  }

  lvgl_port_lock(-1);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, min_val, max_val);
  lv_chart_set_div_line_count(chart, 5, 5);
  lv_chart_refresh(chart);
  lvgl_port_unlock();
}

// Función para parsear un mensaje completo
void parseMessage(const String& msg) {
  if (!msg.startsWith("<DATA>:")) return;

  String data = msg.substring(7);  // Elimina "<DATA>:"
  bool valid_data = false;

  // Función lambda para extraer campos
  auto extractField = [&](const String& key) -> String {
    int idx = data.indexOf(key + "=");
    if (idx == -1) return "";

    int start = idx + key.length() + 1;
    int end = data.indexOf(';', start);
    if (end == -1) end = data.length();
    return data.substring(start, end);
  };

  // Función para procesar datos de señales
  auto processSignalArray = [&](const String& values_str) {
    std::vector<float> parsed_data;
    String values = values_str;

    while (values.length() > 0) {
      int comma = values.indexOf(',');
      String value_str;
      if (comma == -1) {
        value_str = values;
        values = "";
      } else {
        value_str = values.substring(0, comma);
        values = values.substring(comma + 1);
      }

      float float_val = value_str.toFloat();
      parsed_data.push_back(float_val);
    }

    update_chart_auto_scale(chart, series, parsed_data);
  };

  // ---- Procesar tipos de señales ----
  String ecg = extractField("ECG");
  String ppg = extractField("PPG");
  String fcg = extractField("FCG");
  String joints = extractField("JOINTS");
  String ml_result = extractField("ML_RESULT");
  String temp = extractField("TEMP");
  String hr = extractField("HR");

  if (ecg.length() > 0) {
    processSignalArray(ecg);
    valid_data = true;
  }

  if (ppg.length() > 0) {
    processSignalArray(ppg);
    valid_data = true;
  }

  if (fcg.length() > 0) {
    processSignalArray(fcg);
    valid_data = true;
  }

  if (joints.length() > 0) {
    Serial.println("Received Joints: " + joints);
    valid_data = true;
  }

  if (ml_result.length() > 0) {
    Serial.println("ML Result: " + ml_result);
    valid_data = true;
  }

  if (temp.length() > 0) {
    float temperature = temp.toFloat();
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(temp_label, "%.1 °C", temperature);
    lvgl_port_unlock();
    valid_data = true;
  }

  if (hr.length() > 0) {
    int heart_rate = hr.toInt();
    lvgl_port_lock(-1);
    lv_label_set_text_fmt(hr_label, "%d bpm", heart_rate);
    lvgl_port_unlock();
    valid_data = true;
  }

  // ✅ Enviar ACK si hubo datos válidos
  if (valid_data) {
    Serial.println("<ACK>:DATA_RECEIVED;");
    //hide_waiting_icon();  
  }
}

// Leer del serial y procesar
void read_serial_and_process() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseMessage(serialBuffer);
      serialBuffer = "";
      Serial.flush();
    } else {
      serialBuffer += c;
    }
  }
}
