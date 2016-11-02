#ifndef __TIMEBOXED_ACCEL_
#define __TIMEBOXED_ACCEL_

#include <pebble.h>

void accel_data_handler(AccelData *data, uint32_t num_samples);
bool tap_mode_visible();
void init_accel_service(Window * watchface);
bool wrist_mode_visible();
void reset_wrist_handler();
void reset_tap_handler();
void shake_data_handler(AccelAxisType axis, int32_t direction);

#endif
