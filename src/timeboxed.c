#include <pebble.h>
#include <ctype.h>

#define KEY_TEMP 0
#define KEY_MAX 1
#define KEY_MIN 2
#define KEY_WEATHER 3
#define KEY_ENABLEHEALTH 4
#define KEY_USEKM 5
#define KEY_SHOWSLEEP 6
#define KEY_ENABLEWEATHER 7
#define KEY_WEATHERKEY 8
#define KEY_USECELSIUS 9
#define KEY_TIMEZONES 10
#define KEY_BGCOLOR 11
#define KEY_HOURSCOLOR 12
#define KEY_ENABLEADVANCED 13
#define KEY_DATECOLOR 14
#define KEY_ALTHOURSCOLOR 15
#define KEY_BATTERYCOLOR 16
#define KEY_BATTERYLOWCOLOR 17
#define KEY_WEATHERCOLOR 18
#define KEY_TEMPCOLOR 19
#define KEY_MINCOLOR 20
#define KEY_MAXCOLOR 21
#define KEY_STEPSCOLOR 22
#define KEY_DISTCOLOR 23
#define KEY_TIMEZONESCODE 24
#define KEY_TIMEZONESMINUTES 25

static Window *watchface;
static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps_or_sleep;
static TextLayer *dist_or_deep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static GFont time_font;
static GFont date_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_font_big;

static char hour_text[8];
static char date_text[13];
static char temp_text[8];
static char max_text[8];
static char min_text[8];
static char weather_text[4];
static char tz_text[20];
static char s_battery_buffer[7];
static char steps_or_sleep_text[16];
static char dist_or_deep_text[16];
static bool has_health;
static bool weather_enabled;
static bool health_enabled;
static bool sleep_data_visible;
static bool was_asleep;
static int woke_up_at_hour;
static int woke_up_at_min;
static int tz_hour;
static int tz_minute;
static char tz_name[TZ_LEN];
static char* weather_conditions[] = {
    "\U0000F07B", // 'unknown': 0,
    "\U0000F00D", // 'clear': 1,
    "\U0000F00D", // 'sunny': 2,
    "\U0000F002", // 'partlycloudy': 3,
    "\U0000F002", // 'mostlycloudy': 4,
    "\U0000F00C", // 'mostlysunny': 5,
    "\U0000F002", // 'partlysunny': 6,
    "\U0000F013", // 'cloudy': 7,
    "\U0000F019", // 'rain': 8,
    "\U0000F01B", // 'snow': 9,
    "\U0000F01D", // 'tstorms': 10,
    "\U0000F0b5", // 'sleat': 11,
    "\U0000F00A", // 'flurries': 12,
    "\U0000F0b6", // 'hazy': 13,
    "\U0000F01D", // 'chancetstorms': 14,
    "\U0000F01B", // 'chancesnow': 15,
    "\U0000F0b5", // 'chancesleat': 16,
    "\U0000F008", // 'chancerain': 17,
    "\U0000F01B", // 'chanceflurries': 18,
    "\U0000F07B", // 'nt_unknown': 19,
    "\U0000F02E", // 'nt_clear': 20,
    "\U0000F02E", // 'nt_sunny': 21,
    "\U0000F086", // 'nt_partlycloudy': 22,
    "\U0000F086", // 'nt_mostlycloudy': 23,
    "\U0000F081", // 'nt_mostlysunny': 24,
    "\U0000F086", // 'nt_partlysunny': 25,
    "\U0000F013", // 'nt_cloudy': 26,
    "\U0000F019", // 'nt_rain': 27,
    "\U0000F01B", // 'nt_snow': 28,
    "\U0000F01D", // 'nt_tstorms': 29,
    "\U0000F0b5", // 'nt_sleat': 30,
    "\U0000F038", // 'nt_flurries': 31,
    "\U0000F04A", // 'nt_hazy': 32,
    "\U0000F01D", // 'nt_chancetstorms': 33,
    "\U0000F038", // 'nt_chancesnow': 34,
    "\U0000F0B3", // 'nt_chancesleat': 35,
    "\U0000F036", // 'nt_chancerain': 36,
    "\U0000F038", // 'nt_chanceflurries': 37,
};


