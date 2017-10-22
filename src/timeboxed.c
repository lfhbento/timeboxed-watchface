#include <pebble.h>
#include <time.h>
#include "keys.h"
#include "locales.h"
#include "health.h"
#include "text.h"
#include "weather.h"
#include "configs.h"
#include "positions.h"
#include "screen.h"
#include "clock.h"
#include "accel.h"
#include "compass.h"
#include "crypto.h"

static Window *watchface;

static uint32_t color_keys[] = {
    KEY_BGCOLOR,
    KEY_HOURSCOLOR,
    KEY_ALTHOURSCOLOR,
    KEY_DATECOLOR,
    KEY_BLUETOOTHCOLOR,
    KEY_UPDATECOLOR,
    KEY_BATTERYCOLOR,
    KEY_BATTERYLOWCOLOR,
    KEY_TEMPCOLOR,
    KEY_WEATHERCOLOR,
    KEY_MINCOLOR,
    KEY_MAXCOLOR,
    KEY_WINDDIRCOLOR,
    KEY_WINDSPEEDCOLOR,
    KEY_COMPASSCOLOR,
    KEY_SUNRISECOLOR,
    KEY_SUNSETCOLOR,
    KEY_SECONDSCOLOR,
    KEY_CRYPTOCOLOR,
    KEY_ALTHOURSBCOLOR,
    KEY_CRYPTOBCOLOR,
    KEY_CRYPTOCCOLOR,
    KEY_CRYPTODCOLOR,
};

#if !defined PBL_PLATFORM_APLITE
static uint8_t num_colors = 23;
#else
static uint8_t num_colors = 19;
#endif

static uint32_t config_keys[] = {
    KEY_WEATHER,
    KEY_SHOWSLEEP,
    KEY_USECELSIUS,
    KEY_ENABLEADVANCED,
    KEY_BLUETOOTHDISCONNECT,
    KEY_UPDATE,
    KEY_LEADINGZERO,
    KEY_SIMPLEMODE,
    KEY_QUICKVIEW,
    KEY_SHOWTAP,
    KEY_SHOWWRIST,
    KEY_MUTEONQUIET
};

static uint32_t string_config_keys[] = {
    KEY_OVERRIDELOCATION
};

static uint8_t num_string_configs = 1;

static uint32_t config_flags[] = {
    FLAG_WEATHER,
    FLAG_SLEEP,
    FLAG_CELSIUS,
    FLAG_ADVANCED,
    FLAG_BLUETOOTH,
    FLAG_UPDATE,
    FLAG_LEADINGZERO,
    FLAG_SIMPLEMODE,
    FLAG_QUICKVIEW,
    FLAG_TAP,
    FLAG_WRIST,
    FLAG_MUTEONQUIET
};
static bool config_defaults[] = {
    true,
    true,
    true,
    true,
    true,
    false,
    false,
    true,
    false,
    true,
    true,
    false
};
static uint8_t num_configs = 12;

static uint32_t int_keys[] = {
    KEY_FONTTYPE,
    KEY_LOCALE,
    KEY_DATEFORMAT,
    KEY_TEXTALIGN,
    KEY_SPEEDUNIT,
    KEY_WEATHERTIME,
    KEY_DATESEPARATOR,
    KEY_CRYPTOTIME,
};
static uint8_t num_int = 8;

static uint32_t slot_keys[] = {
    KEY_SLOTA,
    KEY_SLOTB,
    KEY_SLOTC,
    KEY_SLOTD,
    KEY_SLOTE,
    KEY_SLOTF,
};
static uint32_t slot_values[] = {
    SLOT_A,
    SLOT_B,
    SLOT_C,
    SLOT_D,
    SLOT_E,
    SLOT_F,
};
uint8_t num_slots = 6;

#if defined(PBL_HEALTH)
static int min_count = 0;
static int min_count_crypto = 0;
static uint8_t health_color_keys[] = {
    KEY_STEPSCOLOR,
    KEY_STEPSBEHINDCOLOR,
    KEY_DISTCOLOR,
    KEY_DISTBEHINDCOLOR,
    KEY_CALCOLOR,
    KEY_CALBEHINDCOLOR,
    KEY_SLEEPCOLOR,
    KEY_SLEEPBEHINDCOLOR,
    KEY_DEEPCOLOR,
    KEY_DEEPBEHINDCOLOR,
    KEY_ACTIVECOLOR,
    KEY_ACTIVEBEHINDCOLOR,
    KEY_HEARTCOLOR,
    KEY_HEARTCOLOROFF
};
static uint8_t num_health_colors = 14;
#endif

