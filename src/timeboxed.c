#include <pebble.h>
#include <ctype.h>
#include <time.h>
#include "keys.h"
#include "locales.h"
#include "health.h"
#include "text.h"

static Window *watchface;

static bool weather_enabled;
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
    char tz_text[22];
    char hour_text[13];
    char date_text[13];

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
        set_alt_time_layer_text(tz_text);
    } else {
        set_alt_time_layer_text("");
    }


    for (unsigned char i = 0; date_text[i]; ++i) {
        date_text[i] = tolower((unsigned char)date_text[i]);
    }
    set_hours_layer_text(hour_text);
    set_date_layer_text(date_text);
}

void bt_handler(bool connected) {
    if (connected) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Phone is connected.");
        set_bluetooth_layer_text("");
    } else {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Phone is not connected.");
        bool is_sleeping = is_user_sleeping();
        bool bluetooth_disconnect_vibe = persist_exists(KEY_BLUETOOTHDISCONNECT) && persist_read_int(KEY_BLUETOOTHDISCONNECT);

        if (bluetooth_disconnect_vibe && !is_sleeping) {
            vibes_long_pulse();
        }
        set_bluetooth_layer_text("\U0000F294");
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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather with key (%s) %d", weather_key_buffer, (int)time(NULL));
    dict_write_uint8(iter, KEY_USECELSIUS, 
        persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS) ? persist_read_int(KEY_USECELSIUS) : 0);
    dict_write_cstring(iter, KEY_WEATHERKEY, weather_key_buffer);
    app_message_outbox_send();
}

static void update_weather_values(int temp_val, int max_val, int min_val, int weather_val) {
    bool useCelsius = persist_exists(KEY_USECELSIUS) && persist_read_int(KEY_USECELSIUS);
    char temp_pattern[4];
    char temp_text[8];
    char max_text[8];
    char min_text[8];
    char weather_text[4];

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

    set_temp_cur_layer_text(temp_text);
    set_temp_max_layer_text(max_text);
    set_temp_min_layer_text(min_text);
    set_weather_layer_text(weather_text);
    set_max_icon_layer_text("\U0000F058");
    set_min_icon_layer_text("\U0000F044");
}

static void toggle_weather(bool from_configs) {
    weather_enabled = persist_exists(KEY_ENABLEWEATHER) && persist_read_int(KEY_ENABLEWEATHER);
    if (weather_enabled) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather is enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        update_weather_values(0, 0, 0, 0);
        if (from_configs) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from configs. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather();
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather disabled, clearing up. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_temp_cur_layer_text("");
        set_temp_max_layer_text("");
        set_temp_min_layer_text("");
        set_weather_layer_text("");
        set_max_icon_layer_text("");
        set_min_icon_layer_text("");
    }
}

static void battery_handler(BatteryChargeState charge_state) {
    char s_battery_buffer[8];

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }

    set_battery_layer_text(s_battery_buffer);
}

static void load_screen(bool from_configs) {
    if (from_configs) {
        unload_face_fonts();
    }
    load_face_fonts();
    set_face_fonts();
    set_colors(watchface);
    update_time();
    toggle_health(from_configs);
    toggle_weather(from_configs);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
}

static void check_for_updates() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Checking for updates. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, KEY_HASUPDATE, 1); 
    app_message_outbox_send();
}

static void notify_update(int update_available) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Notifying user. (%d) %d%d", update_available, (int)time(NULL), (int)time_ms(NULL, NULL));
    set_update_layer_text(update_available ? "\U0000F102" : "");
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *error_tuple = dict_find(iterator, KEY_ERROR);

    if (error_tuple) {
        get_health_data();
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

        get_health_data();

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather data updated. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Configs persisted. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    destroy_text_layers();
    create_text_layers(watchface);
    load_screen(true);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! Reason code: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! Reason code: %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox send success!");
}

static void watchface_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));

    create_text_layers(window);

    if (persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
        tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
        tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    } else {
        tz_name[0] = '#';
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void watchface_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    
    unload_face_fonts();

    destroy_text_layers();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    bool is_sleeping = is_user_sleeping();

    show_sleep_data_if_visible();

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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    init_sleep_data();

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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void deinit(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    window_destroy(watchface);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

int main(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App start. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    init();
    app_event_loop();
    deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App end. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
}
