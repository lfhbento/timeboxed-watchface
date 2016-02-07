#include <pebble.h>
#include <ctype.h>

#define KEY_TEMP 0
#define KEY_MAX 1
#define KEY_MIN 2
#define KEY_WEATHER 3

static Window *watchface;
static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps_or_sleep;
static TextLayer *dist_or_deep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static GFont time_font;
static GFont date_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_font_big;

static char hour_text[8];
static char date_text[13];
static char temp_text[8];
static char max_text[8];
static char min_text[8];
static char weather_text[4];
static char tz_text[20];
static char steps_or_sleep_text[16];
static char dist_or_deep_text[16];
static char s_battery_buffer[7];
static bool has_health;
static int tz_hour;
static int tz_minute;
static char tz_name[TZ_LEN];
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
};


static void update_time() {
    // Get a tm structuiire
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    struct tm *gmt_time = gmtime(&temp);
    gmt_time->tm_hour = gmt_time->tm_hour + tz_hour;
    mktime(gmt_time);
    gmt_time->tm_min = gmt_time->tm_min + tz_minute;
    mktime(gmt_time);

    // Write the current hours and minutes into a buffer
    strftime(hour_text, sizeof(hour_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), tick_time);
    strftime(date_text, sizeof(date_text), "%a.%b.%d", tick_time);
    strftime(tz_text, sizeof(tz_text), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), gmt_time);

    if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday > tick_time->tm_mday) ||
        (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon > tick_time->tm_mon) ||
        (gmt_time->tm_year > tick_time->tm_year)
    ) {
            strcat(tz_text, "+1");
    } else if ((gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon == tick_time->tm_mon && gmt_time->tm_mday < tick_time->tm_mday) ||
        (gmt_time->tm_year == tick_time->tm_year && gmt_time->tm_mon < tick_time->tm_mon) ||
        (gmt_time->tm_year < tick_time->tm_year)
    ) {
        strcat(tz_text, "-1");
    }

    strcat(tz_text, ".");
    strcat(tz_text, tz_name); 

    for (unsigned char i = 0; date_text[i]; ++i) {
        date_text[i] = tolower((unsigned char)date_text[i]);
    }
    for (unsigned char i = 0; tz_text[i]; ++i) {
        tz_text[i] = tolower((unsigned char)tz_text[i]);
    }

    // Display this time on the TextLayer
    text_layer_set_text(hours, hour_text);
    text_layer_set_text(date, date_text);
    text_layer_set_text(alt_time, tz_text);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *temp_tuple = dict_find(iterator, KEY_TEMP);
    Tuple *max_tuple = dict_find(iterator, KEY_MAX);
    Tuple *min_tuple = dict_find(iterator, KEY_MIN);
    Tuple *weather_tuple = dict_find(iterator, KEY_WEATHER);

    // If all data is available, use it
    if(temp_tuple && max_tuple && min_tuple && weather_tuple) {
        snprintf(temp_text, sizeof(temp_text), "%dc", (int)temp_tuple->value->int32);
        snprintf(max_text, sizeof(max_text), "%d", (int)max_tuple->value->int32);
        snprintf(min_text, sizeof(min_text), "%d", (int)min_tuple->value->int32);
        snprintf(weather_text, sizeof(weather_text), "%s", weather_conditions[(int)weather_tuple->value->int32]);

        text_layer_set_text(temp_cur, temp_text);
        text_layer_set_text(temp_max, max_text);
        text_layer_set_text(temp_min, min_text);
        text_layer_set_text(weather, weather_text);
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void battery_handler(BatteryChargeState charge_state) {

    if (charge_state.is_charging) {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), "(=/=)");
    } else {
        snprintf(s_battery_buffer, sizeof(s_battery_buffer), (charge_state.charge_percent <= 20 ? "! %d%%" : "%d%%"), charge_state.charge_percent);
    }


    text_layer_set_text(battery, s_battery_buffer);
}

