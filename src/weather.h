#ifndef __TIMEBOXED_WEATHER_
#define __TIMEBOXED_WEATHER_

void update_weather();
void update_weather_values(int temp_val, int weather_val);
void update_forecast_values(int max_val, int min_val);
void update_wind_values(int speed, int direction);
void store_weather_values(int temp_val, int max_val, int min_val, int weather_val, int speed_val, int direction_val);
void toggle_weather();
bool is_weather_enabled();

#endif
