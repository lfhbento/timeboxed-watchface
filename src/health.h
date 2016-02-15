#ifndef __TIMEBOXED_HEALTH_
#define __TIMEBOXED_HEALTH_

#include <pebble.h>
#include "keys.h"

void toggle_health(bool);

bool is_user_sleeping();

void get_health_data();

void show_sleep_data_if_visible();

void init_sleep_data();

#endif
