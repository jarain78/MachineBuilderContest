
void Tab_4() {
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
}