static void watchface_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
    base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_20));
    weather_font_big = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));

    GRect bounds = layer_get_bounds(window_layer);

    hours = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_color(hours, GColorWhite);
    text_layer_set_text_alignment(hours, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(hours, time_font);
    
    date = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -2), PBL_IF_ROUND_ELSE(100, 92), bounds.size.w, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_color(date, GColorWhite);
    text_layer_set_text_alignment(date, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(date, date_font);
    text_layer_set_text(date, "");
    
    alt_time = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -2), PBL_IF_ROUND_ELSE(48, 40), bounds.size.w, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_color(alt_time, GColorWhite);
    text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(alt_time, base_font);
    text_layer_set_text(alt_time, "");

    battery = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(122, 114), bounds.size.w, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_color(battery, GColorWhite);
    text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(battery, base_font);
    text_layer_set_text(battery, "");

    weather = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(-14, 4), PBL_IF_ROUND_ELSE(2, 0), bounds.size.w, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_color(weather, GColorWhite);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(weather, weather_font);
    text_layer_set_text(weather, "");

    temp_cur = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(14, 36), PBL_IF_ROUND_ELSE(4, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_color(temp_cur, GColorWhite);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(temp_cur, base_font);
    text_layer_set_text(temp_cur, "");

    temp_min = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(70, 85), PBL_IF_ROUND_ELSE(22, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_color(temp_min, GColorWhite);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);
    text_layer_set_font(temp_min, base_font);
    text_layer_set_text(temp_min, "");

    min_icon = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(60, 75), PBL_IF_ROUND_ELSE(18, -2), bounds.size.w, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_color(min_icon, GColorWhite);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);
    text_layer_set_font(min_icon, weather_font_big);
    text_layer_set_text(min_icon, "\U0000F044");

    temp_max = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(105, 120), PBL_IF_ROUND_ELSE(22, 2), bounds.size.w, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_color(temp_max, GColorWhite);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);
    text_layer_set_font(temp_max, base_font);
    text_layer_set_text(temp_max, "");

    max_icon = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(95, 110), PBL_IF_ROUND_ELSE(18, -2), bounds.size.w, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_color(max_icon, GColorWhite);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);
    text_layer_set_font(max_icon, weather_font_big);
    text_layer_set_text(max_icon, "\U0000F058");

    steps_or_sleep = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148), bounds.size.w, 50));
    text_layer_set_background_color(steps_or_sleep, GColorClear);
    text_layer_set_text_color(steps_or_sleep, GColorWhite);
    text_layer_set_text_alignment(steps_or_sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(steps_or_sleep, base_font);
    text_layer_set_text(steps_or_sleep, "");

    dist_or_deep = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148), bounds.size.w, 50));
    text_layer_set_background_color(dist_or_deep, GColorClear);
    text_layer_set_text_color(dist_or_deep, GColorWhite);
    text_layer_set_text_alignment(dist_or_deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(dist_or_deep, base_font);
    text_layer_set_text(dist_or_deep, "");

    layer_add_child(window_layer, text_layer_get_layer(hours));
    layer_add_child(window_layer, text_layer_get_layer(date));
    layer_add_child(window_layer, text_layer_get_layer(alt_time));
    layer_add_child(window_layer, text_layer_get_layer(battery));
    layer_add_child(window_layer, text_layer_get_layer(weather));
    layer_add_child(window_layer, text_layer_get_layer(min_icon));
    layer_add_child(window_layer, text_layer_get_layer(max_icon));
    layer_add_child(window_layer, text_layer_get_layer(temp_cur));
    layer_add_child(window_layer, text_layer_get_layer(temp_min));
    layer_add_child(window_layer, text_layer_get_layer(temp_max));
    layer_add_child(window_layer, text_layer_get_layer(steps_or_sleep));
    layer_add_child(window_layer, text_layer_get_layer(dist_or_deep));

    tz_hour = 8;
    tz_minute = 0;
    snprintf(tz_name, sizeof(tz_name), "cst");
}

static void watchface_unload(Window *window) {
    text_layer_destroy(hours);
    text_layer_destroy(date);
    text_layer_destroy(alt_time);
    text_layer_destroy(battery);
    text_layer_destroy(weather);
    text_layer_destroy(min_icon);
    text_layer_destroy(max_icon);
    text_layer_destroy(temp_cur);
    text_layer_destroy(temp_min);
    text_layer_destroy(temp_max);
    text_layer_destroy(steps_or_sleep);
    text_layer_destroy(dist_or_deep);
    fonts_unload_custom_font(time_font);
    fonts_unload_custom_font(date_font);
    fonts_unload_custom_font(base_font);
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_font_big);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    if(tick_time->tm_min % 30 == 0) {
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);

        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
    }
}

#if defined(PBL_HEALTH)
static void update_steps_data(void) {

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    time_t start_last_week = start - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7);
    time_t end_last_week = end - (SECONDS_PER_HOUR * HOURS_PER_DAY * 7);

    uint16_t current_steps = 0;
    uint16_t current_dist = 0;
    uint16_t steps_last_week = 0;
    uint16_t dist_last_week = 0;

    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);
    HealthServiceAccessibilityMask mask_steps_last_week =
        health_service_metric_accessible(metric_steps, start_last_week, end_last_week);
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);
    HealthServiceAccessibilityMask mask_dist_last_week =
        health_service_metric_accessible(metric_dist, start_last_week, end_last_week);
    
    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), " %d", current_steps);

        if (mask_steps_last_week & HealthServiceAccessibilityMaskAvailable) {
            steps_last_week = (int)health_service_sum(metric_steps, start_last_week, end_last_week);
            if (steps_last_week < current_steps) {
                steps_or_sleep_text[0] = '+';
            }
        }

        text_layer_set_text(steps_or_sleep, steps_or_sleep_text);
    }

    if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        current_dist = (int)health_service_sum_today(metric_dist);
        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), " %dm", current_dist);
        
        if (mask_dist_last_week & HealthServiceAccessibilityMaskAvailable) {
            dist_last_week = (int)health_service_sum(metric_dist, start_last_week, end_last_week);
            if (dist_last_week < current_dist) {
                dist_or_deep_text[0] = '+';
            }
        }

        text_layer_set_text(dist_or_deep, dist_or_deep_text);
    }
}

static void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, 
                "New HealthService HealthEventSignificantUpdate event");
            break;
        case HealthEventMovementUpdate:
            update_steps_data();
            break;
        case HealthEventSleepUpdate:
            APP_LOG(APP_LOG_LEVEL_INFO, 
                "New HealthService HealthEventSleepUpdate event");
            break;
    }
}

#endif

static void init(void) {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);


    watchface = window_create();

    window_set_window_handlers(watchface, (WindowHandlers) {
        .load = watchface_load,
        .unload = watchface_unload,
    });

    window_set_background_color(watchface, GColorBlack);
    battery_state_service_subscribe(battery_handler);

    window_stack_push(watchface, true);
    update_time();

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
    #if defined(PBL_HEALTH)
        has_health = health_service_events_subscribe(health_handler, NULL);
        if (has_health) {
            update_steps_data();
        }
    #else
        has_health = false;
    #endif

    battery_handler(battery_state_service_peek());
}

static void deinit(void) {
    window_destroy(watchface);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
