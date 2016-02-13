#include <pebble.h>
#include <ctype.h>
#include <time.h>

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
#define KEY_FONTTYPE 26
#define KEY_STEPS 27
#define KEY_DIST 28
#define KEY_BLUETOOTHDISCONNECT 29
#define KEY_BLUETOOTHCOLOR 30
#define KEY_OVERRIDELOCATION 31
#define KEY_ERROR 32
#define KEY_UPDATE 33
#define KEY_UPDATECOLOR 34
#define KEY_HASUPDATE 35

#define BLOCKO_FONT 0
#define BLOCKO_BIG_FONT 1
#define SYSTEM_FONT 2
#define ARCHIVO_FONT 3

static Window *watchface;
static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *bluetooth;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps_or_sleep;
static TextLayer *dist_or_deep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static TextLayer *update;
static GFont time_font;
static GFont medium_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_big_font;
static GFont awesome_font;

static char hour_text[13];
static char date_text[13];
static char temp_text[8];
static char max_text[8];
static char min_text[8];
static char weather_text[4];
static char tz_text[22];
static char s_battery_buffer[7];

#if defined(PBL_HEALTH)
static char steps_or_sleep_text[16];
static char dist_or_deep_text[16];
static uint8_t woke_up_at_hour;
static uint8_t woke_up_at_min;
#endif

static bool weather_enabled;
static bool health_enabled;
static bool sleep_data_visible;
static bool was_asleep;
static uint8_t loaded_font;
static signed int tz_hour;
static uint8_t tz_minute;
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
    "\U0000F003", // 'fog': 38,
    "\U0000F04A", // 'nt_fog': 39,
};


static void update_time() {
    // Get a tm structure
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
        strftime(tz_text, sizeof(tz_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M%p"), gmt_time);

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
            if (loaded_font == BLOCKO_FONT || loaded_font == BLOCKO_BIG_FONT) {
                tz_text[i] = tolower((unsigned char)tz_text[i]);
            } else {
                tz_text[i] = toupper((unsigned char)tz_text[i]);
            }
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

static void load_face_fonts() {
    int selected_font = BLOCKO_FONT;
    
    if (persist_exists(KEY_FONTTYPE)) {
        selected_font = persist_read_int(KEY_FONTTYPE);
    }

    if (selected_font == SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Loading system fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        medium_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
        base_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = SYSTEM_FONT;
    } else if (selected_font == ARCHIVO_FONT) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Loading Archivo font. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_28));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_18));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = ARCHIVO_FONT;
    } else if (selected_font == BLOCKO_BIG_FONT) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Loading Blocko font (big). %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_64));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_32));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_19));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = BLOCKO_BIG_FONT;
    } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Loading Blocko font. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = BLOCKO_FONT;
    }

}

static void unload_face_fonts() {
    if (loaded_font != SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Unloading custom fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        fonts_unload_custom_font(time_font);
        fonts_unload_custom_font(medium_font);
        fonts_unload_custom_font(base_font);
    }
}

static void set_face_fonts() {
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    text_layer_set_font(hours, time_font);
    text_layer_set_font(date, medium_font);
    text_layer_set_font(alt_time, base_font);
    text_layer_set_font(battery, base_font);
    text_layer_set_font(bluetooth, awesome_font);
    text_layer_set_font(update, awesome_font);
    text_layer_set_font(weather, weather_font);
    text_layer_set_font(min_icon, weather_big_font);
    text_layer_set_font(max_icon, weather_big_font);
    text_layer_set_font(temp_cur, base_font);
    text_layer_set_font(temp_min, base_font);
    text_layer_set_font(temp_max, base_font);
    text_layer_set_font(steps_or_sleep, base_font);
    text_layer_set_font(dist_or_deep, base_font);
}

