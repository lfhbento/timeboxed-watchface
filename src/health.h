#ifndef __TIMEBOXED_HEALTH_
#define __TIMEBOXED_HEALTH_

#include <pebble.h>

#if defined PBL_HEALTH
void toggle_health(uint8_t);
void get_health_data();
void queue_health_update();
void show_sleep_data_if_visible(Window *watchface);
void init_sleep_data();
void save_health_data_to_storage();
#endif

bool is_user_sleeping();
bool should_show_sleep_data();

#endif
