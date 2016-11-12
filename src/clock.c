#include <pebble.h>
#include <ctype.h>
#include <time.h>
#include "text.h"
#include "clock.h"
#include "locales.h"
#include "configs.h"
#include "keys.h"

static signed int tz_hour;
static uint8_t tz_minute;
static char tz_name[TZ_LEN];

void set_hours(struct tm* tick_time, char* hour_text, int hour_text_len) {
    if (!is_leading_zero_disabled()) {
        strftime(hour_text, hour_text_len, (clock_is_24h_style() ? "%H:%M" : "%I:%M"), tick_time);
    } else {
        strftime(hour_text, hour_text_len, (clock_is_24h_style() ? "%k:%M" : "%l:%M"), tick_time);
        if (hour_text[0] == ' ') {
            for (int i = 0; i < hour_text_len; ++i) {
                hour_text[i] = hour_text[i+1];
            }
            hour_text[hour_text_len - 1] = '\0';
        }
    }
}

void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    struct tm *gmt_time = gmtime(&temp);
    gmt_time->tm_hour = gmt_time->tm_hour + tz_hour;
    mktime(gmt_time);
    gmt_time->tm_min = gmt_time->tm_min + (tz_hour >= 0 ? tz_minute : -1*tz_minute);
    mktime(gmt_time);
    char tz_text[22];
    char hour_text[13];
    char date_text[13];

    set_hours(tick_time, hour_text, sizeof(hour_text));

    if (is_module_enabled(MODULE_TIMEZONE)) {
        set_hours(gmt_time, tz_text, sizeof(tz_text));

        if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday > tick_time->tm_mday) ||
            (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon > tick_time->tm_mon) ||
            (gmt_time->tm_year > tick_time->tm_year)
        ) {
                strcat(tz_text, "+1");
        } else if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday < tick_time->tm_mday) ||
            (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon < tick_time->tm_mon) ||
            (gmt_time->tm_year < tick_time->tm_year)
        ) {
            strcat(tz_text, "-1");
        }

        strcat(tz_text, " ");
        strcat(tz_text, tz_name);

        for (unsigned char i = 0; tz_text[i]; ++i) {
            tz_text[i] = toupper((unsigned char)tz_text[i]);
        }

        set_alt_time_layer_text(tz_text);
    } else {
        set_alt_time_layer_text("");
    }

    set_hours_layer_text(hour_text);
    get_current_date(tick_time, date_text, sizeof(date_text), persist_exists(KEY_DATESEPARATOR) ? persist_read_int(KEY_DATESEPARATOR) : 1);
    set_date_layer_text(date_text);
}


void load_timezone_from_storage() {
    if (is_module_enabled(MODULE_TIMEZONE) && is_timezone_enabled() && persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
        tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
        tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    }
}

void set_timezone(char *name, int hour, int minute) {
   strcpy(tz_name, name);
   tz_hour = hour;
   tz_minute = minute;
}

void update_seconds(struct tm* tick_time) {
    if (is_module_enabled(MODULE_SECONDS)) {
        char seconds_text[4];
        strftime(seconds_text, sizeof(seconds_text), "%S", tick_time);
        set_seconds_layer_text(seconds_text);
    } else {
        set_seconds_layer_text("");
    }
}
