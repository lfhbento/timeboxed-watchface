#ifndef __TIMEBOXED_ACCEL_
#define __TIMEBOXED_ACCEL_

#include <pebble.h>

#if !defined PBL_PLATFORM_APLITE
void accel_data_handler(AccelData *data, uint32_t num_samples);
void reset_tap_handler();
void init_accel_service(Window * watchface);
void reset_wrist_handler();
void shake_data_handler(AccelAxisType axis, int32_t direction);
#endif
bool tap_mode_visible();
bool wrist_mode_visible();

#endif
