#ifndef __TIMEBOXED_SCREEN_
#define __TIMEBOXED_SCREEN_

#include <pebble.h>

void load_screen(uint8_t reload_origin, Window *watchface);
void redraw_screen(Window *watchface);
void reload_fonts();
void recreate_text_layers(Window * watchface);
void bt_handler(bool connected);
void battery_handler(BatteryChargeState battery_state);
void update_time();
void check_for_updates();
void notify_update(int update_available);
void load_timezone();

#endif
