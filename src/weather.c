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

void update_weather(void) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting weather. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    app_message_outbox_send();
}

void update_weather_values(int temp_val, int max_val, int min_val, int weather_val) {
    char temp_pattern[4];
    char temp_text[8];
    char max_text[8];
    char min_text[8];
    char weather_text[4];

    if (get_loaded_font() == BLOCKO_BIG_FONT) {
        strcpy(temp_pattern, use_celsius ? "%dc" : "%df");
    } else if (get_loaded_font() == BLOCKO_FONT) {
        strcpy(temp_pattern, use_celsius ? "%dc" : "%df");
    } else {
        strcpy(temp_pattern, use_celsius ? "%dC" : "%dF");
    }

    snprintf(temp_text, sizeof(temp_text), temp_pattern, temp_val);
    snprintf(max_text, sizeof(max_text), "%d", max_val);
    snprintf(min_text, sizeof(min_text), "%d", min_val);
    snprintf(weather_text, sizeof(weather_text), "%s", weather_conditions[weather_val]);

    set_temp_cur_layer_text(temp_text);
    set_temp_max_layer_text(max_text);
    set_temp_min_layer_text(min_text);
    set_weather_layer_text(weather_text);
    set_max_icon_layer_text("\U0000F058");
    set_min_icon_layer_text("\U0000F044");
}

void toggle_weather(bool from_configs) {
    weather_enabled = is_weather_toggle_enabled();
    if (weather_enabled) {

        use_celsius = is_use_celsius_enabled();

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather is enabled. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        if (from_configs) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from configs. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0, 0, 0);
            update_weather();
        } else if (persist_exists(KEY_TEMP)) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating weather from storage. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            int temp = persist_read_int(KEY_TEMP);
            int min = persist_read_int(KEY_MIN);
            int max = persist_read_int(KEY_MAX);
            int weather = persist_read_int(KEY_WEATHER);

            update_weather_values(temp, max, min, weather);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "No weather data from storage. Requesting... %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_weather_values(0, 0, 0, 0);
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

void store_weather_values(int temp, int max, int min, int weather) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing weather data. %d %d%03d", use_celsius, (int)time(NULL), (int)time_ms(NULL, NULL));
    persist_write_int(KEY_TEMP, temp);
    persist_write_int(KEY_MAX, max);
    persist_write_int(KEY_MIN, min);
    persist_write_int(KEY_WEATHER, weather);
}

bool is_weather_enabled() {
    return weather_enabled;
}