static void set_colors(void) {
    GColor base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    bool enableAdvanced = persist_exists(KEY_ENABLEADVANCED) ? persist_read_int(KEY_ENABLEADVANCED) : false;
    APP_LOG(APP_LOG_LEVEL_INFO, "Advanced colors %d", enableAdvanced);
    text_layer_set_text_color(date, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);
    text_layer_set_text_color(alt_time, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_ALTHOURSCOLOR)) : base_color);
    text_layer_set_text_color(weather, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_WEATHERCOLOR)) : base_color);
    text_layer_set_text_color(temp_cur, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_TEMPCOLOR)) : base_color);
    text_layer_set_text_color(temp_min, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(min_icon, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(temp_max, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(max_icon, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(steps_or_sleep, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_STEPSCOLOR)) : base_color);
    text_layer_set_text_color(dist_or_deep, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color);

    BatteryChargeState charge_state = battery_state_service_peek();
    if (charge_state.charge_percent > 20) {
        text_layer_set_text_color(battery, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color);
    } else {
        text_layer_set_text_color(battery, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color);
    }
    
    text_layer_set_text_color(bluetooth, 
        enableAdvanced && persist_exists(KEY_BLUETOOTHCOLOR) ? GColorFromHEX(persist_read_int(KEY_BLUETOOTHCOLOR)) : base_color);

    text_layer_set_text_color(update, 
        enableAdvanced && persist_exists(KEY_UPDATECOLOR) ? GColorFromHEX(persist_read_int(KEY_UPDATECOLOR)) : base_color);

    window_set_background_color(watchface, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);
    APP_LOG(APP_LOG_LEVEL_INFO, "Defined colors. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void bt_handler(bool connected) {
    if (connected) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected.");
        text_layer_set_text(bluetooth, "");
    } else {
	APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected.");
        bool is_sleeping = false;
        bool bluetooth_disconnect_vibe = persist_exists(KEY_BLUETOOTHDISCONNECT) && persist_read_int(KEY_BLUETOOTHDISCONNECT);

        #if defined(PBL_HEALTH)
        HealthActivityMask activities = health_service_peek_current_activities();
        is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
        #endif

        if (bluetooth_disconnect_vibe && !is_sleeping) {
            vibes_long_pulse();
        }
        text_layer_set_text(bluetooth, "\U0000F294");
    }
}

#if defined(PBL_HEALTH)
static void update_steps_data(void) {

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    uint16_t current_steps = 0;
    uint16_t current_dist = 0;
    uint16_t current_dist_int = 0;
    uint16_t current_dist_dec = 0;
    

    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);
    
    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%d", current_steps);

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

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), (useKm ? "%d.%dkm" : "%d.%dmi"), current_dist_int, current_dist_dec);
        
        text_layer_set_text(dist_or_deep, dist_or_deep_text);
    }
    
    HealthActivityMask activities = health_service_peek_current_activities();
    bool is_sleeping = activities & HealthActivityRestfulSleep || activities & HealthActivityRestfulSleep;
    if (is_sleeping) {
        APP_LOG(APP_LOG_LEVEL_INFO, "We are asleep. %d", was_asleep);
        if (!was_asleep) {
            was_asleep = true;
            APP_LOG(APP_LOG_LEVEL_INFO, "Just went to sleep. %d", was_asleep);
        }
    }
    persist_write_string(KEY_STEPS, steps_or_sleep_text);
    persist_write_string(KEY_DIST, dist_or_deep_text);
}

static void update_sleep_data(void) {
    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;
    
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    uint16_t current_sleep = 0;
    uint16_t current_deep = 0;
    
    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum(metric_sleep, start, end);
        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = (current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%dh%dm", current_sleep_hours, current_sleep_minutes);
        
        text_layer_set_text(steps_or_sleep, steps_or_sleep_text);
    }

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum(metric_deep, start, end);
        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = (current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), "%dh%dm", current_deep_hours, current_deep_minutes);
        
        text_layer_set_text(dist_or_deep, dist_or_deep_text);
    }

}

static void get_health_data(void) {
    if (!sleep_data_visible) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Updating steps data. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        update_steps_data();
    } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Updating sleep data. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        update_sleep_data();
    }
}

static void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
        case HealthEventMovementUpdate:
        case HealthEventSleepUpdate:
            get_health_data();
            break;
    }
}
#endif

static void toggle_health(bool from_configs) {
    bool has_health = false;
    health_enabled = false;
    #if defined(PBL_HEALTH)
        health_enabled = persist_read_int(KEY_ENABLEHEALTH);
        if (health_enabled) {
            APP_LOG(APP_LOG_LEVEL_INFO, "Health enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            has_health = health_service_events_subscribe(health_handler, NULL);
            text_layer_set_text(steps_or_sleep, "0");
            text_layer_set_text(dist_or_deep, "0");
        } else {
            has_health = false;
            health_service_events_unsubscribe();
        }
    #endif

    if (!health_enabled || !has_health) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Health disabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        text_layer_set_text(steps_or_sleep, "");
        text_layer_set_text(dist_or_deep, "");
    }
}

static void update_weather(void) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    char weather_key_buffer[20];
    if (persist_exists(KEY_WEATHERKEY)) {
        persist_read_string(KEY_WEATHERKEY, weather_key_buffer, sizeof(weather_key_buffer));
    } else {
        weather_key_buffer[0] = '\0';
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "Requesting weather with key (%s) %d", weather_key_buffer, (int)time(NULL));
    dict_write_uint8(iter, KEY_USECELSIUS, 
        persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS) ? persist_read_int(KEY_USECELSIUS) : 0);
    dict_write_cstring(iter, KEY_WEATHERKEY, weather_key_buffer);
    app_message_outbox_send();
}

