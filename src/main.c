#include <pebble.h>
#include "main.h"


static Window    *s_main_window;
static TextLayer *s_dow_text_layer;
static TextLayer *s_time_text_layer;
static TextLayer *s_date_text_layer;
static TextLayer *s_battery_text_layer;
static TextLayer *s_bluetooth_text_layer;

static GFont      s_normal_font;
static GFont      s_big_font;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  s_normal_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_big_font    = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  
  // Setup each text layer
    
  GRect window_bounds = layer_get_bounds(window_layer);
  
  //Dow
  s_dow_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_dow_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_dow_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_dow_text_layer));  
  
  //Time
  s_time_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_time_text_layer, s_big_font);
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));  
  
  //Date
  s_date_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_date_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));  

  //Battery
  s_battery_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_battery_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_battery_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_battery_text_layer));  

  //Bluetooth
  s_bluetooth_text_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_bluetooth_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_bluetooth_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_text_layer));  

}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_dow_text_layer);
    text_layer_destroy(s_time_text_layer);
    text_layer_destroy(s_date_text_layer);
    text_layer_destroy(s_battery_text_layer);
    text_layer_destroy(s_bluetooth_text_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  GRect prev_bound;

  // Day of Week
  static char s_dow[11];
  strftime(s_dow, sizeof(s_dow), "%A", tick_time);
  text_layer_set_text(s_dow_text_layer, s_dow);
  GRect new_bound = layer_get_bounds(text_layer_get_layer(s_dow_text_layer));
  new_bound.size.h = 14;
  layer_set_bounds(text_layer_get_layer(s_dow_text_layer), new_bound);
  
  // Date
  static char s_date[12];
  strftime(s_date, sizeof(s_date), "%B %e", tick_time);
  text_layer_set_text(s_date_text_layer, s_date);
  prev_bound = new_bound;
  new_bound = layer_get_bounds(text_layer_get_layer(s_date_text_layer));
  new_bound.origin.y = (prev_bound.origin.y + prev_bound.size.h);
  new_bound.size.h = 14;
  layer_set_bounds(text_layer_get_layer(s_date_text_layer), new_bound);
  
  // Time
  static char s_time[9];
  strftime(s_time, sizeof(s_time), "%l:%M %p", tick_time);
  text_layer_set_text(s_time_text_layer, s_time);
  prev_bound = new_bound;
  new_bound = layer_get_bounds(text_layer_get_layer(s_time_text_layer));
  new_bound.origin.y = (prev_bound.origin.y + prev_bound.size.h);
  new_bound.size.h = 20;
  layer_set_bounds(text_layer_get_layer(s_time_text_layer), new_bound);
  
  // Battery
  static char s_battery[30];
  BatteryChargeState bcs = battery_state_service_peek();
  snprintf(s_battery, sizeof(s_battery), "Battery: %d%%", bcs.charge_percent);
  text_layer_set_text(s_battery_text_layer, s_battery);
  prev_bound = new_bound;
  new_bound = layer_get_bounds(text_layer_get_layer(s_battery_text_layer));
  new_bound.origin.y = (prev_bound.origin.y + prev_bound.size.h);
  new_bound.size.h = 15;
  layer_set_bounds(text_layer_get_layer(s_battery_text_layer), new_bound);
  
  // Bluetooth
  static char s_bluetooth[30];
  bool bt_connected = bluetooth_connection_service_peek();
  snprintf(s_bluetooth, sizeof(s_bluetooth), "Bluetooth: %s", bt_connected ? "Yes" : "No");
  text_layer_set_text(s_bluetooth_text_layer, s_bluetooth);
  prev_bound = new_bound;
  new_bound = layer_get_bounds(text_layer_get_layer(s_bluetooth_text_layer));
  new_bound.origin.y = (prev_bound.origin.y + prev_bound.size.h);
  new_bound.size.h = 15;
  layer_set_bounds(text_layer_get_layer(s_bluetooth_text_layer), new_bound);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  update_time();
}


static void deinit() {
    tick_timer_service_unsubscribe();
    window_destroy(s_main_window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}