static void update_time() {
    // Get a tm structuiire
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    struct tm *gmt_time = gmtime(&temp);
    gmt_time->tm_hour = gmt_time->tm_hour + tz_hour;
    mktime(gmt_time);
    gmt_time->tm_min = gmt_time->tm_min + tz_minute;
    mktime(gmt_time);
    

    // Write the current hours and minutes into a buffer
    strftime(hour_text, sizeof(hour_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), tick_time);
    strftime(date_text, sizeof(date_text), "%a.%b.%d", tick_time);

    if (tz_name[0] != '#') {
        strftime(tz_text, sizeof(tz_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), gmt_time);

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

        strcat(tz_text, ".");
        strcat(tz_text, tz_name); 

        for (unsigned char i = 0; tz_text[i]; ++i) {
            tz_text[i] = tolower((unsigned char)tz_text[i]);
        }
        text_layer_set_text(alt_time, tz_text);
    } else {
        text_layer_set_text(alt_time, "");
    }

    for (unsigned char i = 0; date_text[i]; ++i) {
        date_text[i] = tolower((unsigned char)date_text[i]);
    }
    text_layer_set_text(hours, hour_text);
    text_layer_set_text(date, date_text);
}

static void set_colors(void) {
    GColor base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    bool enableAdvanced = persist_exists(KEY_ENABLEADVANCED) ? persist_read_int(KEY_ENABLEADVANCED) : 0;
    text_layer_set_text_color(date, 
            enableAdvanced && persist_exists(KEY_DATECOLOR) ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);
    text_layer_set_text_color(alt_time, 
            enableAdvanced && persist_exists(KEY_ALTHOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_ALTHOURSCOLOR)) : base_color);
    text_layer_set_text_color(weather, 
            enableAdvanced && persist_exists(KEY_WEATHERCOLOR) ? GColorFromHEX(persist_read_int(KEY_WEATHERCOLOR)) : base_color);
    text_layer_set_text_color(temp_cur, 
            enableAdvanced && persist_exists(KEY_TEMPCOLOR) ? GColorFromHEX(persist_read_int(KEY_TEMPCOLOR)) : base_color);
    text_layer_set_text_color(temp_min, 
            enableAdvanced && persist_exists(KEY_MINCOLOR) ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(min_icon, 
            enableAdvanced && persist_exists(KEY_MINCOLOR) ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(temp_max, 
            enableAdvanced && persist_exists(KEY_MAXCOLOR) ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(max_icon, 
            enableAdvanced && persist_exists(KEY_MAXCOLOR) ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(steps_or_sleep, 
            enableAdvanced && persist_exists(KEY_STEPSCOLOR) ? GColorFromHEX(persist_read_int(KEY_STEPSCOLOR)) : base_color);
    text_layer_set_text_color(dist_or_deep, 
            enableAdvanced && persist_exists(KEY_DISTCOLOR) ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color);

    BatteryChargeState charge_state = battery_state_service_peek();
    if (charge_state.charge_percent > 20) {
        text_layer_set_text_color(battery, 
            enableAdvanced && persist_read_int(KEY_BATTERYCOLOR) ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color);
    } else {
        text_layer_set_text_color(battery, 
            enableAdvanced && persist_read_int(KEY_BATTERYLOWCOLOR) ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color);
    }

    window_set_background_color(watchface, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);
}

#if defined(PBL_HEALTH)
static void update_steps_data(void) {

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    time_t start_last_week = start - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7);
    time_t end_last_week = end - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7);

    uint16_t current_steps = 0;
    uint16_t current_dist = 0;
    uint16_t current_dist_int = 0;
    uint16_t current_dist_dec = 0;
    uint16_t steps_last_week = 0;
    uint16_t dist_last_week = 0;
    

    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);
    HealthServiceAccessibilityMask mask_steps_last_week =
        health_service_metric_accessible(metric_steps, start_last_week, end_last_week);
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);
    HealthServiceAccessibilityMask mask_dist_last_week =
        health_service_metric_accessible(metric_dist, start_last_week, end_last_week);
    
    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), " %d", current_steps);

        if (mask_steps_last_week & HealthServiceAccessibilityMaskAvailable) {
            steps_last_week = (int)health_service_sum(metric_steps, start_last_week, end_last_week);
            if (steps_last_week < current_steps) {
                steps_or_sleep_text[0] = '+';
            }
        }
        
        APP_LOG(APP_LOG_LEVEL_INFO, steps_or_sleep_text);
        text_layer_set_text(steps_or_sleep, steps_or_sleep_text);
    }

    if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        bool useKm = persist_exists(KEY_USEKM) && persist_read_int(KEY_USEKM);
        current_dist = (int)health_service_sum_today(metric_dist);
        if (!useKm) {
            current_dist /= 1.6;
        }
        current_dist_int = current_dist/1000;
        current_dist_dec = (current_dist%1000)/100;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), (useKm ? " %d.%dkm" : " %d.%dmi"), current_dist_int, current_dist_dec);
        
        if (mask_dist_last_week & HealthServiceAccessibilityMaskAvailable) {
            dist_last_week = (int)health_service_sum(metric_dist, start_last_week, end_last_week);
            if (dist_last_week < current_dist) {
                dist_or_deep_text[0] = '+';
            }
        }

        APP_LOG(APP_LOG_LEVEL_INFO, dist_or_deep_text);
        text_layer_set_text(dist_or_deep, dist_or_deep_text);
    }
    
    HealthActivityMask activities = health_service_peek_current_activities();
    bool is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
    if (is_sleeping) {
        APP_LOG(APP_LOG_LEVEL_INFO, "We are asleep.");
        text_layer_set_text(steps_or_sleep, "zzz");
        text_layer_set_text(dist_or_deep, "zzz");
    }
}

