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

void Tab_3() {

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
}