static void update_weather_values(int temp_val, int max_val, int min_val, int weather_val) {
    bool useCelsius = persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS);
    char temp_pattern[4];
    if (loaded_font == BLOCKO_BIG_FONT) {
        strcpy(temp_pattern, useCelsius ? "%dc" : "%df");
    } else if (loaded_font == BLOCKO_FONT) {
        strcpy(temp_pattern, useCelsius ? "%dc" : "%df");
    } else {
        strcpy(temp_pattern, useCelsius ? "%dC" : "%dF");
    }
        
    snprintf(temp_text, sizeof(temp_text), temp_pattern, temp_val);
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

static void toggle_weather(bool from_configs) {
    weather_enabled = persist_exists(KEY_ENABLEWEATHER) && persist_read_int(KEY_ENABLEWEATHER);
    if (weather_enabled) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather is enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        update_weather_values(0, 0, 0, 0);
        if (from_configs) {
            APP_LOG(APP_LOG_LEVEL_INFO, "Updating weather from configs. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather();
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather disabled, clearing up. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        text_layer_set_text(temp_cur, "");
        text_layer_set_text(temp_max, "");
        text_layer_set_text(temp_min, "");
        text_layer_set_text(weather, "");
        text_layer_set_text(min_icon, "");
        text_layer_set_text(max_icon, "");
    }
}

static void battery_handler(BatteryChargeState charge_state) {

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }

    text_layer_set_text(battery, s_battery_buffer);
}

static void load_screen(bool from_configs) {
    if (from_configs) {
        unload_face_fonts();
    }
    load_face_fonts();
    set_face_fonts();
    set_colors();
    update_time();
    toggle_health(from_configs);
    toggle_weather(from_configs);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
}

