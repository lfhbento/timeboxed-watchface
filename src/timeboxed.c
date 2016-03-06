#include <pebble.h>
#include <ctype.h>
#include <time.h>
#include "keys.h"
#include "locales.h"
#include "health.h"
#include "text.h"
#include "weather.h"
#include "configs.h"

static Window *watchface;

static signed int tz_hour;
static uint8_t tz_minute;
static char tz_name[TZ_LEN];
static uint8_t min_counter;
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
        if (is_leading_zero_disabled()) {
            if (hour_text[0] == '0') hour_text[0] = ' ';
            if (tz_text[0] == '0') tz_text[0] = ' ';
        }

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
        if (is_bluetooth_vibrate_enabled() && !is_user_sleeping()) {
            vibes_long_pulse();
        }
        set_bluetooth_color();
        set_bluetooth_layer_text("a");
    }
}

static void battery_handler(BatteryChargeState charge_state) {
    char s_battery_buffer[8];

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }
    set_battery_color(charge_state.charge_percent);
    set_battery_layer_text(s_battery_buffer);
}

static void load_screen(bool from_configs) {
    if (from_configs) {
        unload_face_fonts();
    }
    load_face_fonts();
    set_face_fonts();
    load_locale();
    update_time();
    set_colors(watchface);
    toggle_health(from_configs);
    toggle_weather(from_configs);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
}

static void check_for_updates() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Checking for updates. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, KEY_HASUPDATE, 1);
    app_message_outbox_send();
}