static void update_sleep_data(void) {
    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;
    
    time_t start = time_start_of_today() - (SECONDS_PER_HOUR * 4); // 8pm
    time_t end = time(NULL);
    time_t start_last_week = start - (SECONDS_PER_HOUR * 4) - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7); // last week 8pm
    time_t end_last_week = end - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7);

    uint16_t current_sleep = 0;
    uint16_t current_deep = 0;
    uint16_t sleep_last_week = 0;
    uint16_t deep_last_week = 0;
    
    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_sleep_last_week =
        health_service_metric_accessible(metric_sleep, start_last_week, end_last_week);
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);
    HealthServiceAccessibilityMask mask_deep_last_week =
        health_service_metric_accessible(metric_deep, start_last_week, end_last_week);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum(metric_sleep, start, end);
        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = (current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), " %dh%dm", current_sleep_hours, current_sleep_minutes);
        
        if (mask_sleep_last_week & HealthServiceAccessibilityMaskAvailable) {
            sleep_last_week = (int)health_service_sum(metric_sleep, start_last_week, end_last_week);
            if (sleep_last_week < current_sleep) {
                steps_or_sleep_text[0] = '+';
            }
        }
        
        APP_LOG(APP_LOG_LEVEL_INFO, steps_or_sleep_text);
        text_layer_set_text(steps_or_sleep, steps_or_sleep_text);
    }

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum(metric_deep, start, end);
        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = (current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), " %dh%dm", current_deep_hours, current_deep_minutes);
        
        if (mask_deep_last_week & HealthServiceAccessibilityMaskAvailable) {
            deep_last_week = (int)health_service_sum(metric_deep, start_last_week, end_last_week);
            if (deep_last_week < current_deep) {
                dist_or_deep_text[0] = '+';
            }
        }
        
        APP_LOG(APP_LOG_LEVEL_INFO, dist_or_deep_text);
        text_layer_set_text(dist_or_deep, dist_or_deep_text);
    }

}

