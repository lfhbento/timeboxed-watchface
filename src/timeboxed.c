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

static uint8_t min_counter;
static uint8_t weather_interval;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple * error_tuple = dict_find(iterator, KEY_ERROR);

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

    Tuple *keyValue = dict_find(iterator, KEY_SHOWSLEEP);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_SLEEP;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SHOWTAP);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_TAP;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SHOWWRIST);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_WRIST;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_USECELSIUS);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_CELSIUS;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TIMEZONES);
    if (keyValue) {
        persist_write_int(KEY_TIMEZONES, keyValue->value->int8);
        tz_hour = keyValue->value->int8;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TIMEZONESMINUTES);
    if (keyValue) {
        persist_write_int(KEY_TIMEZONESMINUTES, keyValue->value->int8);
        tz_minute = keyValue->value->int8;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TIMEZONESCODE);
    if (keyValue) {
        char* tz_code = keyValue->value->cstring;
        persist_write_string(KEY_TIMEZONESCODE, tz_code);
        strcpy(tz_name, tz_code);
        if (tz_code[0] != '#') configs += FLAG_TIMEZONES;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_BGCOLOR);
    if (keyValue) {
        persist_write_int(KEY_BGCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_HOURSCOLOR);
    if (keyValue) {
        persist_write_int(KEY_HOURSCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_ENABLEADVANCED);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_ADVANCED;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DATECOLOR);
    if (keyValue) {
        persist_write_int(KEY_DATECOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_ALTHOURSCOLOR);
    if (keyValue) {
        persist_write_int(KEY_ALTHOURSCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_BATTERYCOLOR);
    if (keyValue) {
        persist_write_int(KEY_BATTERYCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_BATTERYLOWCOLOR);
    if (keyValue) {
        persist_write_int(KEY_BATTERYLOWCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_WEATHERCOLOR);
    if (keyValue) {
        persist_write_int(KEY_WEATHERCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TEMPCOLOR);
    if (keyValue) {
        persist_write_int(KEY_TEMPCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_MINCOLOR);
    if (keyValue) {
        persist_write_int(KEY_MINCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_MAXCOLOR);
    if (keyValue) {
        persist_write_int(KEY_MAXCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_WINDDIRCOLOR);
    if (keyValue) {
        persist_write_int(KEY_WINDDIRCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_WINDSPEEDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_WINDSPEEDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SUNRISECOLOR);
    if (keyValue) {
        persist_write_int(KEY_SUNRISECOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SUNSETCOLOR);
    if (keyValue) {
        persist_write_int(KEY_SUNSETCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_COMPASSCOLOR);
    if (keyValue) {
        persist_write_int(KEY_COMPASSCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SECONDSCOLOR);
    if (keyValue) {
        persist_write_int(KEY_SECONDSCOLOR, keyValue->value->int32);
    }

    #if defined(PBL_HEALTH)
    keyValue = NULL; keyValue = dict_find(iterator, KEY_STEPSCOLOR);
    if (keyValue) {
        persist_write_int(KEY_STEPSCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DISTCOLOR);
    if (keyValue) {
        persist_write_int(KEY_DISTCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_CALCOLOR);
    if (keyValue) {
        persist_write_int(KEY_CALCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPCOLOR);
    if (keyValue) {
        persist_write_int(KEY_SLEEPCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DEEPCOLOR);
    if (keyValue) {
        persist_write_int(KEY_DEEPCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_STEPSBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_STEPSBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DISTBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_DISTBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_CALBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_CALBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_SLEEPBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DEEPBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_DEEPBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_ACTIVECOLOR);
    if (keyValue) {
        persist_write_int(KEY_ACTIVECOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_ACTIVEBEHINDCOLOR);
    if (keyValue) {
        persist_write_int(KEY_ACTIVEBEHINDCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_HEARTCOLOR);
    if (keyValue) {
        persist_write_int(KEY_HEARTCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_HEARTCOLOROFF);
    if (keyValue) {
        persist_write_int(KEY_HEARTCOLOROFF, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_HEARTLOW);
    if (keyValue) {
        persist_write_int(KEY_HEARTLOW, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_HEARTHIGH);
    if (keyValue) {
        persist_write_int(KEY_HEARTHIGH, keyValue->value->int32);
    }
    #endif

    keyValue = NULL; keyValue = dict_find(iterator, KEY_FONTTYPE);
    if (keyValue) {
        persist_write_int(KEY_FONTTYPE, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_BLUETOOTHDISCONNECT);
    if (keyValue) {
        if(keyValue->value->int8) configs += FLAG_BLUETOOTH;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_BLUETOOTHCOLOR);
    if (keyValue) {
        persist_write_int(KEY_BLUETOOTHCOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_OVERRIDELOCATION);
    if (keyValue) {
        persist_write_string(KEY_OVERRIDELOCATION, keyValue->value->cstring);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_UPDATE);
    if (keyValue) {
        if (!keyValue->value->int8) configs += FLAG_UPDATE;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_UPDATECOLOR);
    if (keyValue) {
        persist_write_int(KEY_UPDATECOLOR, keyValue->value->int32);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_LOCALE);
    if (keyValue) {
        persist_write_int(KEY_LOCALE, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DATEFORMAT);
    if (keyValue) {
        persist_write_int(KEY_DATEFORMAT, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TEXTALIGN);
    if (keyValue) {
        persist_write_int(KEY_TEXTALIGN, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SPEEDUNIT);
    if (keyValue) {
        persist_write_int(KEY_SPEEDUNIT, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_LEADINGZERO);
    if (keyValue) {
        if (!keyValue->value->int8) configs += FLAG_LEADINGZERO;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SIMPLEMODE);
    if (keyValue) {
        if (keyValue->value->int8) configs += FLAG_SIMPLEMODE;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_QUICKVIEW);
    if (keyValue) {
        if (!keyValue->value->int8) configs += FLAG_QUICKVIEW;
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTA);
    if (keyValue) {
        set_module(SLOT_A, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTA, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTB);
    if (keyValue) {
        set_module(SLOT_B, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTB, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTC);
    if (keyValue) {
        set_module(SLOT_C, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTC, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTD);
    if (keyValue) {
        set_module(SLOT_D, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTD, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTE);
    if (keyValue) {
        set_module(SLOT_E, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTE, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLOTF);
    if (keyValue) {
        set_module(SLOT_F, keyValue->value->int8, STATE_NORMAL);
        persist_write_int(KEY_SLOTF, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTA);
    if (keyValue) {
        set_module(SLOT_A, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTA, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTB);
    if (keyValue) {
        set_module(SLOT_B, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTB, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTC);
    if (keyValue) {
        set_module(SLOT_C, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTC, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTD);
    if (keyValue) {
        set_module(SLOT_D, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTD, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTE);
    if (keyValue) {
        set_module(SLOT_E, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTE, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_SLEEPSLOTF);
    if (keyValue) {
        set_module(SLOT_F, keyValue->value->int8, STATE_SLEEP);
        persist_write_int(KEY_SLEEPSLOTF, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTA);
    if (keyValue) {
        set_module(SLOT_A, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTA, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTB);
    if (keyValue) {
        set_module(SLOT_B, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTB, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTC);
    if (keyValue) {
        set_module(SLOT_C, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTC, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTD);
    if (keyValue) {
        set_module(SLOT_D, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTD, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTE);
    if (keyValue) {
        set_module(SLOT_E, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTE, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPSLOTF);
    if (keyValue) {
        set_module(SLOT_F, keyValue->value->int8, STATE_TAP);
        persist_write_int(KEY_TAPSLOTF, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTA);
    if (keyValue) {
        set_module(SLOT_A, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTA, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTB);
    if (keyValue) {
        set_module(SLOT_B, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTB, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTC);
    if (keyValue) {
        set_module(SLOT_C, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTC, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTD);
    if (keyValue) {
        set_module(SLOT_D, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTD, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTE);
    if (keyValue) {
        set_module(SLOT_E, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTE, keyValue->value->int8);
    }
    keyValue = NULL; keyValue = dict_find(iterator, KEY_WRISTSLOTF);
    if (keyValue) {
        set_module(SLOT_F, keyValue->value->int8, STATE_WRIST);
        persist_write_int(KEY_WRISTSLOTF, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_TAPTIME);
    if (keyValue) {
        persist_write_int(KEY_TAPTIME, keyValue->value->int8);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_WEATHERTIME);
    if (keyValue) {
        weather_interval = keyValue->value->int8;
        persist_write_int(KEY_WEATHERTIME, weather_interval);
    }

    keyValue = NULL; keyValue = dict_find(iterator, KEY_DATESEPARATOR);
    if (keyValue) {
        persist_write_int(KEY_DATESEPARATOR, keyValue->value->int8);
    }

    persist_write_int(KEY_CONFIGS, configs);
    set_config_toggles(configs);
    set_timezone(tz_name, tz_hour, tz_minute);

    init_accel_service(watchface);
    #if defined PBL_COMPASS
    init_compass_service(watchface);
    #endif
    reload_fonts();
    recreate_text_layers(watchface);
    load_screen(true, watchface);
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

    save_health_data_to_storage();
    recreate_text_layers(watchface);
    load_screen(false, watchface);
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

    weather_interval = persist_exists(KEY_WEATHERTIME) ? persist_read_int(KEY_WEATHERTIME) : 30;
    min_counter = weather_interval; // after loading, get the next weather update

    load_timezone_from_storage();
}

static void watchface_unload(Window *window) {
    #if defined(PBL_HEALTH)
    save_health_data_to_storage();
    #endif

    unload_face_fonts();

    destroy_text_layers();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_seconds(tick_time);

    if (units_changed & MINUTE_UNIT) {
        update_time();
        min_counter++;

        if (!is_update_disabled() && tick_time->tm_hour == 4 && tick_time->tm_min == 0) { // updates at 4:00am
            check_for_updates();
        }
        if (is_update_disabled()) {
            notify_update(false);
        }

        uint8_t tick_interval = is_user_sleeping() ? 90 : weather_interval;

        #if defined(PBL_HEALTH)
        if (!tap_mode_visible() && !wrist_mode_visible()) {
            show_sleep_data_if_visible(watchface);
        }
        #endif

        if(min_counter >= tick_interval) {
            if (is_weather_enabled()) {
                update_weather();
            }

            #if defined(PBL_HEALTH)
            if (is_user_sleeping()) {
                queue_health_update();
            }
            #endif

            min_counter = 0;
        }

        #if defined(PBL_HEALTH)
        if (tick_time->tm_min % 2 == 0 || is_module_enabled(MODULE_HEART)) { // check for health updates only every 2 minutes if heart rate is disabled
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

    init_accel_service(watchface);

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

    load_screen(false, watchface);
}

static void deinit(void) {
    window_destroy(watchface);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
