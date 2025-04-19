void robot_action_event_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  const char *label = lv_label_get_text(lv_obj_get_child(btn, 0));
  String action = String(label);

  if (action.equals("Action # 1")) {
    Serial.println("<CMD>:GOTO=1;");
  } else if (action.equals("Action # 2")) {
    Serial.println("<CMD>:GOTO=2;");
  } else if (action.equals("Action # 3")) {
    Serial.println("<CMD>:GOTO=3;");
  } else if (action.equals("Action # 4")) {
    Serial.println("<CMD>:GOTO=4;");
  }
  //action.replace(" ", "_");  // convierte "Acción 1" → "Acción_1"
  //Serial.println("ACCION_" + action.substring(action.length() - 1));
}
