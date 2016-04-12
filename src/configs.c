#include <pebble.h>
#include "configs.h"
#include "keys.h"
#include "health.h"

static bool configs_loaded;
static bool modules_loaded;
static int configs;
static int modules[4];
static int modules_sleep[4];

void set_module(int slot, int module, bool sleeping_mode) {
    if (!sleeping_mode) {
        modules[slot] = module;
    } else {
        modules_sleep[slot] = module;
    }
}

int get_wind_speed_unit() {
    return UNIT_MPH;
}

static void load_modules() {
    modules[SLOT_A] = persist_read_int(KEY_SLOTA);
    modules[SLOT_B] = persist_read_int(KEY_SLOTB);
    modules[SLOT_C] = persist_read_int(KEY_SLOTC);
    modules[SLOT_D] = persist_read_int(KEY_SLOTD);
    modules_sleep[SLOT_A] = persist_read_int(KEY_SLEEPSLOTA);
    modules_sleep[SLOT_B] = persist_read_int(KEY_SLEEPSLOTB);
    modules_sleep[SLOT_C] = persist_read_int(KEY_SLEEPSLOTC);
    modules_sleep[SLOT_D] = persist_read_int(KEY_SLEEPSLOTD);

    modules_loaded = true;
}

bool is_module_enabled(int module) {
    if (!modules_loaded) {
        load_modules();
    }
    if (should_show_sleep_data()) {
        for (unsigned int i = 0; i < 4; ++i) {
            if (modules_sleep[i] == module) {
                return true;
            }
        }
    } else {
        for (unsigned int i = 0; i < 4; ++i) {
            if (modules[i] == module) {
                return true;
            }
        }
    }
    return false;
}

static int load_config_toggles() {
    configs = persist_exists(KEY_CONFIGS) ? persist_read_int(KEY_CONFIGS) : 0;
    configs_loaded = true;
    return configs;
}

int get_config_toggles() {
    if (!configs_loaded) {
        return load_config_toggles();
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

bool is_simple_mode_enabled() {
    return get_config_toggles() & FLAG_SIMPLEMODE;
}

bool is_timezone_enabled() {
    return get_config_toggles() & FLAG_TIMEZONES;
}

int get_slot_for_module(int module) {
    if (!modules_loaded) {
        load_modules();
    }
    if (should_show_sleep_data()) {
        for (unsigned int i = 0; i < 4; ++i) {
            if (modules_sleep[i] == module) {
                return i;
            }
        }
    } else {
        for (unsigned int i = 0; i < 4; ++i) {
            if (modules[i] == module) {
                return i;
            }
        }
    }
    return -1;
}
