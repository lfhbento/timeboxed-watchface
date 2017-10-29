#include <pebble.h>
#ifndef __TIMEBOXED_CONFIGS
#define __TIMEBOXED_CONFIGS

void set_config_toggles(int);
int get_config_toggles();
bool is_module_enabled(int);
bool is_module_enabled_any(int);
int get_slot_for_module(int);

void set_module(int, int, int);

bool is_weather_toggle_enabled();
bool is_health_toggle_enabled();
bool is_use_celsius_enabled();
bool is_use_km_enabled();
bool is_bluetooth_vibrate_enabled();
bool is_update_disabled();
bool is_leading_zero_disabled();
bool is_advanced_colors_enabled();
bool is_sleep_data_enabled();
bool is_use_calories_enabled();
bool is_simple_mode_enabled();
bool is_timezone_enabled();
bool is_quickview_disabled();
bool is_tap_enabled();
bool is_wrist_enabled();
bool is_mute_on_quiet_enabled();

int get_wind_speed_unit();
void toggle_center_slots(bool);

#endif
