#include <pebble.h>
#include "screen.h"
#include "text.h"
#include "health.h"
#include "weather.h"
#include "locales.h"
#include "configs.h"
#include "keys.h"
#include "accel.h"
#include "compass.h"
#include "clock.h"

void load_screen(uint8_t reload_origin, Window *watchface) {
    load_locale();
    update_time();

    #if defined PBL_COMPASS
    if (reload_origin != RELOAD_REDRAW) {
        init_compass_service(watchface);
    }
    #endif

    set_colors(watchface);

    #if defined(PBL_HEALTH)
    toggle_health(reload_origin);
    #endif

    toggle_weather(reload_origin);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    update_seconds(tick_time);
}

void reload_fonts() {
    unload_face_fonts();
    load_face_fonts();
}

void recreate_text_layers(Window * watchface) {
    destroy_text_layers();
    create_text_layers(watchface);
    set_face_fonts();
    notify_update(false);
}

void redraw_screen(Window *watchface) {
    recreate_text_layers(watchface);
    load_screen(RELOAD_MODULE, watchface);
}

void bt_handler(bool connected) {
    if (connected) {
        set_bluetooth_layer_text("");
        persist_write_int(KEY_BLUETOOTHDISCONNECT, 0);
    } else {
        bool did_vibrate = persist_exists(KEY_BLUETOOTHDISCONNECT) ? persist_read_int(KEY_BLUETOOTHDISCONNECT): 0;
        bool should_vibrate_if_quiet = !is_mute_on_quiet_enabled() || !quiet_time_is_active();
        if (is_bluetooth_vibrate_enabled() && should_vibrate_if_quiet && !is_user_sleeping() && !did_vibrate) {
            vibes_long_pulse();
            persist_write_int(KEY_BLUETOOTHDISCONNECT, 1);
        }
        set_bluetooth_color();
        set_bluetooth_layer_text("a");
    }
}

void battery_handler(BatteryChargeState charge_state) {
    if (is_module_enabled(MODULE_BATTERY)) {
        char s_battery_buffer[8];

        if (charge_state.is_charging) {
            snprintf(s_battery_buffer, sizeof(s_battery_buffer), "chg");
        } else {
            snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent < 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
        }
        set_battery_color(charge_state.charge_percent);
        set_battery_layer_text(s_battery_buffer);
    } else {
        set_battery_layer_text("");
    }
}

void check_for_updates() {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, KEY_HASUPDATE, 1);
    app_message_outbox_send();
}

void notify_update(int update_available) {
    if (update_available) {
        set_update_color();
        set_update_layer_text("f");
    } else {
        set_update_layer_text("");
    }
}
