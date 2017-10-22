#include <pebble.h>
#include <time.h>
#include "keys.h"
#include "text.h"
#include "configs.h"
#include "clock.h"

static bool weather_enabled;
static bool use_celsius;
static int last_update = 0;
static int weather_interval = 30;

static char* weather_conditions[] = {
    "\U0000F07B", // 'unknown': 0,
    "\U0000F00D", // 'clear': 1,
    "\U0000F00D", // 'sunny': 2,
    "\U0000F002", // 'partlycloudy': 3,
    "\U0000F002", // 'mostlycloudy': 4,
    "\U0000F00C", // 'mostlysunny': 5,
    "\U0000F002", // 'partlysunny': 6,
    "\U0000F013", // 'cloudy': 7,
    "\U0000F019", // 'rain': 8,
    "\U0000F01B", // 'snow': 9,
    "\U0000F01D", // 'tstorms': 10,
    "\U0000F0b5", // 'sleat': 11,
    "\U0000F00A", // 'flurries': 12,
    "\U0000F0b6", // 'hazy': 13,
    "\U0000F01D", // 'chancetstorms': 14,
    "\U0000F01B", // 'chancesnow': 15,
    "\U0000F0b5", // 'chancesleat': 16,
    "\U0000F008", // 'chancerain': 17,
    "\U0000F01B", // 'chanceflurries': 18,
    "\U0000F07B", // 'nt_unknown': 19,
    "\U0000F02E", // 'nt_clear': 20,
    "\U0000F02E", // 'nt_sunny': 21,
    "\U0000F086", // 'nt_partlycloudy': 22,
    "\U0000F086", // 'nt_mostlycloudy': 23,
    "\U0000F081", // 'nt_mostlysunny': 24,
    "\U0000F086", // 'nt_partlysunny': 25,
    "\U0000F013", // 'nt_cloudy': 26,
    "\U0000F019", // 'nt_rain': 27,
    "\U0000F01B", // 'nt_snow': 28,
    "\U0000F01D", // 'nt_tstorms': 29,
    "\U0000F0b5", // 'nt_sleat': 30,
    "\U0000F038", // 'nt_flurries': 31,
    "\U0000F04A", // 'nt_hazy': 32,
    "\U0000F01D", // 'nt_chancetstorms': 33,
    "\U0000F038", // 'nt_chancesnow': 34,
    "\U0000F0B3", // 'nt_chancesleat': 35,
    "\U0000F036", // 'nt_chancerain': 36,
    "\U0000F038", // 'nt_chanceflurries': 37,
    "\U0000F003", // 'fog': 38,
    "\U0000F04A", // 'nt_fog': 39,
    "\U0000F04e", // drizzle: 40
    "\U0000F015", // hail: 41
    "\U0000F076", // cold: 42
    "\U0000F072", // hot: 43
    "\U0000F050", // windy: 44
    "\U0000F056", // tornado: 45
    "\U0000F073", // hurricane: 46
};

void update_weather(bool force) {
    int current_time = (int)time(NULL);
    if (force || last_update == 0 || (current_time - last_update) >= weather_interval * 60) {
        DictionaryIterator *iter;
        AppMessageResult result = app_message_outbox_begin(&iter);
        if (result == APP_MSG_OK) {
            dict_write_uint8(iter, KEY_REQUESTWEATHER, 1);
            result = app_message_outbox_send();

            if (result == APP_MSG_OK) {
                last_update = current_time;
            }
        }
    }
}

char* get_wind_direction_text(int degrees) {
    if (degrees > 349 || degrees <= 11) {
        return "S"; // N -> S
    } else if (degrees > 11 && degrees <= 34) {
        return "SSW"; // NNE -> SSW
    } else if (degrees > 34 && degrees <= 56) {
        return "SW"; // NE -> SW
    } else if (degrees > 56 && degrees <= 79) {
        return "WSW"; // ENE -> WSW
    } else if (degrees > 79 && degrees <= 101) {
        return "W"; // E -> W
    } else if (degrees > 101 && degrees <= 124) {
        return "WNW"; // ESE -> WNW
    } else if (degrees > 124 && degrees <= 146) {
        return "NW"; // SE -> NW
    } else if (degrees > 146 && degrees <= 169) {
        return "NNW"; // SSE -> NNW
    } else if (degrees > 169 && degrees <= 191) {
        return "N"; // S -> N
    } else if (degrees > 191 && degrees <= 214) {
        return "NNE"; // SSW -> NNE
    } else if (degrees > 214 && degrees <= 236) {
        return "NE"; // SW -> NE
    } else if (degrees > 236 && degrees <= 259) {
        return "ENE"; // WSW -> ENE
    } else if (degrees > 259 && degrees <= 281) {
        return "E"; // W -> E
    } else if (degrees > 281 && degrees <= 304) {
        return "ESE"; // WNW -> ESE
    } else if (degrees > 304 && degrees <= 326) {
        return "SE"; // NW -> SE
    } else if (degrees > 326 && degrees <= 349) {
        return "SSE"; // NNW -> SSE
    }

    return "NONE";
}

