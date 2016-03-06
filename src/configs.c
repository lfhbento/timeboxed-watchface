#include <pebble.h>
#include "configs.h"
#include "keys.h"

static bool configs_loaded;
static int configs;

static int load_configs() {
    configs = persist_exists(KEY_CONFIGS) ? persist_read_int(KEY_CONFIGS) : -1;
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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_weather_toggle_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_WEATHER, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_WEATHER;
}

bool is_health_toggle_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_health_toggle_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_HEALTH, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_HEALTH;
}

bool is_use_celsius_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_use_celsius_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_CELSIUS, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_CELSIUS;
}

bool is_use_km_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_use_km_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_KM, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_KM;
}

bool is_bluetooth_vibrate_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_bluetooth_vibrate_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_BLUETOOTH, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_BLUETOOTH;
}

bool is_update_disabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_update_disabled(): (%d). %d%2d", get_config_toggles() & FLAG_UPDATE, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_UPDATE;
}

bool is_leading_zero_disabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_leading_zero_disabled(): (%d). %d%2d", get_config_toggles() & FLAG_LEADINGZERO, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_LEADINGZERO;
}

bool is_advanced_colors_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_advanced_colors_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_ADVANCED, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_ADVANCED;
}

bool is_sleep_data_enabled() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "is_sleep_data_enabled(): (%d). %d%2d", get_config_toggles() & FLAG_SLEEP, (int)time(NULL), (int)time_ms(NULL, NULL));
    return get_config_toggles() & FLAG_SLEEP;
}
