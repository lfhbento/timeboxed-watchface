#include <pebble.h>
#include "screen.h"
#include "text.h"
#include "health.h"
#include "weather.h"
#include "locales.h"
#include "configs.h"
#include "keys.h"
#include "accel.h"

void load_screen(bool from_configs, Window *watchface) {
    load_locale();
    update_time();
    set_colors(watchface);
    toggle_health(from_configs);
    toggle_weather(from_configs);
    battery_handler(battery_state_service_peek());
    bt_handler(connection_service_peek_pebble_app_connection());
}

void reload_fonts() {
    unload_face_fonts();
    load_face_fonts();
}

void recreate_text_layers(Window * watchface) {
    destroy_text_layers();
    create_text_layers(watchface);
    set_face_fonts();
}

void redraw_screen(Window *watchface) {
    recreate_text_layers(watchface);
    load_screen(true, watchface);
}

void bt_handler(bool connected) {
    if (connected) {
        set_bluetooth_layer_text("");
        persist_write_int(KEY_BLUETOOTHDISCONNECT, 0);
    } else {
        bool did_vibrate = persist_exists(KEY_BLUETOOTHDISCONNECT) ? persist_read_int(KEY_BLUETOOTHDISCONNECT): 0;
        if (is_bluetooth_vibrate_enabled() && !is_user_sleeping() && !did_vibrate) {
            vibes_long_pulse();
            persist_write_int(KEY_BLUETOOTHDISCONNECT, 1);
        }
        set_bluetooth_color();
        set_bluetooth_layer_text("a");
    }
}

void battery_handler(BatteryChargeState charge_state) {
    char s_battery_buffer[8];

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent < 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }
    set_battery_color(charge_state.charge_percent);
    set_battery_layer_text(s_battery_buffer);
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
    }
    set_update_layer_text(update_available ? "f" : "");
}
