
// ===== Callback to show keyboard when a text field is focused =====
static void textarea_event_cb(lv_event_t *e) {
  lv_obj_t *ta = lv_event_get_target(e);
  if (global_keyboard) {
    lv_keyboard_set_textarea(global_keyboard, ta);
    lv_obj_clear_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(global_keyboard);
  }
}

// ===== Helper to create a labeled text field =====
lv_obj_t *create_form_field(lv_obj_t *parent, const char *label_text) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, label_text);

  lv_obj_t *ta = lv_textarea_create(parent);
  lv_textarea_set_one_line(ta, true);
  lv_obj_set_width(ta, LV_PCT(100));

  // Show keyboard when the text area is focused
  lv_obj_add_event_cb(ta, textarea_event_cb, LV_EVENT_FOCUSED, NULL);

  return ta;
}

// ===== Create the form layout (form fields + buttons) =====
void create_formulario(lv_obj_t *parent) {
  // Main container: horizontal flex layout (form + buttons)
  lv_obj_t *main_container = lv_obj_create(parent);
  lv_obj_set_size(main_container, 460, 250);
  lv_obj_align(main_container, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(main_container, 10, 0);

  // Form container (left side)
  form_container = lv_obj_create(main_container);
  lv_obj_set_size(form_container, LV_PCT(60), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(form_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scroll_dir(form_container, LV_DIR_VER);
  lv_obj_set_style_pad_all(form_container, 5, 0);

  // Buttons container (right side)
  btn_container = lv_obj_create(main_container);
  lv_obj_set_size(btn_container, LV_PCT(40), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(btn_container, 5, 0);

  // Create hidden keyboard initially
  global_keyboard = lv_keyboard_create(parent);
  lv_obj_add_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
}

void save_user_data() {
  String userData = "";
  userData += "Name: " + String(lv_textarea_get_text(name_ta)) + "\n";
  userData += "Age: " + String(lv_textarea_get_text(age_ta)) + "\n";
  userData += "Weight: " + String(lv_textarea_get_text(weight_ta)) + "\n";
  userData += "Blood Sugar: " + String(lv_textarea_get_text(sugar_ta)) + "\n";
  userData += "Blood Pressure: " + String(lv_textarea_get_text(bp_ta)) + "\n";
  userData += "Sex: " + String(lv_textarea_get_text(sex_ta)) + "\n";

  // Guardarlo en la SD usando tu función
  writeFile(SD, "/user_data.txt", userData.c_str());
}


// ===== Fill Tab 2: Create the form fields and buttons =====
void Tab_2() {
  // ✅ First create the form containers
  create_formulario(tab2);

  // ✅ Then create the form fields inside form_container
  name_ta = create_form_field(form_container, "Name:");
  age_ta = create_form_field(form_container, "Age:");
  weight_ta = create_form_field(form_container, "Weight (kg):");
  sugar_ta = create_form_field(form_container, "Blood Sugar:");
  bp_ta = create_form_field(form_container, "Blood Pressure:");
  sex_ta = create_form_field(form_container, "Sex:");

  // === Button to hide keyboard ===
  lv_obj_t *close_btn = lv_btn_create(btn_container);
  lv_obj_set_size(close_btn, 120, 40);
  lv_obj_t *close_lbl = lv_label_create(close_btn);
  lv_label_set_text(close_lbl, "Hide Keyboard");
  lv_obj_center(close_lbl);

  lv_obj_add_event_cb(
    close_btn, [](lv_event_t *e) {
      if (global_keyboard) {
        lv_keyboard_set_textarea(global_keyboard, NULL);
        lv_obj_add_flag(global_keyboard, LV_OBJ_FLAG_HIDDEN);
      }
    },
    LV_EVENT_CLICKED, NULL);

  // === Button to save form data to SD card ===
  lv_obj_t *save_btn = lv_btn_create(btn_container);
  lv_obj_set_size(save_btn, 120, 40);
  lv_obj_t *save_lbl = lv_label_create(save_btn);
  lv_label_set_text(save_lbl, "Save Data");
  lv_obj_center(save_lbl);

  lv_obj_add_event_cb(
    save_btn, [](lv_event_t *e) {
      Serial.println("Saving data to SD...");

      if (!SD.begin(SD_SS)) {
        Serial.println("Card Mount Failed");  // SD card mounting failed
        return;
      }
      uint8_t cardType = SD.cardType();

      if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");  // No SD card connected
        return;
      }

      //File file = SD.open("/user_data.txt", FILE_WRITE);
      save_user_data();
    },
    LV_EVENT_CLICKED, NULL);
}
