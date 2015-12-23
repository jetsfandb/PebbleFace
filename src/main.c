#include <pebble.h>
#include "main.h"


static Window      *s_main_window;
static TextLayer   *s_dow_text_layer;
static TextLayer   *s_time_text_layer;
static TextLayer   *s_date_text_layer;
static BitmapLayer *s_battery_image_layer;
static TextLayer   *s_battery_pct_layer[10];
static TextLayer   *s_bluetooth_text_layer;
static BitmapLayer *s_bluetooth_conn_layer;

static GBitmap   *s_battery_base;
static GBitmap   *s_icon_bluetooth_connected;
static GBitmap   *s_icon_bluetooth_disconnected;
static GFont      s_normal_font;
static GFont      s_big_font;

static BatteryChargeState s_battery_charge_state;
static bool s_bluetooth_connected;

static void main_window_load(Window *window) {
  s_battery_charge_state = battery_state_service_peek();
  s_bluetooth_connected  = bluetooth_connection_service_peek();
  Layer *window_layer = window_get_root_layer(s_main_window);
  s_normal_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_big_font    = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  
  // Setup each text layer
    
  GRect window_bounds = layer_get_bounds(window_layer);
  GRect layer_bounds;
  layer_bounds.origin.x = 0;
  layer_bounds.size.w = window_bounds.size.w;
  s_battery_base                = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_BASE);  
  s_icon_bluetooth_connected    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_CONNECTED);  
  s_icon_bluetooth_disconnected = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_DISCONNECTED);  
  
  //Dow
  layer_bounds.origin.y = 0;
  layer_bounds.size.h = 26;
  s_dow_text_layer = text_layer_create(layer_bounds);
  text_layer_set_font(s_dow_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_dow_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_dow_text_layer));  
  
  //Date
  layer_bounds.origin.y = (layer_bounds.origin.y + layer_bounds.size.h);
  layer_bounds.size.h = 26;
  s_date_text_layer = text_layer_create(layer_bounds);
  text_layer_set_font(s_date_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));  
  
  //Time
  layer_bounds.origin.y = (layer_bounds.origin.y + layer_bounds.size.h);
  layer_bounds.size.h = 40;
  s_time_text_layer = text_layer_create(layer_bounds);
  text_layer_set_font(s_time_text_layer, s_big_font);
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));  
  
  //Battery - Image
  layer_bounds.origin.y = (layer_bounds.origin.y + layer_bounds.size.h);
  layer_bounds.size.h = 10;
  s_battery_image_layer = bitmap_layer_create(layer_bounds);
  bitmap_layer_set_bitmap(s_battery_image_layer, s_battery_base);
  bitmap_layer_set_alignment(s_battery_image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_battery_image_layer));  

  //Battery - Percentages
  GRect pct_bound;  
  int base_x_offset  = 6;
  pct_bound.origin.y = 2;
  pct_bound.size.w   = 12;
  pct_bound.size.h   = 6;
  
  for(int x = 0; x < 10; x++) {
    pct_bound.origin.x = (base_x_offset + (pct_bound.size.w * x) + (1 + x));
    s_battery_pct_layer[x] = text_layer_create(pct_bound);
    layer_add_child(bitmap_layer_get_layer(s_battery_image_layer), text_layer_get_layer(s_battery_pct_layer[x]));      
  }
  
  //Bluetooth
  layer_bounds.origin.y = (layer_bounds.origin.y + layer_bounds.size.h);
  GRect bluetooth_label_rect = layer_bounds;  
  bluetooth_label_rect.origin.x = 2;
  bluetooth_label_rect.size.h = 26;
  bluetooth_label_rect.size.w = 120;
  s_bluetooth_text_layer = text_layer_create(bluetooth_label_rect);
  text_layer_set_font(s_bluetooth_text_layer, s_normal_font);
  text_layer_set_text_alignment(s_bluetooth_text_layer, GTextAlignmentCenter);
  text_layer_set_text(s_bluetooth_text_layer, "Bluetooth:");
  layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_text_layer));  

  GRect bluetooth_state_rect = layer_bounds;  
  bluetooth_state_rect.origin.x = (bluetooth_label_rect.origin.x + bluetooth_label_rect.size.w);
  bluetooth_state_rect.size.h = 26;
  bluetooth_state_rect.size.w = 20;
  s_bluetooth_conn_layer = bitmap_layer_create(bluetooth_state_rect);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bluetooth_conn_layer));
}

static void main_window_unload(Window *window) {
  for(int x = 0; x < 10; x++) {
    text_layer_destroy(s_battery_pct_layer[x]);
  }
  text_layer_destroy(s_dow_text_layer);
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_date_text_layer);
  gbitmap_destroy(s_battery_base);
  gbitmap_destroy(s_icon_bluetooth_connected);
  gbitmap_destroy(s_icon_bluetooth_disconnected);
  bitmap_layer_destroy(s_battery_image_layer);
  text_layer_destroy(s_bluetooth_text_layer);
  bitmap_layer_destroy(s_bluetooth_conn_layer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Day of Week
  static char s_dow[11];
  strftime(s_dow, sizeof(s_dow), "%A", tick_time);
  text_layer_set_text(s_dow_text_layer, s_dow);
  
  // Date
  static char s_date[12];
  strftime(s_date, sizeof(s_date), "%B %e", tick_time);
  text_layer_set_text(s_date_text_layer, s_date);
  
  // Time
  static char s_time[9];
  strftime(s_time, sizeof(s_time), "%l:%M %p", tick_time);
  text_layer_set_text(s_time_text_layer, s_time);
  
  
  // Battery - Image
  int battery_pct_tenths = (s_battery_charge_state.charge_percent / 10);
  GColor battery_pct_color;
  if(battery_pct_tenths > 6) {
    battery_pct_color = GColorGreen;
  }
  else if (battery_pct_tenths > 3) {
    battery_pct_color = GColorChromeYellow;    
  }
  else {
    battery_pct_color = GColorRed;
  }
  for(int x = 0; x < 10; x++) {
    if(x < battery_pct_tenths) {
      text_layer_set_background_color(s_battery_pct_layer[x], battery_pct_color);
    }
    else {
      text_layer_set_background_color(s_battery_pct_layer[x], GColorWhite);      
    }
  }
  
  // Bluetooth
  bitmap_layer_set_bitmap(s_bluetooth_conn_layer, s_bluetooth_connected? s_icon_bluetooth_connected : s_icon_bluetooth_disconnected);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
static void battery_state_handler(BatteryChargeState bcs) {
  s_battery_charge_state = bcs;
}
static void bluetooth_state_handler(bool connected) {
  s_bluetooth_connected = connected;
}
static void init() {
  s_main_window = window_create();  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_state_handler);
  bluetooth_connection_service_subscribe(bluetooth_state_handler);
  update_time();
}


static void deinit() {
    tick_timer_service_unsubscribe();
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
  //  window_destroy(s_main_window);
}
int main(void) {
  init();
  app_event_loop();
  deinit();
}