static void notify_update(int update_available) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Notifying user. (%d) %d%2d", update_available, (int)time(NULL), (int)time_ms(NULL, NULL));
    if (update_available) {
        set_update_color();
    }
    set_update_layer_text(update_available ? "f" : "");
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

        store_weather_values(temp_val, max_val, min_val, weather_val);

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather data updated. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        return;
    }

    int configs = 0;

    Tuple *enableHealth = dict_find(iterator, KEY_ENABLEHEALTH);
    if (enableHealth) {
        bool enabled = enableHealth->value->int8;
        if (enabled) configs += FLAG_HEALTH;
    }

    Tuple *useKm = dict_find(iterator, KEY_USEKM);
    if (useKm) {
        bool enabled = useKm->value->int8;
        if (enabled) configs += FLAG_KM;
    }

    Tuple *showSleep = dict_find(iterator, KEY_SHOWSLEEP);
    if (showSleep) {
        bool enabled = showSleep->value->int8;
        if (enabled) configs += FLAG_SLEEP;
    }

    Tuple *enableWeather = dict_find(iterator, KEY_ENABLEWEATHER);
    if (enableWeather) {
        bool enabled =  enableWeather->value->int8;
        if (enabled) configs += FLAG_WEATHER;
    }

    Tuple *useCelsius = dict_find(iterator, KEY_USECELSIUS);
    if (useCelsius) {
        bool enabled = useCelsius->value->int8;
        if (enabled)  configs += FLAG_CELSIUS;
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
        persist_write_int(KEY_BGCOLOR, bgColor->value->int32);
    }

    Tuple *hoursColor = dict_find(iterator, KEY_HOURSCOLOR);
    if (hoursColor) {
        persist_write_int(KEY_HOURSCOLOR, hoursColor->value->int32);
    }

    Tuple *enableAdvanced = dict_find(iterator, KEY_ENABLEADVANCED);
    if (enableAdvanced) {
        bool enabled = enableAdvanced->value->int8;
        if (enabled) configs += FLAG_ADVANCED;
    }

    Tuple *dateColor = dict_find(iterator, KEY_DATECOLOR);
    if (dateColor) {
        persist_write_int(KEY_DATECOLOR, dateColor->value->int32);
    }

    Tuple *altHoursColor = dict_find(iterator, KEY_ALTHOURSCOLOR);
    if (altHoursColor) {
        persist_write_int(KEY_ALTHOURSCOLOR, altHoursColor->value->int32);
    }

    Tuple *batteryColor = dict_find(iterator, KEY_BATTERYCOLOR);
    if (batteryColor) {
        persist_write_int(KEY_BATTERYCOLOR, batteryColor->value->int32);
    }

    Tuple *batteryLowColor = dict_find(iterator, KEY_BATTERYLOWCOLOR);
    if (batteryLowColor) {
        persist_write_int(KEY_BATTERYLOWCOLOR, batteryLowColor->value->int32);
    }

    Tuple *weatherColor = dict_find(iterator, KEY_WEATHERCOLOR);
    if (weatherColor) {
        persist_write_int(KEY_WEATHERCOLOR, weatherColor->value->int32);
    }

    Tuple *tempColor = dict_find(iterator, KEY_TEMPCOLOR);
    if (tempColor) {
        persist_write_int(KEY_TEMPCOLOR, tempColor->value->int32);
    }

    Tuple *minColor = dict_find(iterator, KEY_MINCOLOR);
    if (minColor) {
        persist_write_int(KEY_MINCOLOR, minColor->value->int32);
    }

    Tuple *maxColor = dict_find(iterator, KEY_MAXCOLOR);
    if (maxColor) {
        persist_write_int(KEY_MAXCOLOR, maxColor->value->int32);
    }

    Tuple *stepsColor = dict_find(iterator, KEY_STEPSCOLOR);
    if (stepsColor) {
        persist_write_int(KEY_STEPSCOLOR, stepsColor->value->int32);
    }

    Tuple *distColor = dict_find(iterator, KEY_DISTCOLOR);
    if (distColor) {
        persist_write_int(KEY_DISTCOLOR, distColor->value->int32);
    }

    Tuple *stepsBehindColor = dict_find(iterator, KEY_STEPSBEHINDCOLOR);
    if (stepsBehindColor) {
        persist_write_int(KEY_STEPSBEHINDCOLOR, stepsBehindColor->value->int32);
    }

    Tuple *distBehindColor = dict_find(iterator, KEY_DISTBEHINDCOLOR);
    if (distBehindColor) {
        persist_write_int(KEY_DISTBEHINDCOLOR, distBehindColor->value->int32);
    }

    Tuple *fontType = dict_find(iterator, KEY_FONTTYPE);
    if (fontType) {
        persist_write_int(KEY_FONTTYPE, fontType->value->int8);
    }

    Tuple *bluetoothDisconnect = dict_find(iterator, KEY_BLUETOOTHDISCONNECT);
    if (bluetoothDisconnect) {
        bool enabled = bluetoothDisconnect->value->int8;
        if(enabled) configs += FLAG_BLUETOOTH;
    }

    Tuple *bluetoothColor = dict_find(iterator, KEY_BLUETOOTHCOLOR);
    if (bluetoothColor) {
        persist_write_int(KEY_BLUETOOTHCOLOR, bluetoothColor->value->int32);
    }

    Tuple *overrideLocation = dict_find(iterator, KEY_OVERRIDELOCATION);
    if (overrideLocation) {
        persist_write_string(KEY_OVERRIDELOCATION, overrideLocation->value->cstring);
    }

    Tuple *updateAvailable = dict_find(iterator, KEY_UPDATE);
    if (updateAvailable) {
        bool enabled = updateAvailable->value->int8;
        if (!enabled) configs += FLAG_UPDATE;
    }

    Tuple *updateColor = dict_find(iterator, KEY_UPDATECOLOR);
    if (updateColor) {
        persist_write_int(KEY_UPDATECOLOR, updateColor->value->int32);
    }

    Tuple *locale = dict_find(iterator, KEY_LOCALE);
    if (locale) {
        persist_write_int(KEY_LOCALE, locale->value->int8);
    }

    Tuple *dateFormat = dict_find(iterator, KEY_DATEFORMAT);
    if (dateFormat) {
        persist_write_int(KEY_DATEFORMAT, dateFormat->value->int8);
    }

    Tuple *textAlign = dict_find(iterator, KEY_TEXTALIGN);
    if (textAlign) {
        persist_write_int(KEY_TEXTALIGN, textAlign->value->int8);
    }

    Tuple *leadingZero = dict_find(iterator, KEY_LEADINGZERO);
    if (leadingZero) {
        bool enabled = leadingZero->value->int8;
        if (!enabled) configs += FLAG_LEADINGZERO;
    }

    persist_write_int(KEY_CONFIGS, configs);
    set_config_toggles(configs);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Configs persisted. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));

    create_text_layers(window);

    min_counter = 0;

    if (persist_exists(KEY_TIMEZONESCODE)) {
        persist_read_string(KEY_TIMEZONESCODE, tz_name, sizeof(tz_name));
        tz_hour = persist_exists(KEY_TIMEZONES) ? persist_read_int(KEY_TIMEZONES) : 0;
        tz_minute = persist_exists(KEY_TIMEZONESMINUTES) ? persist_read_int(KEY_TIMEZONESMINUTES) : 0;
    } else {
        tz_name[0] = '#';
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Watchface load end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void watchface_unload(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));

    unload_face_fonts();

    destroy_text_layers();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Unload end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    min_counter++;

    bool is_sleeping = is_user_sleeping();

    if (!is_update_disabled() && tick_time->tm_hour == 4) { // updates at 4am
        check_for_updates();
    }
    if (is_update_disabled()) {
        notify_update(false);
    }

    uint8_t tick_interval = is_sleeping ? 90 : 30;

    show_sleep_data_if_visible();

    if(min_counter == tick_interval) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Time for updates (%d). %d%2d", tick_interval, (int)time(NULL), (int)time_ms(NULL, NULL));
        if (is_weather_enabled()) {
            update_weather();
        }
        if (is_sleeping) {
            queue_health_update();
        }
        min_counter = 0;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting health from time. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    get_health_data();
}


static void init(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    init_sleep_data();
    queue_health_update();

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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Init end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

static void deinit(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    window_destroy(watchface);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Deinit end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

int main(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    init();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App event loop start. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    app_event_loop();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App event loop end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    deinit();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App end. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}
