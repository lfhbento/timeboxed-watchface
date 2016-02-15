#ifndef __TIMEBOXED_LOCALE_
#define __TIMEBOXED_LOCALE_

void get_current_date(struct tm* tick_time, char* buffer, int buf_size);
void load_locale();

#endif