static void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
            break;
        case HealthEventMovementUpdate:
            if (!sleep_data_visible) {
                APP_LOG(APP_LOG_LEVEL_INFO, "Updating steps data.");
                update_steps_data();
            }
            break;
        case HealthEventSleepUpdate:
            if (sleep_data_visible) {
                APP_LOG(APP_LOG_LEVEL_INFO, "Updating sleep data.");
                update_sleep_data();
            }
            break;
    }
}

#endif

static void toggle_health(void) {
    has_health = false;
    health_enabled = false;
    #if defined(PBL_HEALTH)
        health_enabled = persist_read_int(KEY_ENABLEHEALTH);
        if (health_enabled) {
            has_health = health_service_events_subscribe(health_handler, NULL);
            if (has_health && !sleep_data_visible) {
                update_steps_data();
            }
        } else {
            has_health = false;
            health_service_events_unsubscribe();
        }
    #endif

    if (!health_enabled) {
        text_layer_set_text(steps_or_sleep, "");
        text_layer_set_text(dist_or_deep, "");
    }
}

static void update_weather(void) {
    psleep(500);
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    char weather_key_buffer[20];
    if (persist_exists(KEY_WEATHERKEY)) {
        persist_read_string(KEY_WEATHERKEY, weather_key_buffer, sizeof(weather_key_buffer));
    } else {
        weather_key_buffer[0] = '\0';
    }

    APP_LOG(APP_LOG_LEVEL_INFO, weather_key_buffer);
    dict_write_uint8(iter, KEY_USECELSIUS, 
        persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS) ? persist_read_int(KEY_USECELSIUS) : 0);
    dict_write_cstring(iter, KEY_WEATHERKEY, weather_key_buffer);
    app_message_outbox_send();
}

static void update_weather_values(int temp_val, int max_val, int min_val, int weather_val) {
    bool useCelsius = persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS);
        
    snprintf(temp_text, sizeof(temp_text), (useCelsius ? "%dc" : "%df"), temp_val);
    snprintf(max_text, sizeof(max_text), "%d", max_val);
    snprintf(min_text, sizeof(min_text), "%d", min_val);
    snprintf(weather_text, sizeof(weather_text), "%s", weather_conditions[weather_val]);

    persist_write_int(KEY_TEMP, temp_val);
    persist_write_int(KEY_MAX, max_val);
    persist_write_int(KEY_MIN, min_val);
    persist_write_int(KEY_WEATHER, weather_val);

    text_layer_set_text(temp_cur, temp_text);
    text_layer_set_text(temp_max, max_text);
    text_layer_set_text(temp_min, min_text);
    text_layer_set_text(weather, weather_text);
    text_layer_set_text(min_icon, "\U0000F044");
    text_layer_set_text(max_icon, "\U0000F058");
}

