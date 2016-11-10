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

static Window *watchface;
static int timeout_sec = 0;

#if defined(PBL_HEALTH)
static int min_count = 0;
#endif

#if !defined PBL_PLATFORM_APLITE
static int sec_count = 0;
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

    int configs = 0;
    signed int tz_hour = 0;
    uint8_t tz_minute = 0;
    static char tz_name[TZ_LEN];

    Tuple *key_value = NULL;
    key_value = dict_find(iterator, KEY_SHOWSLEEP);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_SLEEP;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SHOWTAP);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_TAP;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SHOWWRIST);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_WRIST;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_USECELSIUS);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_CELSIUS;
    }

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
        if (tz_code[0] != '#') configs += FLAG_TIMEZONES;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_BGCOLOR);
    if (key_value) {
        persist_write_int(KEY_BGCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_HOURSCOLOR);
    if (key_value) {
        persist_write_int(KEY_HOURSCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_ENABLEADVANCED);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_ADVANCED;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DATECOLOR);
    if (key_value) {
        persist_write_int(KEY_DATECOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_ALTHOURSCOLOR);
    if (key_value) {
        persist_write_int(KEY_ALTHOURSCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_BATTERYCOLOR);
    if (key_value) {
        persist_write_int(KEY_BATTERYCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_BATTERYLOWCOLOR);
    if (key_value) {
        persist_write_int(KEY_BATTERYLOWCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_WEATHERCOLOR);
    if (key_value) {
        persist_write_int(KEY_WEATHERCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TEMPCOLOR);
    if (key_value) {
        persist_write_int(KEY_TEMPCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_MINCOLOR);
    if (key_value) {
        persist_write_int(KEY_MINCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_MAXCOLOR);
    if (key_value) {
        persist_write_int(KEY_MAXCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_WINDDIRCOLOR);
    if (key_value) {
        persist_write_int(KEY_WINDDIRCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_WINDSPEEDCOLOR);
    if (key_value) {
        persist_write_int(KEY_WINDSPEEDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SUNRISECOLOR);
    if (key_value) {
        persist_write_int(KEY_SUNRISECOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SUNSETCOLOR);
    if (key_value) {
        persist_write_int(KEY_SUNSETCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_COMPASSCOLOR);
    if (key_value) {
        persist_write_int(KEY_COMPASSCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SECONDSCOLOR);
    if (key_value) {
        persist_write_int(KEY_SECONDSCOLOR, key_value->value->int32);
    }

    #if defined(PBL_HEALTH)
    key_value = NULL; key_value = dict_find(iterator, KEY_STEPSCOLOR);
    if (key_value) {
        persist_write_int(KEY_STEPSCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DISTCOLOR);
    if (key_value) {
        persist_write_int(KEY_DISTCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_CALCOLOR);
    if (key_value) {
        persist_write_int(KEY_CALCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPCOLOR);
    if (key_value) {
        persist_write_int(KEY_SLEEPCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DEEPCOLOR);
    if (key_value) {
        persist_write_int(KEY_DEEPCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_STEPSBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_STEPSBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DISTBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_DISTBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_CALBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_CALBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_SLEEPBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DEEPBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_DEEPBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_ACTIVECOLOR);
    if (key_value) {
        persist_write_int(KEY_ACTIVECOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_ACTIVEBEHINDCOLOR);
    if (key_value) {
        persist_write_int(KEY_ACTIVEBEHINDCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_HEARTCOLOR);
    if (key_value) {
        persist_write_int(KEY_HEARTCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_HEARTCOLOROFF);
    if (key_value) {
        persist_write_int(KEY_HEARTCOLOROFF, key_value->value->int32);
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

    key_value = NULL; key_value = dict_find(iterator, KEY_FONTTYPE);
    if (key_value) {
        persist_write_int(KEY_FONTTYPE, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_BLUETOOTHDISCONNECT);
    if (key_value) {
        if(key_value->value->int8) configs += FLAG_BLUETOOTH;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_BLUETOOTHCOLOR);
    if (key_value) {
        persist_write_int(KEY_BLUETOOTHCOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_OVERRIDELOCATION);
    if (key_value) {
        persist_write_string(KEY_OVERRIDELOCATION, key_value->value->cstring);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_UPDATE);
    if (key_value) {
        if (!key_value->value->int8) configs += FLAG_UPDATE;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_UPDATECOLOR);
    if (key_value) {
        persist_write_int(KEY_UPDATECOLOR, key_value->value->int32);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_LOCALE);
    if (key_value) {
        persist_write_int(KEY_LOCALE, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DATEFORMAT);
    if (key_value) {
        persist_write_int(KEY_DATEFORMAT, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TEXTALIGN);
    if (key_value) {
        persist_write_int(KEY_TEXTALIGN, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SPEEDUNIT);
    if (key_value) {
        persist_write_int(KEY_SPEEDUNIT, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_LEADINGZERO);
    if (key_value) {
        if (!key_value->value->int8) configs += FLAG_LEADINGZERO;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SIMPLEMODE);
    if (key_value) {
        if (key_value->value->int8) configs += FLAG_SIMPLEMODE;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_QUICKVIEW);
    if (key_value) {
        if (!key_value->value->int8) configs += FLAG_QUICKVIEW;
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTA);
    if (key_value) {
        set_module(SLOT_A, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTA, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTB);
    if (key_value) {
        set_module(SLOT_B, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTB, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTC);
    if (key_value) {
        set_module(SLOT_C, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTC, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTD);
    if (key_value) {
        set_module(SLOT_D, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTD, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTE);
    if (key_value) {
        set_module(SLOT_E, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTE, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLOTF);
    if (key_value) {
        set_module(SLOT_F, key_value->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTF, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTA);
    if (key_value) {
        set_module(SLOT_A, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTA, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTB);
    if (key_value) {
        set_module(SLOT_B, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTB, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTC);
    if (key_value) {
        set_module(SLOT_C, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTC, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTD);
    if (key_value) {
        set_module(SLOT_D, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTD, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTE);
    if (key_value) {
        set_module(SLOT_E, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTE, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_SLEEPSLOTF);
    if (key_value) {
        set_module(SLOT_F, key_value->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTF, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTA);
    if (key_value) {
        set_module(SLOT_A, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTA, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTB);
    if (key_value) {
        set_module(SLOT_B, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTB, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTC);
    if (key_value) {
        set_module(SLOT_C, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTC, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTD);
    if (key_value) {
        set_module(SLOT_D, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTD, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTE);
    if (key_value) {
        set_module(SLOT_E, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTE, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_TAPSLOTF);
    if (key_value) {
        set_module(SLOT_F, key_value->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTF, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTA);
    if (key_value) {
        set_module(SLOT_A, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTA, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTB);
    if (key_value) {
        set_module(SLOT_B, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTB, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTC);
    if (key_value) {
        set_module(SLOT_C, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTC, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTD);
    if (key_value) {
        set_module(SLOT_D, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTD, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTE);
    if (key_value) {
        set_module(SLOT_E, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTE, key_value->value->int8);
    }
    key_value = NULL; key_value = dict_find(iterator, KEY_WRISTSLOTF);
    if (key_value) {
        set_module(SLOT_F, key_value->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTF, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_TAPTIME);
    if (key_value) {
        timeout_sec = key_value->value->int8;
        persist_write_int(KEY_TAPTIME, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_WEATHERTIME);
    if (key_value) {
        persist_write_int(KEY_WEATHERTIME, key_value->value->int8);
    }

    key_value = NULL; key_value = dict_find(iterator, KEY_DATESEPARATOR);
    if (key_value) {
        persist_write_int(KEY_DATESEPARATOR, key_value->value->int8);
    }

    persist_write_int(KEY_CONFIGS, configs);
    set_config_toggles(configs);
    set_timezone(tz_name, tz_hour, tz_minute);

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
    timeout_sec = persist_exists(KEY_TAPTIME) ? persist_read_int(KEY_TAPTIME) : 7;
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
            reset_tap_handler();
            reset_wrist_handler();
        }
    }
    #endif

    if (units_changed & MINUTE_UNIT) {
        if (is_weather_enabled()) {
            #if defined(PBL_HEALTH)
                if (is_user_sleeping()) {
                    min_count++;
                    if (min_count > 90) {
                        update_weather();
                        min_count = 0;
                    }
                } else {
                    update_weather();
                }
            #else
                update_weather();
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
    app_message_open(1024, 64);

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
