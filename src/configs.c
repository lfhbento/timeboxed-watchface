#include <pebble.h>
#include "configs.h"
#include "keys.h"
#include "health.h"

static bool configs_loaded;
static int configs;
static int modules[4];
static int modules_sleep[4];

<<<<<<< a73bd3dd480ce384ec8e1e3d401e1f42cd810f28
static int load_configs() {
    configs = persist_exists(KEY_CONFIGS) ? persist_read_int(KEY_CONFIGS) : 0;
=======
void set_module(int slot, int module, bool sleeping_mode) {
    if (!sleeping_mode) {
        modules[slot] = module;
    } else {
        modules_sleep[slot] = module;
    }
}

bool is_module_enabled(int module) {
    if (should_show_sleep_data()) {
        for (unsigned int i = 0; i < sizeof(modules_sleep); ++i) {
            if (modules_sleep[i] == module) {
                return true;
            }
        }
    } else {
        for (unsigned int i = 0; i < sizeof(modules); ++i) {
            if (modules[i] == module) {
                return true;
            }
        }
    }
    return false;
}

static int load_config_toggles() {
    configs = persist_exists(KEY_CONFIGS) ? persist_read_int(KEY_CONFIGS) : -1;
>>>>>>> more component separation for modularization
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

static int get_module_for_slot(int index, int key, int key_sleep) {
    if (should_show_sleep_data()) {
        if (!modules_sleep[index]) {
            modules_sleep[index] = persist_read_int(key_sleep);
        }
        return modules_sleep[index];
    } else {
        if (!modules[index]) {
            modules[index] = persist_read_int(key);
        }
        return modules[index];
    }
    return MODULE_NONE;
}

int get_slot_a_module() {
    int value = get_module_for_slot(SLOT_A, KEY_SLOTA, KEY_SLEEP_SLOTA);
    if (!value) value = MODULE_WEATHER;
    return value;
}

int get_slot_b_module() {
    int value = get_module_for_slot(SLOT_B, KEY_SLOTB, KEY_SLEEP_SLOTB);
    if (!value) value = MODULE_FORECAST;
    return value;
}

int get_slot_c_module() {
    int value = get_module_for_slot(SLOT_C, KEY_SLOTC, KEY_SLEEP_SLOTC);
    if (!value) {
        if (should_show_sleep_data()) {
            value = MODULE_SLEEP;
        } else {
            value = MODULE_STEPS;
        }
    }
    return value;
}

int get_slot_d_module() {
    int value = get_module_for_slot(SLOT_D, KEY_SLOTD, KEY_SLEEP_SLOTD);
    if (!value) {
        if (should_show_sleep_data()) {
            value = MODULE_DEEP;
        } else {
            if (is_use_calories_enabled()) {
                value = MODULE_CAL;
            } else {
                value = MODULE_DIST;
            }
        }
    }
    return value;
}