static void toggle_weather(void) {
    weather_enabled = persist_exists(KEY_ENABLEWEATHER) && persist_read_int(KEY_ENABLEWEATHER);
    if (weather_enabled) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather enabled, retrieving weather.");
        if (persist_exists(KEY_TEMP) && persist_exists(KEY_MAX) && persist_exists(KEY_MIN) && persist_exists(KEY_WEATHER)) {
            APP_LOG(APP_LOG_LEVEL_INFO, "Using stored data.");
            int temp_val = persist_read_int(KEY_TEMP);
            int max_val = persist_read_int(KEY_MAX);
            int min_val = persist_read_int(KEY_MIN);
            int weather_val = persist_read_int(KEY_WEATHER);

            update_weather_values(temp_val, max_val, min_val, weather_val);
        }
        update_weather();
    } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather disabled, clearing up");
        text_layer_set_text(temp_cur, "");
        text_layer_set_text(temp_max, "");
        text_layer_set_text(temp_min, "");
        text_layer_set_text(weather, "");
        text_layer_set_text(min_icon, "");
        text_layer_set_text(max_icon, "");
    }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
    Tuple *max_tuple = dict_find(iterator, KEY_MAX);
    Tuple *min_tuple = dict_find(iterator, KEY_MIN);
    Tuple *weather_tuple = dict_find(iterator, KEY_WEATHER);

    if(temp_tuple && max_tuple && min_tuple && weather_tuple && weather_enabled) {
        int temp_val = (int)temp_tuple->value->int32;
        int max_val = (int)max_tuple->value->int32;
        int min_val = (int)min_tuple->value->int32;
        int weather_val = (int)weather_tuple->value->int32;

        update_weather_values(temp_val, max_val, min_val, weather_val);

        APP_LOG(APP_LOG_LEVEL_INFO, "Weather updated!");
        return;
    }

    Tuple *enableHealth = dict_find(iterator, KEY_ENABLEHEALTH);
    if (enableHealth) {
        bool health = enableHealth->value->int32;
        persist_write_int(KEY_ENABLEHEALTH, health);
    }

    Tuple *useKm = dict_find(iterator, KEY_USEKM);
    if (useKm) {
        bool km = useKm->value->int8;
        persist_write_int(KEY_USEKM, km);
    }

    Tuple *showSleep = dict_find(iterator, KEY_SHOWSLEEP);
    if (showSleep) {
        bool sleep = showSleep->value->int8;
        persist_write_int(KEY_SHOWSLEEP, sleep);
    }

    Tuple *enableWeather = dict_find(iterator, KEY_ENABLEWEATHER);
    if (enableWeather) {
        bool weather = enableWeather->value->int8;
        persist_write_int(KEY_ENABLEWEATHER, weather);
    }

    Tuple *weatherKey = dict_find(iterator, KEY_WEATHERKEY);
    if (weatherKey) {
        char* key = weatherKey->value->cstring;
        persist_write_string(KEY_WEATHERKEY, key);
    }

    Tuple *useCelsius = dict_find(iterator, KEY_USECELSIUS);
    if (useCelsius) {
        bool celsius = useCelsius->value->int8;
        persist_write_int(KEY_USECELSIUS, celsius);
    }

    Tuple *timezones = dict_find(iterator, KEY_TIMEZONES);
    if (timezones) {
        int tz = timezones->value->int8;
        persist_write_int(KEY_TIMEZONES, tz);
        tz_hour = tz;
    }

    Tuple *timezonesMin = dict_find(iterator, KEY_TIMEZONESMINUTES);
    if (timezonesMin) {
        int tz_min = timezonesMin->value->int8;
        persist_write_int(KEY_TIMEZONESMINUTES, tz_min);
        tz_minute = tz_min;
    }

    Tuple *timezonesCode = dict_find(iterator, KEY_TIMEZONESCODE);
    if (timezones) {
        char* tz_code = timezonesCode->value->cstring;
        persist_write_string(KEY_TIMEZONESCODE, tz_code);
        strcpy(tz_name, tz_code);
    }

    Tuple *bgColor = dict_find(iterator, KEY_BGCOLOR);
    if (bgColor) {
        uint32_t bg_c = bgColor->value->int32;
        persist_write_int(KEY_BGCOLOR, bg_c);
    }

    Tuple *hoursColor = dict_find(iterator, KEY_HOURSCOLOR);
    if (hoursColor) {
        uint32_t time_c = hoursColor->value->int32;
        persist_write_int(KEY_HOURSCOLOR, time_c);
    }

    Tuple *enableAdvanced = dict_find(iterator, KEY_ENABLEADVANCED);
    if (enableAdvanced) {
        bool adv = enableAdvanced->value->int8;
        persist_write_int(KEY_ENABLEADVANCED, adv);
    }

    Tuple *dateColor = dict_find(iterator, KEY_DATECOLOR);
    if (dateColor) {
        uint32_t date_c = dateColor->value->int32;
        persist_write_int(KEY_DATECOLOR, date_c);
    }

    Tuple *altHoursColor = dict_find(iterator, KEY_ALTHOURSCOLOR);
    if (altHoursColor) {
        uint32_t alt_c = altHoursColor->value->int32;
        persist_write_int(KEY_ALTHOURSCOLOR, alt_c);
    }

    Tuple *batteryColor = dict_find(iterator, KEY_BATTERYCOLOR);
    if (batteryColor) {
        uint32_t bat_c = batteryColor->value->int32;
        persist_write_int(KEY_BATTERYCOLOR, bat_c);
    }

    Tuple *batteryLowColor = dict_find(iterator, KEY_BATTERYLOWCOLOR);
    if (batteryLowColor) {
        uint32_t batl_c = batteryLowColor->value->int32;
        persist_write_int(KEY_BATTERYLOWCOLOR, batl_c);
    }

    Tuple *weatherColor = dict_find(iterator, KEY_WEATHERCOLOR);
    if (weatherColor) {
        uint32_t weather_c = weatherColor->value->int32;
        persist_write_int(KEY_WEATHERCOLOR, weather_c);
    }

    Tuple *tempColor = dict_find(iterator, KEY_TEMPCOLOR);
    if (tempColor) {
        uint32_t temp_c = tempColor->value->int32;
        persist_write_int(KEY_TEMPCOLOR, temp_c);
    }

    Tuple *minColor = dict_find(iterator, KEY_MINCOLOR);
    if (minColor) {
        uint32_t min_c = minColor->value->int32;
        persist_write_int(KEY_MINCOLOR, min_c);
    }

    Tuple *maxColor = dict_find(iterator, KEY_MAXCOLOR);
    if (maxColor) {
        uint32_t max_c = maxColor->value->int32;
        persist_write_int(KEY_MAXCOLOR, max_c);
    }

    Tuple *stepsColor = dict_find(iterator, KEY_STEPSCOLOR);
    if (stepsColor) {
        uint32_t steps_c = stepsColor->value->int32;
        persist_write_int(KEY_STEPSCOLOR, steps_c);
    }

    Tuple *distColor = dict_find(iterator, KEY_DISTCOLOR);
    if (distColor) {
        uint32_t dist_c = distColor->value->int32;
        persist_write_int(KEY_DISTCOLOR, dist_c);
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "All persisted!");
    set_colors();
    update_time();
    toggle_health();
    toggle_weather();

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void battery_handler(BatteryChargeState charge_state) {

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }

    text_layer_set_text(battery, s_battery_buffer);
}

