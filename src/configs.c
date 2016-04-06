#include <pebble.h>
#include "configs.h"
#include "keys.h"

static bool configs_loaded;
static int configs;

static int load_configs() {
    configs = persist_exists(KEY_CONFIGS) ? persist_read_int(KEY_CONFIGS) : 0;
    configs_loaded = true;
    return configs;
}

int get_config_toggles() {
    if (!configs_loaded) {
        return load_configs();
    }

    return configs;
}

void set_config_toggles(int toggles) {
    configs = toggles;
    configs_loaded = true;
}

bool is_weather_toggle_enabled() {
    return get_config_toggles() & FLAG_WEATHER;
}

bool is_health_toggle_enabled() {
    return get_config_toggles() & FLAG_HEALTH;
}

bool is_use_celsius_enabled() {
    return get_config_toggles() & FLAG_CELSIUS;
}

bool is_use_km_enabled() {
    return get_config_toggles() & FLAG_KM;
}

bool is_bluetooth_vibrate_enabled() {
    return get_config_toggles() & FLAG_BLUETOOTH;
}

bool is_update_disabled() {
    return get_config_toggles() & FLAG_UPDATE;
}

bool is_leading_zero_disabled() {
    return get_config_toggles() & FLAG_LEADINGZERO;
}

bool is_advanced_colors_enabled() {
    return get_config_toggles() & FLAG_ADVANCED;
}

bool is_sleep_data_enabled() {
    return get_config_toggles() & FLAG_SLEEP;
}

bool is_use_calories_enabled() {
    return get_config_toggles() & FLAG_CALORIES;
}