#if !defined PBL_PLATFORM_APLITE
static uint32_t slot_sleep_keys[] = {
    KEY_SLEEPSLOTA,
    KEY_SLEEPSLOTB,
    KEY_SLEEPSLOTC,
    KEY_SLEEPSLOTD,
    KEY_SLEEPSLOTE,
    KEY_SLEEPSLOTF,
};
static uint32_t slot_tap_keys[] = {
    KEY_TAPSLOTA,
    KEY_TAPSLOTB,
    KEY_TAPSLOTC,
    KEY_TAPSLOTD,
    KEY_TAPSLOTE,
    KEY_TAPSLOTF,
};
static uint32_t slot_wrist_keys[] = {
    KEY_WRISTSLOTA,
    KEY_WRISTSLOTB,
    KEY_WRISTSLOTC,
    KEY_WRISTSLOTD,
    KEY_WRISTSLOTE,
    KEY_WRISTSLOTF,
};
static int sec_count = 0;
static int timeout_sec = 0;
#endif

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple * error_tuple = dict_find(iterator, KEY_ERROR);

    if (error_tuple) {
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
    Tuple *speed_tuple = dict_find(iterator, KEY_SPEED);
    Tuple *direction_tuple = dict_find(iterator, KEY_DIRECTION);
    Tuple *sunrise_tuple = dict_find(iterator, KEY_SUNRISE);
    Tuple *sunset_tuple = dict_find(iterator, KEY_SUNSET);

    if (temp_tuple || max_tuple || speed_tuple || sunrise_tuple || sunset_tuple) {
        int temp_val = (int)temp_tuple->value->int32;
        int max_val = (int)max_tuple->value->int32;
        int min_val = (int)min_tuple->value->int32;
        int weather_val = (int)weather_tuple->value->int32;

        update_weather_values(temp_val, weather_val);
        update_forecast_values(max_val, min_val);

        int speed_val = (int)speed_tuple->value->int32;
        int direction_val = (int)direction_tuple->value->int32;

        int sunrise_val = (int)sunrise_tuple->value->int32;
        int sunset_val = (int)sunset_tuple->value->int32;

        update_wind_values(speed_val, direction_val);
        update_sunrise(sunrise_val);
        update_sunset(sunset_val);
        store_weather_values(temp_val, max_val, min_val, weather_val, speed_val, direction_val, sunrise_val, sunset_val);
        return;
    }

    Tuple *crypto_price = dict_find(iterator, KEY_CRYPTOPRICE);
    Tuple *crypto_price_b = dict_find(iterator, KEY_CRYPTOPRICEB);
    Tuple *crypto_price_c = dict_find(iterator, KEY_CRYPTOPRICEC);
    Tuple *crypto_price_d = dict_find(iterator, KEY_CRYPTOPRICED);

    if (crypto_price || crypto_price_b || crypto_price_c || crypto_price_d) {
        if (crypto_price) {
            static char crypto_val[8];
            char* value = crypto_price->value->cstring;
            strcpy(crypto_val, value);
            update_crypto_price(crypto_val);
            store_crypto_price(crypto_val);
        }

        #if !defined PBL_PLATFORM_APLITE
        if (crypto_price_b) {
            static char crypto_val[8];
            char* value = crypto_price_b->value->cstring;
            strcpy(crypto_val, value);
            update_crypto_price_b(crypto_val);
            store_crypto_price_b(crypto_val);
        }

        if (crypto_price_c) {
            static char crypto_val[8];
            char* value = crypto_price_c->value->cstring;
            strcpy(crypto_val, value);
            update_crypto_price_c(crypto_val);
            store_crypto_price_c(crypto_val);
        }

        if (crypto_price_d) {
            static char crypto_val[8];
            char* value = crypto_price_d->value->cstring;
            strcpy(crypto_val, value);
            update_crypto_price_d(crypto_val);
            store_crypto_price_d(crypto_val);
        }
        #endif
        return;
    }

    int configs = 0;
    signed int tz_hour = 0;
    uint8_t tz_minute = 0;
    static char tz_name[TZ_LEN];

    signed int tz_hour_b = 0;
    uint8_t tz_minute_b = 0;
    static char tz_name_b[TZ_LEN];

    Tuple *key_value = NULL;

    // Timezone A
    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONES);
    if (key_value) {
        persist_write_int(KEY_TIMEZONES, key_value->value->int8);
        tz_hour = key_value->value->int8;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONESMINUTES);
    if (key_value) {
        persist_write_int(KEY_TIMEZONESMINUTES, key_value->value->int8);
        tz_minute = key_value->value->int8;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONESCODE);
    if (key_value) {
        char* tz_code = key_value->value->cstring;
        persist_write_string(KEY_TIMEZONESCODE, tz_code);
        strcpy(tz_name, tz_code);
    }

    // Timezone B
    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONESB);
    if (key_value) {
        persist_write_int(KEY_TIMEZONESB, key_value->value->int8);
        tz_hour_b = key_value->value->int8;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONESBMINUTES);
    if (key_value) {
        persist_write_int(KEY_TIMEZONESBMINUTES, key_value->value->int8);
        tz_minute_b = key_value->value->int8;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TIMEZONESBCODE);
    if (key_value) {
        char* tz_code = key_value->value->cstring;
        persist_write_string(KEY_TIMEZONESBCODE, tz_code);
        strcpy(tz_name_b, tz_code);
    }

    // configs
    for (int i = 0; i < num_configs; ++i) {
        key_value = NULL; key_value = dict_find(iterator, config_keys[i]);
        if (key_value) {
            if (!!key_value->value->int8 == config_defaults[i]) configs += config_flags[i];
        }
    }

    // options
    for (int i = 0; i < num_int; ++i) {
        key_value = NULL; key_value = dict_find(iterator, int_keys[i]);
        if (key_value) {
            persist_write_int(int_keys[i], key_value->value->int8);
        }
    }

    // colors
    for (int i = 0; i < num_colors; ++i) {
        key_value = NULL; key_value = dict_find(iterator, color_keys[i]);
        if (key_value) {
            persist_write_int(color_keys[i], key_value->value->int32);
        }
    }

    #if defined(PBL_HEALTH)
    // health colors
    for (int i = 0; i < num_health_colors; ++i) {
        key_value = NULL; key_value = dict_find(iterator, health_color_keys[i]);
        if (key_value) {
            persist_write_int(health_color_keys[i], key_value->value->int32);
        }
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_HEARTLOW);
    if (key_value) {
        persist_write_int(KEY_HEARTLOW, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_HEARTHIGH);
    if (key_value) {
        persist_write_int(KEY_HEARTHIGH, key_value->value->int32);
    }
    #endif

    // string configs
    for (int i = 0; i < num_string_configs; ++i) {
        key_value = NULL; key_value = dict_find(iterator, string_config_keys[i]);
        if (key_value) {
            persist_write_string(string_config_keys[i], key_value->value->cstring);
        }
    }

    // slots
    for (int i = 0; i < num_slots; ++i) {
        key_value = NULL; key_value = dict_find(iterator, slot_keys[i]);
        if (key_value) {
            set_module(slot_values[i], key_value->value->int8, STATE_NORMAL);
            persist_write_int(slot_keys[i], key_value->value->int8);
        }
    }

    #if !defined PBL_PLATFORM_APLITE
    // sleep slots
    for (int i = 0; i < num_slots; ++i) {
        key_value = NULL; key_value = dict_find(iterator, slot_sleep_keys[i]);
        if (key_value) {
            set_module(slot_values[i], key_value->value->int8, STATE_SLEEP);
            persist_write_int(slot_sleep_keys[i], key_value->value->int8);
        }
    }

    // tap slots
    for (int i = 0; i < num_slots; ++i) {
        key_value = NULL; key_value = dict_find(iterator, slot_tap_keys[i]);
        if (key_value) {
            set_module(slot_values[i], key_value->value->int8, STATE_TAP);
            persist_write_int(slot_tap_keys[i], key_value->value->int8);
        }
    }

    // wrist slots
    for (int i = 0; i < num_slots; ++i) {
        key_value = NULL; key_value = dict_find(iterator, slot_wrist_keys[i]);
        if (key_value) {
            set_module(slot_values[i], key_value->value->int8, STATE_WRIST);
            persist_write_int(slot_wrist_keys[i], key_value->value->int8);
        }
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TAPTIME);
    if (key_value) {
        timeout_sec = key_value->value->int8;
        persist_write_int(KEY_TAPTIME, key_value->value->int8);
    }
    #endif

    persist_write_int(KEY_CONFIGS, configs);
    set_config_toggles(configs);
    set_timezone(tz_name, tz_hour, tz_minute);
    set_timezone_b(tz_name_b, tz_hour_b, tz_minute_b);

    #if !defined PBL_PLATFORM_APLITE
    init_accel_service(watchface);
    #endif
    #if defined PBL_COMPASS
    init_compass_service(watchface);
    #endif
    reload_fonts();
    recreate_text_layers(watchface);
    load_screen(RELOAD_CONFIGS, watchface);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
}