static void watchface_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
    base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_20));
    weather_font_big = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));

    GRect bounds = layer_get_bounds(window_layer);

    hours = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_color(hours, GColorWhite);
    text_layer_set_text_alignment(hours, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(hours, time_font);
    
    date = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -2), PBL_IF_ROUND_ELSE(100, 92), bounds.size.w, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_alignment(date, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(date, date_font);
    text_layer_set_text(date, "");
    
    alt_time = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -2), PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(alt_time, base_font);
    text_layer_set_text(alt_time, "");

    battery = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(122, 114), bounds.size.w, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(battery, base_font);
    text_layer_set_text(battery, "");

    weather = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(-14, 4), PBL_IF_ROUND_ELSE(2, 0), bounds.size.w, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(weather, weather_font);
    text_layer_set_text(weather, "");

    temp_cur = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(14, 36), PBL_IF_ROUND_ELSE(4, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(temp_cur, base_font);
    text_layer_set_text(temp_cur, "");

    temp_min = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(70, 85), PBL_IF_ROUND_ELSE(22, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);
    text_layer_set_font(temp_min, base_font);
    text_layer_set_text(temp_min, "");

    min_icon = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(60, 75), PBL_IF_ROUND_ELSE(18, -2), bounds.size.w, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);
    text_layer_set_font(min_icon, weather_font_big);
    text_layer_set_text(min_icon, "");

    temp_max = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(105, 120), PBL_IF_ROUND_ELSE(22, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);
    text_layer_set_font(temp_max, base_font);
    text_layer_set_text(temp_max, "");

    max_icon = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(95, 110), PBL_IF_ROUND_ELSE(18, -2), bounds.size.w, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);
    text_layer_set_font(max_icon, weather_font_big);
    text_layer_set_text(max_icon, "");

    steps_or_sleep = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148), bounds.size.w, 50));
    text_layer_set_background_color(steps_or_sleep, GColorClear);
    text_layer_set_text_alignment(steps_or_sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(steps_or_sleep, base_font);
    text_layer_set_text(steps_or_sleep, "");

    dist_or_deep = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148), bounds.size.w, 50));
    text_layer_set_background_color(dist_or_deep, GColorClear);
    text_layer_set_text_alignment(dist_or_deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(dist_or_deep, base_font);
    text_layer_set_text(dist_or_deep, "");
    
    set_colors();

    layer_add_child(window_layer, text_layer_get_layer(hours));
    layer_add_child(window_layer, text_layer_get_layer(date));
    layer_add_child(window_layer, text_layer_get_layer(alt_time));
    layer_add_child(window_layer, text_layer_get_layer(battery));
    layer_add_child(window_layer, text_layer_get_layer(weather));
    layer_add_child(window_layer, text_layer_get_layer(min_icon));
    layer_add_child(window_layer, text_layer_get_layer(max_icon));
    layer_add_child(window_layer, text_layer_get_layer(temp_cur));
    layer_add_child(window_layer, text_layer_get_layer(temp_min));
    layer_add_child(window_layer, text_layer_get_layer(temp_max));
    layer_add_child(window_layer, text_layer_get_layer(steps_or_sleep));
    layer_add_child(window_layer, text_layer_get_layer(dist_or_deep));

    tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
    tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    if (persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
    } else {
        tz_name[0] = '#';
    }
}

