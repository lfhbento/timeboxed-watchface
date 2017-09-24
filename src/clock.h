#ifndef __TIMEBOXED_CLOCK_
#define __TIMEBOXED_CLOCK_
#include <time.h>

void set_hours(struct tm* tick_time, char* hour_text, int hour_text_len);
void update_time();
void load_timezone_from_storage();
void set_timezone(char *name, int hour, int minute);
void update_seconds(struct tm* tick_time);

#endif
