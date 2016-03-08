#ifndef __TIMEBOXED_CONFIGS
#define __TIMEBOXED_CONFIGS

void set_config_toggles(int);
int get_config_toggles();

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

#endif