static void watchface_unload(Window *window) {
    text_layer_destroy(hours);
    text_layer_destroy(date);
    text_layer_destroy(alt_time);
    text_layer_destroy(battery);
    text_layer_destroy(weather);
    text_layer_destroy(min_icon);
    text_layer_destroy(max_icon);
    text_layer_destroy(temp_cur);
    text_layer_destroy(temp_min);
    text_layer_destroy(temp_max);
    text_layer_destroy(steps_or_sleep);
    text_layer_destroy(dist_or_deep);
    fonts_unload_custom_font(time_font);
    fonts_unload_custom_font(date_font);
    fonts_unload_custom_font(base_font);
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_font_big);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    bool is_sleeping = false;

    #if defined(PBL_HEALTH)
        if (health_enabled) {
            HealthActivityMask activities = health_service_peek_current_activities();
            is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
        
    
            bool sleep_data_enabled = persist_exists(KEY_SHOWSLEEP) && persist_read_int(KEY_SHOWSLEEP);
            
            if (!is_sleeping && was_asleep && sleep_data_enabled) {
                APP_LOG(APP_LOG_LEVEL_INFO, "We woke up!");
                sleep_data_visible = true;
                woke_up_at_hour = tick_time->tm_hour;
                woke_up_at_min = tick_time->tm_min;
                was_asleep = false;
                update_sleep_data();
            }

            if (sleep_data_visible && tick_time->tm_hour >= woke_up_at_hour + 1 && tick_time->tm_min >= woke_up_at_min) {
                APP_LOG(APP_LOG_LEVEL_INFO, "Past an hour after wake up!");
                sleep_data_visible = false;
            }
        }
    #endif

    uint16_t tick_interval = is_sleeping ? 59 : 30;

    if(tick_time->tm_min % tick_interval == 0 && weather_enabled) {
        update_weather();
    }
}


static void init(void) {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    was_asleep = false;
    sleep_data_visible = false;

    watchface = window_create();

    window_set_window_handlers(watchface, (WindowHandlers) {
        .load = watchface_load,
        .unload = watchface_unload,
    });

    battery_state_service_subscribe(battery_handler);

    window_stack_push(watchface, true);
    update_time();

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    toggle_health();
    toggle_weather();

    battery_handler(battery_state_service_peek());
}

static void deinit(void) {
    window_destroy(watchface);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
