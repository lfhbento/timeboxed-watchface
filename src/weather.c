#include <pebble.h>
#include "keys.h"
#include "text.h"
#include "configs.h"

static bool weather_enabled;
static bool use_celsius;

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

static char* wind_directions[] = {
    "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
    "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW",
    "N/A"
};

void update_weather(void) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    app_message_outbox_send();
}

static char* get_wind_direction(int degrees) {
    if (degrees > 349 || degrees <= 11) {
        return wind_directions[0]; // N
    } else if (degrees > 11 && degrees <= 34) {
        return wind_directions[1]; // NNE
    } else if (degrees > 34 && degrees <= 56) {
        return wind_directions[2]; // NE
    } else if (degrees > 56 && degrees <= 79) {
        return wind_directions[3]; // ENE
    } else if (degrees > 79 && degrees <= 101) {
        return wind_directions[4]; // E
    } else if (degrees > 101 && degrees <= 124) {
        return wind_directions[5]; // ESE
    } else if (degrees > 124 && degrees <= 146) {
        return wind_directions[6]; // SE
    } else if (degrees > 146 && degrees <= 169) {
        return wind_directions[7]; // SSE
    } else if (degrees > 169 && degrees <= 191) {
        return wind_directions[8]; // S
    } else if (degrees > 191 && degrees <= 214) {
        return wind_directions[9]; // SSW
    } else if (degrees > 214 && degrees <= 236) {
        return wind_directions[10]; // SW
    } else if (degrees > 239 && degrees <= 259) {
        return wind_directions[11]; // WSW
    } else if (degrees > 259 && degrees <= 281) {
        return wind_directions[12]; // W
    } else if (degrees > 281 && degrees <= 304) {
        return wind_directions[13]; // WNW
    } else if (degrees > 304 && degrees <= 326) {
        return wind_directions[14]; // NW
    } else if (degrees > 326 && degrees <= 349) {
        return wind_directions[15]; // NNW
    }

    return wind_directions[16];
}

void update_weather_values(int temp_val, int weather_val) {
    if (is_module_enabled(MODULE_WEATHER)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather values... %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    }
}

void update_forecast_values(int max_val, int min_val) {
    if (is_module_enabled(MODULE_FORECAST)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating forecast values... %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        char max_text[8];
        char min_text[8];

        snprintf(max_text, sizeof(max_text), "%d", max_val);
        snprintf(min_text, sizeof(min_text), "%d", min_val);

        set_temp_max_layer_text(max_text);
        set_temp_min_layer_text(min_text);
        set_max_icon_layer_text("\U0000F058");
        set_min_icon_layer_text("\U0000F044");
    }
}

void update_wind_values(int speed, int direction) {
    if (is_module_enabled(MODULE_WIND)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating wind values... %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        char wind_speed[10];
        char wind_dir[4];

        if (get_wind_speed_unit() == UNIT_KPH) {
            speed = (speed * 1.60934)/1;
        } else if (get_wind_speed_unit() == UNIT_KNOTS) {
            speed = (speed * 0.868976)/1;
        }

        speed = 100;

        strcpy(wind_dir, get_wind_direction(direction));
        snprintf(wind_speed, sizeof(wind_speed), "%d", speed);

        set_wind_direction_layer_text(wind_dir);
        set_wind_speed_layer_text(wind_speed);
    }
}

static bool get_weather_enabled() {
    bool weather_module_available =
        is_module_enabled(MODULE_WEATHER) ||
        is_module_enabled(MODULE_FORECAST) ||
        is_module_enabled(MODULE_WIND);
    return is_weather_toggle_enabled() || weather_module_available;
}

void toggle_weather(bool from_configs) {
    weather_enabled = get_weather_enabled();
    if (weather_enabled) {

        use_celsius = is_use_celsius_enabled();

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather is enabled. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        if (from_configs) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from configs. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0);
            update_forecast_values(0, 0);
            update_wind_values(0, 16);
            update_weather();
        } else if (persist_exists(KEY_TEMP)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from storage. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            int temp = persist_read_int(KEY_TEMP);
            int weather = persist_read_int(KEY_WEATHER);
            update_weather_values(temp, weather);

            int min = persist_read_int(KEY_MIN);
            int max = persist_read_int(KEY_MAX);
            update_forecast_values(max, min);

            int speed = persist_read_int(KEY_SPEED);
            int direction = persist_read_int(KEY_DIRECTION);
            update_wind_values(speed, direction);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "No weather data from storage. Requesting... %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0);
            update_forecast_values(0, 0);
            update_wind_values(0, 16);
            update_weather();
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather disabled, clearing up. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_temp_cur_layer_text("");
        set_temp_max_layer_text("");
        set_temp_min_layer_text("");
        set_weather_layer_text("");
        set_max_icon_layer_text("");
        set_min_icon_layer_text("");
    }
}

void store_weather_values(int temp, int max, int min, int weather, int speed, int direction) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing weather data. %d %d%03d", use_celsius, (int)time(NULL), (int)time_ms(NULL, NULL));
    persist_write_int(KEY_TEMP, temp);
    persist_write_int(KEY_MAX, max);
    persist_write_int(KEY_MIN, min);
    persist_write_int(KEY_WEATHER, weather);
    persist_write_int(KEY_SPEED, speed);
    persist_write_int(KEY_DIRECTION, direction);
}

bool is_weather_enabled() {
    return weather_enabled;
}
