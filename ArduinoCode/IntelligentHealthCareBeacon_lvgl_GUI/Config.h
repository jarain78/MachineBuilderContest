#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"
//#include <WiFi.h>  // Uncomment if Wi-Fi functionality is needed
#include <SD.h>
#include <SPI.h>
#include "waveshare_sd_card.h"


using namespace esp_panel::drivers;
using namespace esp_panel::board;

// ====================
// Global Objects for LVGL UI
// ====================

// General UI components
lv_obj_t *tabview = NULL;
lv_obj_t *tab1 = NULL;  // Tab for Bio-Signals
lv_obj_t *tab2 = NULL;  // Tab for User Form

// Tab 1: Bio-Signal components
lv_obj_t *serial_list = NULL;   // Serial messages list
lv_obj_t *chart = NULL;         // Chart for ECG/PPG/FCG signals
lv_chart_series_t *series = NULL;
const int ecg_resolution = 256; // Number of points in the signal chart
lv_obj_t *temp_label = NULL;    // Temperature label
lv_obj_t *hr_label = NULL;      // Heart rate label
lv_obj_t *waiting_icon = NULL;


// Tab 2: User Form components
lv_obj_t *global_keyboard = NULL; // Global keyboard (hidden by default)
lv_obj_t *name_ta = NULL;          // TextArea for Name
lv_obj_t *age_ta = NULL;           // TextArea for Age
lv_obj_t *weight_ta = NULL;        // TextArea for Weight
lv_obj_t *sugar_ta = NULL;         // TextArea for Sugar Level
lv_obj_t *bp_ta = NULL;            // TextArea for Blood Pressure
lv_obj_t *sex_ta = NULL;           // TextArea for Sex
lv_obj_t *form_container = NULL;


// Global LVGL objects
lv_obj_t *btn_container = NULL;


// Wi-Fi components (optional)
// lv_obj_t *ssid_ta = NULL;
// String selected_ssid = "";