static void create_text_layers() {
    APP_LOG(APP_LOG_LEVEL_INFO, "Creating text layers. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    Layer *window_layer = window_get_root_layer(watchface);
    GRect bounds = layer_get_bounds(window_layer);
    int selected_font = BLOCKO_FONT;
    
    if (persist_exists(KEY_FONTTYPE)) {
        selected_font = persist_read_int(KEY_FONTTYPE);
    }
    int width = bounds.size.w;

    int hours_top;
    int date_left;
    int date_top;
    int alt_top;
    int battery_top;
    int bt_top;
    int temp_cur_top;
    int temp_min_max_top;
    int temp_icon_min_max_top;
    int update_top;

    if (selected_font == BLOCKO_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(46, 38);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(98, 90);
        alt_top = PBL_IF_ROUND_ELSE(46, 38);
        battery_top = PBL_IF_ROUND_ELSE(120, 112);
        bt_top = PBL_IF_ROUND_ELSE(70, 60);
        update_top = PBL_IF_ROUND_ELSE(88, 78);
        temp_cur_top = PBL_IF_ROUND_ELSE(4, 4);
        temp_min_max_top = PBL_IF_ROUND_ELSE(22, 4);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(20, 0);
    } else if (selected_font == BLOCKO_BIG_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(40, 32);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(96, 88);
        alt_top = PBL_IF_ROUND_ELSE(42, 34);
        battery_top = PBL_IF_ROUND_ELSE(124, 116);
        bt_top = PBL_IF_ROUND_ELSE(64, 54);
        update_top = PBL_IF_ROUND_ELSE(86, 76);
        temp_cur_top = PBL_IF_ROUND_ELSE(4, 3);
        temp_min_max_top = PBL_IF_ROUND_ELSE(22, 3);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    } else if (selected_font == ARCHIVO_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(50, 40);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(102, 92);
        alt_top = PBL_IF_ROUND_ELSE(44, 34);
        battery_top = PBL_IF_ROUND_ELSE(128, 118);
        bt_top = PBL_IF_ROUND_ELSE(68, 56);
        update_top = PBL_IF_ROUND_ELSE(86, 74);
        temp_cur_top = PBL_IF_ROUND_ELSE(2, 2);
        temp_min_max_top = PBL_IF_ROUND_ELSE(24, 2);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    } else {
        hours_top = PBL_IF_ROUND_ELSE(54, 42);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(98, 86);
        alt_top = PBL_IF_ROUND_ELSE(46, 34);
        battery_top = PBL_IF_ROUND_ELSE(124, 112);
        bt_top = PBL_IF_ROUND_ELSE(68, 56);
        update_top = PBL_IF_ROUND_ELSE(86, 74);
        temp_cur_top = PBL_IF_ROUND_ELSE(2, 2);
        temp_min_max_top = PBL_IF_ROUND_ELSE(20, 2);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    }

    hours = text_layer_create(GRect(0, hours_top, width, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_alignment(hours, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));

    date = text_layer_create(GRect(date_left, date_top, width, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_alignment(date, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
    alt_time = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -2), alt_top, width, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));

    battery = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -4), battery_top, width, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
    bluetooth = text_layer_create(GRect(PBL_IF_ROUND_ELSE(2, 0), bt_top, width, 50));
    text_layer_set_background_color(bluetooth, GColorClear);
    text_layer_set_text_alignment(bluetooth, GTextAlignmentLeft);

    update = text_layer_create(GRect(PBL_IF_ROUND_ELSE(2, 0), update_top, width, 50));
    text_layer_set_background_color(update, GColorClear);
    text_layer_set_text_alignment(update, GTextAlignmentLeft);

    weather = text_layer_create(GRect(PBL_IF_ROUND_ELSE(-14, 4), 0, width, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_cur = text_layer_create(GRect(PBL_IF_ROUND_ELSE(16, 38), temp_cur_top, width, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_min = text_layer_create(GRect(PBL_IF_ROUND_ELSE(70, 80), temp_min_max_top, width, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);

    min_icon = text_layer_create(GRect(PBL_IF_ROUND_ELSE(60, 70), temp_icon_min_max_top, width, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);

    temp_max = text_layer_create(GRect(PBL_IF_ROUND_ELSE(105, 113), temp_min_max_top, width, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);

    max_icon = text_layer_create(GRect(PBL_IF_ROUND_ELSE(95, 103), temp_icon_min_max_top, width, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);

    steps_or_sleep = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148), width, 50));
    text_layer_set_background_color(steps_or_sleep, GColorClear);
    text_layer_set_text_alignment(steps_or_sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    dist_or_deep = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148), width, 50));
    text_layer_set_background_color(dist_or_deep, GColorClear);
    text_layer_set_text_alignment(dist_or_deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
    layer_add_child(window_layer, text_layer_get_layer(hours));
    layer_add_child(window_layer, text_layer_get_layer(date));
    layer_add_child(window_layer, text_layer_get_layer(alt_time));
    layer_add_child(window_layer, text_layer_get_layer(battery));
    layer_add_child(window_layer, text_layer_get_layer(bluetooth));
    layer_add_child(window_layer, text_layer_get_layer(update));
    layer_add_child(window_layer, text_layer_get_layer(weather));
    layer_add_child(window_layer, text_layer_get_layer(min_icon));
    layer_add_child(window_layer, text_layer_get_layer(max_icon));
    layer_add_child(window_layer, text_layer_get_layer(temp_cur));
    layer_add_child(window_layer, text_layer_get_layer(temp_min));
    layer_add_child(window_layer, text_layer_get_layer(temp_max));
    layer_add_child(window_layer, text_layer_get_layer(steps_or_sleep));
    layer_add_child(window_layer, text_layer_get_layer(dist_or_deep));
    
}

static void destroy_text_layers() {
    APP_LOG(APP_LOG_LEVEL_INFO, "Destroying text layers. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    text_layer_destroy(hours);
    text_layer_destroy(date);
    text_layer_destroy(alt_time);
    text_layer_destroy(battery);
    text_layer_destroy(bluetooth);
    text_layer_destroy(update);
    text_layer_destroy(weather);
    text_layer_destroy(min_icon);
    text_layer_destroy(max_icon);
    text_layer_destroy(temp_cur);
    text_layer_destroy(temp_min);
    text_layer_destroy(temp_max);
    text_layer_destroy(steps_or_sleep);
    text_layer_destroy(dist_or_deep);
}

static void check_for_updates() {
    APP_LOG(APP_LOG_LEVEL_INFO, "Checking for updates. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, KEY_HASUPDATE, 1); 
    app_message_outbox_send();
}

static void notify_update(int update_available) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Notifying user. (%d) %d%d", update_available, (int)time(NULL), (int)time_ms(NULL, NULL));
    text_layer_set_text(update, update_available ? "\U0000F102" : "");
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *error_tuple = dict_find(iterator, KEY_ERROR);

    if (error_tuple) {
        #if defined(PBL_HEALTH)
        get_health_data();
        #endif
        return;
    }

    Tuple *update_tuple = dict_find(iterator, KEY_HASUPDATE);
    if (update_tuple) {
        int update_val = update_tuple->value->int8;
        persist_write_int(KEY_HASUPDATE, update_val);
        notify_update(update_val);
        return;
    }

    Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
    Tuple *max_tuple = dict_find(iterator, KEY_MAX);
    Tuple *min_tuple = dict_find(iterator, KEY_MIN);
    Tuple *weather_tuple = dict_find(iterator, KEY_WEATHER);

    if (temp_tuple && max_tuple && min_tuple && weather_tuple && weather_enabled) {
        int temp_val = (int)temp_tuple->value->int32;
        int max_val = (int)max_tuple->value->int32;
        int min_val = (int)min_tuple->value->int32;
        int weather_val = (int)weather_tuple->value->int32;

        update_weather_values(temp_val, max_val, min_val, weather_val);

        #if defined(PBL_HEALTH)
        get_health_data();
        #endif

        APP_LOG(APP_LOG_LEVEL_INFO, "Weather data updated. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
        persist_write_int(KEY_SHOWSLEEP, false);
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
        signed int tz = timezones->value->int8;
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

    Tuple *fontType = dict_find(iterator, KEY_FONTTYPE);
    if (fontType) {
        uint8_t font = fontType->value->int8;
        persist_write_int(KEY_FONTTYPE, font);
    }

    Tuple *bluetoothDisconnect = dict_find(iterator, KEY_BLUETOOTHDISCONNECT);
    if (bluetoothDisconnect) {
        uint8_t btd = bluetoothDisconnect->value->int8;
        persist_write_int(KEY_BLUETOOTHDISCONNECT, btd);
    }

    Tuple *bluetoothColor = dict_find(iterator, KEY_BLUETOOTHCOLOR);
    if (bluetoothColor) {
        uint32_t bluetooth_c = bluetoothColor->value->int32;
        persist_write_int(KEY_BLUETOOTHCOLOR, bluetooth_c);
    }

    Tuple *overrideLocation = dict_find(iterator, KEY_OVERRIDELOCATION);
    if (overrideLocation) {
        char* loc = overrideLocation->value->cstring;
        persist_write_string(KEY_OVERRIDELOCATION, loc);
    }

    Tuple *updateAvailable = dict_find(iterator, KEY_UPDATE);
    if (updateAvailable) {
        uint32_t update_a = updateAvailable->value->int32;
        persist_write_int(KEY_UPDATE, update_a);
    }

    Tuple *updateColor = dict_find(iterator, KEY_UPDATECOLOR);
    if (updateColor) {
        uint32_t update_c = updateColor->value->int32;
        persist_write_int(KEY_UPDATECOLOR, update_c);
    }

    APP_LOG(APP_LOG_LEVEL_INFO, "Configs persisted. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    destroy_text_layers();
    create_text_layers();
    load_screen(true);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! Reason code: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason code: %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void watchface_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Watchface load start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
    awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AWESOME_18));

    create_text_layers();

    if (persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
        tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
        tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    } else {
        tz_name[0] = '#';
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Watchface load end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void watchface_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Unload start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_big_font);
    fonts_unload_custom_font(awesome_font);

    unload_face_fonts();

    destroy_text_layers();

    APP_LOG(APP_LOG_LEVEL_INFO, "Unload end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
                get_health_data();
            }

            if (sleep_data_visible && tick_time->tm_hour >= (woke_up_at_hour + 1) % 24 && tick_time->tm_min >= woke_up_at_min) {
                APP_LOG(APP_LOG_LEVEL_INFO, "Past an hour after wake up!");
                sleep_data_visible = false;
            }
        }
    #endif

    bool update_enabled = persist_exists(KEY_UPDATE) ? persist_read_int(KEY_UPDATE) : true;

    if (update_enabled && tick_time->tm_hour == 4) { // updates at 4am
        check_for_updates();
    }
    if (!update_enabled) {
        notify_update(false);
    }

    uint8_t tick_interval = is_sleeping ? 60 : 30;

    if((tick_time->tm_min % tick_interval == 0) && weather_enabled) {
        update_weather();
    }
}


static void init(void) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Init start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(384, 64);

    connection_service_subscribe((ConnectionHandlers) {
	.pebble_app_connection_handler = bt_handler
    });

    load_screen(false);

    APP_LOG(APP_LOG_LEVEL_INFO, "Init end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void deinit(void) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Deinit start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    window_destroy(watchface);
    APP_LOG(APP_LOG_LEVEL_INFO, "Deinit end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

int main(void) {
    APP_LOG(APP_LOG_LEVEL_INFO, "App start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    init();
    app_event_loop();
    deinit();
    APP_LOG(APP_LOG_LEVEL_INFO, "App end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}
