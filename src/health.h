#ifndef __TIMEBOXED_HEALTH_
#define __TIMEBOXED_HEALTH_

void toggle_health(bool);

bool is_user_sleeping();

void get_health_data();

void queue_health_update();

void show_sleep_data_if_visible();

void init_sleep_data();

void save_health_data_to_storage();

bool should_show_sleep_data();

#endif