#if !defined PBL_PLATFORM_APLITE && !defined PBL_PLATFORM_CHALK
static void unobstructed_area_handle_changes() {
    Layer *window_layer = window_get_root_layer(watchface);
    GRect full_bounds = layer_get_bounds(window_layer);
    GRect bounds = layer_get_unobstructed_bounds(window_layer);
    toggle_center_slots(bounds.size.h == full_bounds.size.h);
    recreate_text_layers(watchface);
    load_screen(RELOAD_REDRAW, watchface);
}

static void unobstructed_area_did_change(void * context) {
    if (!is_quickview_disabled()) {
        unobstructed_area_handle_changes();
    }
}
#endif

static void watchface_load(Window *window) {
    create_text_layers(window);
    load_face_fonts();
    set_face_fonts();

    load_timezone_from_storage();
    #if !defined PBL_PLATFORM_APLITE
    timeout_sec = persist_exists(KEY_TAPTIME) ? persist_read_int(KEY_TAPTIME) : 7;
    #endif
}

static void watchface_unload(Window *window) {
    #if defined(PBL_HEALTH)
    save_health_data_to_storage();
    #endif

    unload_face_fonts();

    destroy_text_layers();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    if (is_module_enabled(MODULE_SECONDS)) {
        update_seconds(tick_time);
    }

    #if !defined PBL_PLATFORM_APLITE
    if (tap_mode_visible() || wrist_mode_visible()) {
        sec_count++;
        if (sec_count > timeout_sec) {
            sec_count = 0;
            if (tap_mode_visible()) {
                reset_tap_handler();
            }
            if (wrist_mode_visible()) {
                reset_wrist_handler();
            }
        }
    }
    #endif

    if (units_changed & MINUTE_UNIT) {
        if (is_weather_enabled()) {
            #if defined(PBL_HEALTH)
                if (is_user_sleeping()) {
                    min_count++;
                    if (min_count > 90) {
                        update_weather(false);
                        min_count = 0;
                    }
                } else {
                    update_weather(false);
                }
            #else
                update_weather(false);
            #endif
        }

        if (is_crypto_enabled()) {
            #if defined(PBL_HEALTH)
                if (is_user_sleeping()) {
                    min_count_crypto++;
                    if (min_count_crypto > 90) {
                        update_crypto(false);;
                        min_count_crypto = 0;
                    }
                } else {
                    update_crypto(false);;
                }
            #else
                update_crypto(false);;
            #endif
        }

        update_time();

        if (!is_update_disabled() && tick_time->tm_hour == 4 && tick_time->tm_min == 0) { // updates at 4:00am
            check_for_updates();
        }

        #if defined(PBL_HEALTH)
        if (!tap_mode_visible() && !wrist_mode_visible()) {
            show_sleep_data_if_visible(watchface);
        }

        if (
            ((tick_time->tm_min % 2 == 0 || is_module_enabled(MODULE_HEART)) && !is_user_sleeping()) || // check for health updates only every 2 minutes (or 1 min if heart rate is enabled)
            (is_user_sleeping() && tick_time->tm_min == 0)
        ) {
            get_health_data();
        }
        #endif
    }
}

