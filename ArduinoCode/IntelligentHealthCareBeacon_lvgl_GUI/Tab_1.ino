
void show_waiting_icon(lv_obj_t *parent) {
  if (waiting_icon != NULL) return;  // Ya está creado

  waiting_icon = lv_spinner_create(parent, 1000, 60);  // (periodo, tamaño)
  lv_obj_set_size(waiting_icon, 50, 50);
  lv_obj_align(waiting_icon, LV_ALIGN_CENTER, 0, 0);
}



void hide_waiting_icon() {
  if (waiting_icon != NULL) {
    lv_obj_del(waiting_icon);
    waiting_icon = NULL;
  }
}



void Tab_1() {

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
    } else if (signal.equals("PPG")) {
      Serial.println("<CMD>:START:PPG;");
    } else if (signal.equals("Analize Signal")) {
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
    
    /*lv_obj_add_event_cb(
      btn, [](lv_event_t *e) {
        // Envía el comando correspondiente por Serial o lo que sea...
        Serial.println("Starting ECG Capture");

        // Mostrar ícono de espera
        show_waiting_icon(tab1);
      },
      LV_EVENT_CLICKED, NULL);*/
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
}