char* get_wind_direction(int degrees) {
    if (degrees > 349 || degrees <= 11) {
        return "0"; // N -> S
    } else if (degrees > 11 && degrees <= 34) {
        return "1"; // NNE -> SSW
    } else if (degrees > 34 && degrees <= 56) {
        return "2"; // NE -> SW
    } else if (degrees > 56 && degrees <= 79) {
        return "3"; // ENE -> WSW
    } else if (degrees > 79 && degrees <= 101) {
        return "4"; // E -> W
    } else if (degrees > 101 && degrees <= 124) {
        return "5"; // ESE -> WNW
    } else if (degrees > 124 && degrees <= 146) {
        return "6"; // SE -> NW
    } else if (degrees > 146 && degrees <= 169) {
        return "7"; // SSE -> NNW
    } else if (degrees > 169 && degrees <= 191) {
        return "S"; // S -> N
    } else if (degrees > 191 && degrees <= 214) {
        return "T"; // SSW -> NNE
    } else if (degrees > 214 && degrees <= 236) {
        return "U"; // SW -> NE
    } else if (degrees > 236 && degrees <= 259) {
        return "V"; // WSW -> ENE
    } else if (degrees > 259 && degrees <= 281) {
        return "W"; // W -> E
    } else if (degrees > 281 && degrees <= 304) {
        return "X"; // WNW -> ESE
    } else if (degrees > 304 && degrees <= 326) {
        return "Y"; // NW -> SE
    } else if (degrees > 326 && degrees <= 349) {
        return "Z"; // NNW -> SSE
    }

    return "o";
}

void update_weather_values(int temp_val, int weather_val) {
    if (is_module_enabled(MODULE_WEATHER)) {
        char temp_pattern[4];
        char temp_text[8];
        char weather_text[4];

        if (get_loaded_font() == BLOCKO_BIG_FONT || get_loaded_font() == BLOCKO_FONT) {
            strcpy(temp_pattern, use_celsius ? "%dc" : "%df");
        } else {
            strcpy(temp_pattern, use_celsius ? "%dC" : "%dF");
        }

        snprintf(temp_text, sizeof(temp_text), temp_pattern, temp_val);
        snprintf(weather_text, sizeof(weather_text), "%s", weather_conditions[weather_val]);

        set_temp_cur_layer_text(temp_text);
        set_weather_layer_text(weather_text);
    } else {
        set_temp_cur_layer_text("");
        set_weather_layer_text("");
    }
}

void update_forecast_values(int max_val, int min_val) {
    if (is_module_enabled(MODULE_FORECAST)) {
        char max_text[6];
        char min_text[6];

        snprintf(max_text, sizeof(max_text), "%d", max_val);
        snprintf(min_text, sizeof(min_text), "%d", min_val);

        set_temp_max_layer_text(max_text);
        set_temp_min_layer_text(min_text);
        set_max_icon_layer_text("y");
        set_min_icon_layer_text("z");
    } else {
        set_temp_max_layer_text("");
        set_temp_min_layer_text("");
        set_max_icon_layer_text("");
        set_min_icon_layer_text("");
    }
}

void update_sunrise(int sunrise) {
    if (is_module_enabled(MODULE_SUNRISE)) {
        char sunrise_text[6];
        time_t temp = (time_t)sunrise;
        struct tm *tick_time = localtime(&temp);
        set_hours(tick_time, sunrise_text, sizeof(sunrise_text));
        set_sunrise_layer_text(sunrise_text);
        set_sunrise_icon_layer_text("\U0000F051");
    } else {
        set_sunrise_layer_text("");
        set_sunrise_icon_layer_text("");
    }
}