static void init(void) {
    tick_timer_service_subscribe(SECOND_UNIT | MINUTE_UNIT, tick_handler);

    #if defined(PBL_HEALTH)
    init_sleep_data();
    queue_health_update();
    #endif

    watchface = window_create();

    window_set_window_handlers(watchface, (WindowHandlers) {
        .load = watchface_load,
        .unload = watchface_unload,
    });

    #if !defined PBL_PLATFORM_APLITE
    init_accel_service(watchface);
    #endif

    #if defined PBL_COMPASS
    init_compass_service(watchface);
    #endif

    battery_state_service_subscribe(battery_handler);

    window_stack_push(watchface, true);

    #if !defined PBL_PLATFORM_APLITE && !defined PBL_PLATFORM_CHALK
    UnobstructedAreaHandlers unobstructed_handlers = {
        .did_change = unobstructed_area_did_change,
    };

    unobstructed_area_service_subscribe(unobstructed_handlers, NULL);
    #endif

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    #if !defined PBL_PLATFORM_APLITE
    app_message_open(1024, 64);
    #else
    app_message_open(512, 64);
    #endif

    connection_service_subscribe((ConnectionHandlers) {
	.pebble_app_connection_handler = bt_handler
    });

    load_screen(RELOAD_DEFAULT, watchface);
    notify_update(false);
}

static void deinit(void) {
    window_destroy(watchface);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
