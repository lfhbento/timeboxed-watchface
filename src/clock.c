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
static signed int tz_hour_b;
static uint8_t tz_minute_b;
static char tz_name_b[TZ_LEN];

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

static void update_timezone(tm* tick_time, time_t* temp_gmt, int hour, int minute, char* code, char* text, int text_size) {
    struct tm *gmt_time = gmtime(temp_gmt);
    gmt_time->tm_hour = gmt_time->tm_hour + hour;
    mktime(gmt_time);
    gmt_time->tm_min = gmt_time->tm_min + (hour >= 0 ? minute : -1*minute);
    mktime(gmt_time);

    set_hours(gmt_time, text, text_size);

    if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday > tick_time->tm_mday) ||
        (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon > tick_time->tm_mon) ||
        (gmt_time->tm_year > tick_time->tm_year)
    ) {
            strcat(text, "+1");
    } else if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday < tick_time->tm_mday) ||
        (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon < tick_time->tm_mon) ||
        (gmt_time->tm_year < tick_time->tm_year)
    ) {
        strcat(text, "-1");
    }

    strcat(text, " ");
    strcat(text, code);

    for (unsigned char i = 0; text[i]; ++i) {
        text[i] = toupper((unsigned char)text[i]);
    }
}

void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    time_t temp_gmt = time(NULL);
    #if !defined PBL_PLATFORM_APLITE
    time_t temp_gmt_b = time(NULL);
    #endif
    struct tm *tick_time = localtime(&temp);

    char tz_text[22];
    #if !defined PBL_PLATFORM_APLITE
    char tz_text_b[22];
    #endif
    char hour_text[13];
    char date_text[13];

    set_hours(tick_time, hour_text, sizeof(hour_text));

    if (is_module_enabled(MODULE_TIMEZONE) && tz_name[0] != '#') {
        update_timezone(tick_time, &temp_gmt, tz_hour, tz_minute, tz_name, tz_text, sizeof(tz_text));
        set_alt_time_layer_text(tz_text);
    } else {
        set_alt_time_layer_text("");
    }

    #if !defined PBL_PLATFORM_APLITE
    if (is_module_enabled(MODULE_TIMEZONEB) && tz_name_b[0] != '#') {
        update_timezone(tick_time, &temp_gmt_b, tz_hour_b, tz_minute_b, tz_name_b, tz_text_b, sizeof(tz_text_b));
        set_alt_time_b_layer_text(tz_text_b);
    } else {
        set_alt_time_b_layer_text("");
    }
    #endif

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
    if (is_module_enabled(MODULE_TIMEZONEB) && is_timezone_enabled() && persist_exists(KEY_TIMEZONESBCODE)) {
        persist_read_string(KEY_TIMEZONESBCODE, tz_name_b, sizeof(tz_name_b));
        tz_hour_b = persist_exists(KEY_TIMEZONESB) ? persist_read_int(KEY_TIMEZONESB) : 0;
        tz_minute_b = persist_exists(KEY_TIMEZONESBMINUTES) ? persist_read_int(KEY_TIMEZONESBMINUTES) : 0;
    }
}

void set_timezone(char *name, int hour, int minute) {
   strcpy(tz_name, name);
   tz_hour = hour;
   tz_minute = minute;
}

void set_timezone_b(char *name, int hour, int minute) {
   strcpy(tz_name_b, name);
   tz_hour_b = hour;
   tz_minute_b = minute;
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