void update_sunset(int sunset) {
    if (is_module_enabled(MODULE_SUNSET)) {
        char sunset_text[6];
        time_t temp = (time_t)sunset;
        struct tm *tick_time = localtime(&temp);
        set_hours(tick_time, sunset_text, sizeof(sunset_text));
        set_sunset_layer_text(sunset_text);
        set_sunset_icon_layer_text("\U0000F052");
    } else {
        set_sunset_layer_text("");
        set_sunset_icon_layer_text("");
    }
}

void update_wind_values(int speed, int direction) {
    if (is_module_enabled(MODULE_WIND)) {
        char wind_speed[4];
        char wind_dir[2];
        char *wind_unit;

        strcpy(wind_dir, get_wind_direction(direction));
        if (get_wind_speed_unit() == UNIT_KPH) {
            speed = (speed * 1.60934)/1;
            wind_unit = ")";
        } else if (get_wind_speed_unit() == UNIT_KNOTS) {
            speed = (speed * 0.868976)/1;
            wind_unit = "*";
        } else {
            wind_unit = "(";
        }

        snprintf(wind_speed, sizeof(wind_speed), "%d", speed);
        set_wind_direction_layer_text(wind_dir);
        set_wind_speed_layer_text(wind_speed);
        set_wind_unit_layer_text(wind_unit);
    } else {
        set_wind_direction_layer_text("");
        set_wind_speed_layer_text("");
        set_wind_unit_layer_text("");
    }
}

static bool get_weather_enabled() {
    bool weather_module_available =
        is_module_enabled_any(MODULE_WEATHER) ||
        is_module_enabled_any(MODULE_FORECAST) ||
        is_module_enabled_any(MODULE_WIND) ||
        is_module_enabled_any(MODULE_SUNRISE) ||
        is_module_enabled_any(MODULE_SUNSET);
    return is_weather_toggle_enabled() || weather_module_available;
}

static void update_weather_from_storage() {
    if (persist_exists(KEY_TEMP)) {
        update_weather_values(persist_read_int(KEY_TEMP), persist_read_int(KEY_WEATHER));
    }

    if (persist_exists(KEY_MIN)) {
        update_forecast_values(persist_read_int(KEY_MAX), persist_read_int(KEY_MIN));
    }

    if (persist_exists(KEY_SPEED)) {
        update_wind_values(persist_read_int(KEY_SPEED), persist_read_int(KEY_DIRECTION));
    }

    if (persist_exists(KEY_SUNRISE)) {
        update_sunrise(persist_read_int(KEY_SUNRISE));
    }

    if (persist_exists(KEY_SUNSET)) {
        update_sunset(persist_read_int(KEY_SUNSET));
    }
}

void toggle_weather(uint8_t reload_origin) {
    weather_enabled = get_weather_enabled();
    if (reload_origin == RELOAD_CONFIGS || reload_origin == RELOAD_DEFAULT) {
        weather_interval = persist_exists(KEY_WEATHERTIME) ? persist_read_int(KEY_WEATHERTIME) : 30;
    }
    if (weather_enabled) {
        use_celsius = is_use_celsius_enabled();
        update_weather_from_storage();
        if (reload_origin == RELOAD_MODULE || reload_origin == RELOAD_CONFIGS) {
            update_weather(true);
        }
    } else {
        set_temp_cur_layer_text("");
        set_temp_max_layer_text("");
        set_temp_min_layer_text("");
        set_weather_layer_text("");
        set_max_icon_layer_text("");
        set_min_icon_layer_text("");
        set_wind_direction_layer_text("");
        set_wind_speed_layer_text("");
        set_wind_unit_layer_text("");
        set_sunrise_layer_text("");
        set_sunrise_icon_layer_text("");
        set_sunset_icon_layer_text("");
    }
}

void store_weather_values(int temp, int max, int min, int weather, int speed, int direction, int sunrise, int sunset) {
    persist_write_int(KEY_TEMP, temp);
    persist_write_int(KEY_MAX, max);
    persist_write_int(KEY_MIN, min);
    persist_write_int(KEY_WEATHER, weather);
    persist_write_int(KEY_SPEED, speed);
    persist_write_int(KEY_DIRECTION, direction);
    persist_write_int(KEY_SUNRISE, sunrise);
    persist_write_int(KEY_SUNSET, sunset);
}

bool is_weather_enabled() {
    return weather_enabled;
}
