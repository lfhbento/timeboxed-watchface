#ifndef __TIMEBOXED_ACCEL_
#define __TIMEBOXED_ACCEL_

#include <pebble.h>

void accel_data_handler(AccelData *data, uint32_t num_samples);
bool tap_mode_visible();
void init_accel_service(Window * watchface);

#endif
