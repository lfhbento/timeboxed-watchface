#include <pebble.h>
#include <ctype.h>
#include <time.h>
#include "keys.h"
#include "locales.h"
#include "health.h"
#include "text.h"
#include "weather.h"

static Window *watchface;

static signed int tz_hour;
static uint8_t tz_minute;
static char tz_name[TZ_LEN];

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
            if (get_loaded_font() == BLOCKO_FONT || get_loaded_font() == BLOCKO_BIG_FONT) {
                tz_text[i] = tolower((unsigned char)tz_text[i]);
            } else {
                tz_text[i] = toupper((unsigned char)tz_text[i]);
            }
        }
        set_alt_time_layer_text(tz_text);
    } else {
        set_alt_time_layer_text("");
    }

    set_hours_layer_text(hour_text);
    get_current_date(tick_time, date_text, sizeof(date_text));
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
    load_locale();
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

    if (temp_tuple && max_tuple && min_tuple && weather_tuple && is_weather_enabled()) {
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

    Tuple *locale = dict_find(iterator, KEY_LOCALE);
    if (locale) {
        uint8_t locale_v = locale->value->int8;
        persist_write_int(KEY_LOCALE, locale_v);
    }

    Tuple *dateFormat = dict_find(iterator, KEY_DATEFORMAT);
    if (dateFormat) {
        uint8_t dateformat_v = dateFormat->value->int8;
        persist_write_int(KEY_DATEFORMAT, dateformat_v);
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

    if((tick_time->tm_min % tick_interval == 0) && is_weather_enabled()) {
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
