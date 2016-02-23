#ifndef __TIMEBOXED_TEXT_
#define __TIMEBOXED_TEXT_

#include <pebble.h>

void create_text_layers(Window*);

void destroy_text_layers();

void load_face_fonts();
void unload_face_fonts();
void set_face_fonts();
void set_colors(Window*);

uint8_t get_loaded_font();

void set_steps_dist_color(bool falling_behind_steps, bool falling_behind_dist);
void set_bluetooth_color();
void set_update_color();
void set_battery_color();

void set_hours_layer_text(char*);
void set_date_layer_text(char*);
void set_alt_time_layer_text(char*);
void set_battery_layer_text(char*);
void set_bluetooth_layer_text(char*);
void set_temp_cur_layer_text(char*);
void set_temp_max_layer_text(char*);
void set_temp_min_layer_text(char*);
void set_steps_or_sleep_layer_text(char*);
void set_dist_or_deep_layer_text(char*);
void set_weather_layer_text(char*);
void set_max_icon_layer_text(char*);
void set_min_icon_layer_text(char*);
void set_update_layer_text(char*);


#endif
