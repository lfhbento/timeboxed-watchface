#include <pebble.h>
#include "text.h"
#include "keys.h"
#include "configs.h"
#include "positions.h"

static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *bluetooth;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps;
static TextLayer *sleep;
static TextLayer *dist;
static TextLayer *cal;
static TextLayer *deep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static TextLayer *update;
static TextLayer *direction;
static TextLayer *speed;

static GFont time_font;
static GFont medium_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_big_font;
static GFont awesome_font;

static GColor base_color;
static GColor battery_color;
static GColor battery_low_color;
static GColor steps_color;
static GColor steps_behind_color;
static GColor dist_color;
static GColor dist_behind_color;

static char hour_text[13];
static char date_text[13];
static char bluetooth_text[4];
static char update_text[4];
static char battery_text[8];
static char alt_time_text[22];

static char temp_cur_text[8];
static char temp_max_text[8];
static char max_icon_text[4];
static char temp_min_text[8];
static char min_icon_text[4];
static char weather_text[4];
static char direction_text[4];
static char speed_text[8];

static char steps_text[16];
static char cal_text[16];
static char dist_text[16];
static char sleep_text[16];
static char deep_text[16];

static uint8_t loaded_font;
static bool enable_advanced;

uint8_t get_loaded_font() {
    return loaded_font;
}

void create_text_layers(Window* window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating text layers. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    int selected_font = persist_exists(KEY_FONTTYPE) ? persist_read_int(KEY_FONTTYPE) : BLOCKO_FONT;

    int alignment = PBL_IF_ROUND_ELSE(ALIGN_CENTER, persist_exists(KEY_TEXTALIGN) ? persist_read_int(KEY_TEXTALIGN) : ALIGN_RIGHT);
    int mode = is_simple_mode_enabled() ? MODE_SIMPLE : MODE_NORMAL;

    GTextAlignment text_align = GTextAlignmentRight;
    switch (alignment) {
        case ALIGN_LEFT:
            text_align = GTextAlignmentLeft;
            break;
        case ALIGN_CENTER:
            text_align = GTextAlignmentCenter;
            break;
        case ALIGN_RIGHT:
            text_align = GTextAlignmentRight;
            break;
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Alignment set: %d. %d%03d", text_align, (int)time(NULL), (int)time_ms(NULL, NULL));

    struct TextPositions text_positions;
    get_text_positions(selected_font, text_align, &text_positions);

    int width = bounds.size.w;
    int slot_width = is_simple_mode_enabled() ? width : 68;

    hours = text_layer_create(GRect(text_positions.hours.x, text_positions.hours.y, width, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_alignment(hours, text_align);

    date = text_layer_create(GRect(text_positions.date.x, text_positions.date.y, width, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_alignment(date, text_align);

    alt_time = text_layer_create(GRect(text_positions.alt_time.x, text_positions.alt_time.y, width, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_alignment(alt_time, text_align);

    battery = text_layer_create(GRect(text_positions.battery.x, text_positions.battery.y, width, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_alignment(battery, text_align);

    bluetooth = text_layer_create(GRect(text_positions.bluetooth.x, text_positions.bluetooth.y, width, 50));
    text_layer_set_background_color(bluetooth, GColorClear);
    text_layer_set_text_alignment(bluetooth, text_align == GTextAlignmentLeft ? GTextAlignmentRight : GTextAlignmentLeft);

    update = text_layer_create(GRect(text_positions.updates.x, text_positions.updates.y, width, 50));
    text_layer_set_background_color(update, GColorClear);
    text_layer_set_text_alignment(update, text_align == GTextAlignmentLeft ? GTextAlignmentRight : GTextAlignmentLeft);

    int weather_slot = get_slot_for_module(MODULE_WEATHER);
    GPoint weather_pos = get_pos_for_item(weather_slot, WEATHER_ITEM, mode, selected_font);
    weather = text_layer_create(GRect(weather_pos.x, weather_pos.y, width, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    GPoint temp_pos = get_pos_for_item(weather_slot, TEMP_ITEM, mode, selected_font);
    temp_cur = text_layer_create(GRect(temp_pos.x, temp_pos.y, width, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    int forecast_slot = get_slot_for_module(MODULE_FORECAST);
    GPoint min_pos = get_pos_for_item(forecast_slot, TEMPMIN_ITEM, mode, selected_font);
    temp_min = text_layer_create(GRect(min_pos.x, min_pos.y, width, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);

    min_icon = text_layer_create(GRect(min_pos.x - 10, min_pos.y - 3, width, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);

    GPoint max_pos = get_pos_for_item(forecast_slot, TEMPMAX_ITEM, mode, selected_font);
    temp_max = text_layer_create(GRect(max_pos.x, max_pos.y, width, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);

    max_icon = text_layer_create(GRect(max_pos.x - 10, max_pos.y - 3, width, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);

    int wind_slot = get_slot_for_module(MODULE_WIND);
    GPoint speed_pos = get_pos_for_item(wind_slot, SPEED_ITEM, mode, selected_font);
    speed = text_layer_create(GRect(speed_pos.x, speed_pos.y, width, 50));
    text_layer_set_background_color(speed, GColorClear);
    text_layer_set_text_alignment(speed, GTextAlignmentLeft);

    GPoint direction_pos = get_pos_for_item(wind_slot, DIRECTION_ITEM, mode, selected_font);
    direction = text_layer_create(GRect(direction_pos.x, direction_pos.y, width, 50));
    text_layer_set_background_color(direction, GColorClear);
    text_layer_set_text_alignment(direction, GTextAlignmentLeft);

    int steps_slot = get_slot_for_module(MODULE_STEPS);
    GPoint steps_pos = get_pos_for_item(steps_slot, STEPS_ITEM, mode, selected_font);
    steps = text_layer_create(GRect(steps_pos.x, steps_pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
    text_layer_set_background_color(steps, GColorClear);
    text_layer_set_text_alignment(steps, PBL_IF_ROUND_ELSE(
                GTextAlignmentCenter, is_simple_mode_enabled() ? text_align : (steps_slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));

    int dist_slot = get_slot_for_module(MODULE_DIST);
    GPoint dist_pos = get_pos_for_item(dist_slot, DIST_ITEM, mode, selected_font);
    dist = text_layer_create(GRect(dist_pos.x, dist_pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
    text_layer_set_background_color(dist, GColorClear);
    text_layer_set_text_alignment(dist, PBL_IF_ROUND_ELSE(
                GTextAlignmentCenter, is_simple_mode_enabled() ? text_align : (dist_slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));

    int cal_slot = get_slot_for_module(MODULE_CAL);
    GPoint cal_pos = get_pos_for_item(cal_slot, CAL_ITEM, mode, selected_font);
    cal = text_layer_create(GRect(cal_pos.x, cal_pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
    text_layer_set_background_color(cal, GColorClear);
    text_layer_set_text_alignment(cal, PBL_IF_ROUND_ELSE(
                GTextAlignmentCenter, is_simple_mode_enabled() ? text_align : (cal_slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));

    int sleep_slot = get_slot_for_module(MODULE_SLEEP);
    GPoint sleep_pos = get_pos_for_item(sleep_slot, SLEEP_ITEM, mode, selected_font);
    sleep = text_layer_create(GRect(sleep_pos.x, sleep_pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
    text_layer_set_background_color(sleep, GColorClear);
    text_layer_set_text_alignment(sleep, PBL_IF_ROUND_ELSE(
                GTextAlignmentCenter, is_simple_mode_enabled() ? text_align : (sleep_slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));

    int deep_slot = get_slot_for_module(MODULE_DEEP);
    GPoint deep_pos = get_pos_for_item(deep_slot, DEEP_ITEM, mode, selected_font);
    deep = text_layer_create(GRect(deep_pos.x, deep_pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
    text_layer_set_background_color(deep, GColorClear);
    text_layer_set_text_alignment(deep, PBL_IF_ROUND_ELSE(
                GTextAlignmentCenter, is_simple_mode_enabled() ? text_align : (deep_slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));

    layer_add_child(window_layer, text_layer_get_layer(hours));
    layer_add_child(window_layer, text_layer_get_layer(date));
    layer_add_child(window_layer, text_layer_get_layer(alt_time));
    layer_add_child(window_layer, text_layer_get_layer(battery));
    layer_add_child(window_layer, text_layer_get_layer(bluetooth));
    layer_add_child(window_layer, text_layer_get_layer(update));
    layer_add_child(window_layer, text_layer_get_layer(weather));
    layer_add_child(window_layer, text_layer_get_layer(min_icon));
    layer_add_child(window_layer, text_layer_get_layer(max_icon));
    layer_add_child(window_layer, text_layer_get_layer(temp_cur));
    layer_add_child(window_layer, text_layer_get_layer(temp_min));
    layer_add_child(window_layer, text_layer_get_layer(temp_max));
    layer_add_child(window_layer, text_layer_get_layer(steps));
    layer_add_child(window_layer, text_layer_get_layer(dist));
    layer_add_child(window_layer, text_layer_get_layer(cal));
    layer_add_child(window_layer, text_layer_get_layer(sleep));
    layer_add_child(window_layer, text_layer_get_layer(deep));
    layer_add_child(window_layer, text_layer_get_layer(speed));
    layer_add_child(window_layer, text_layer_get_layer(direction));

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text layers created. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void destroy_text_layers() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying text layers. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    text_layer_destroy(hours);
    text_layer_destroy(date);
    text_layer_destroy(alt_time);
    text_layer_destroy(battery);
    text_layer_destroy(bluetooth);
    text_layer_destroy(update);
    text_layer_destroy(weather);
    text_layer_destroy(min_icon);
    text_layer_destroy(max_icon);
    text_layer_destroy(temp_cur);
    text_layer_destroy(temp_min);
    text_layer_destroy(temp_max);
    text_layer_destroy(steps);
    text_layer_destroy(dist);
    text_layer_destroy(sleep);
    text_layer_destroy(cal);
    text_layer_destroy(deep);
    text_layer_destroy(speed);
    text_layer_destroy(direction);
}

void load_face_fonts() {
    int selected_font = persist_exists(KEY_FONTTYPE) ? persist_read_int(KEY_FONTTYPE) : BLOCKO_FONT;

    if (selected_font == SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading system fonts. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        medium_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
        base_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = SYSTEM_FONT;
    } else if (selected_font == ARCHIVO_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Archivo font. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_28));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_18));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = ARCHIVO_FONT;
    } else if (selected_font == DIN_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading DIN font. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_58));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_26));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_20));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = DIN_FONT;
    } else if (selected_font == PROTOTYPE_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Prototype font. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_48));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_22));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_16));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = PROTOTYPE_FONT;
    } else if (selected_font == BLOCKO_BIG_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font (big). %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_64));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_32));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_19));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = BLOCKO_BIG_FONT;
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = BLOCKO_FONT;
    }

    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
    awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ICONS_20));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Fonts loaded. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void unload_face_fonts() {
    if (loaded_font != SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading custom fonts. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        fonts_unload_custom_font(time_font);
        fonts_unload_custom_font(medium_font);
        fonts_unload_custom_font(base_font);
    }
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_big_font);
    fonts_unload_custom_font(awesome_font);
}

void set_face_fonts() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting fonts. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    text_layer_set_font(hours, time_font);
    text_layer_set_font(date, medium_font);
    text_layer_set_font(alt_time, base_font);
    text_layer_set_font(battery, base_font);
    text_layer_set_font(bluetooth, awesome_font);
    text_layer_set_font(update, awesome_font);
    text_layer_set_font(weather, weather_font);
    text_layer_set_font(min_icon, weather_big_font);
    text_layer_set_font(max_icon, weather_big_font);
    text_layer_set_font(temp_cur, base_font);
    text_layer_set_font(temp_min, base_font);
    text_layer_set_font(temp_max, base_font);
    text_layer_set_font(steps, base_font);
    text_layer_set_font(dist, base_font);
    text_layer_set_font(cal, base_font);
    text_layer_set_font(sleep, base_font);
    text_layer_set_font(deep, base_font);
    text_layer_set_font(speed, base_font);
    text_layer_set_font(direction, base_font);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Fonts set. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void set_colors(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Defining colors. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    enable_advanced = is_advanced_colors_enabled();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Advanced colors %d", enable_advanced);
    GColor min_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color;
    GColor max_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color;

    steps_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_STEPSCOLOR)) : base_color;
    steps_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_STEPSBEHINDCOLOR)) : base_color;
    dist_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color;
    dist_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DISTBEHINDCOLOR)) : base_color;

    text_layer_set_text_color(date,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);
    text_layer_set_text_color(alt_time,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_ALTHOURSCOLOR)) : base_color);
    text_layer_set_text_color(weather,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_WEATHERCOLOR)) : base_color);
    text_layer_set_text_color(temp_cur,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_TEMPCOLOR)) : base_color);
    text_layer_set_text_color(temp_min, min_color);
    text_layer_set_text_color(min_icon, min_color);
    text_layer_set_text_color(temp_max, max_color);
    text_layer_set_text_color(max_icon, max_color);

    text_layer_set_text_color(speed, base_color);
    text_layer_set_text_color(direction, base_color);

    battery_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color;
    battery_low_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color;

    window_set_background_color(window, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Defined colors. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void set_progress_color_steps(bool falling_behind) {
    text_layer_set_text_color(steps, falling_behind ? steps_behind_color : steps_color);
}

void set_progress_color_dist(bool falling_behind) {
    text_layer_set_text_color(dist, falling_behind ? steps_behind_color : steps_color);
}

void set_progress_color_cal(bool falling_behind) {
    text_layer_set_text_color(cal, falling_behind ? steps_behind_color : steps_color);
}

void set_progress_color_sleep(bool falling_behind) {
    text_layer_set_text_color(sleep, falling_behind ? steps_behind_color : steps_color);
}

void set_progress_color_deep(bool falling_behind) {
    text_layer_set_text_color(deep, falling_behind ? steps_behind_color : steps_color);
}

void set_bluetooth_color() {
    text_layer_set_text_color(bluetooth,
        enable_advanced && persist_exists(KEY_BLUETOOTHCOLOR) ? GColorFromHEX(persist_read_int(KEY_BLUETOOTHCOLOR)) : base_color);
}

void set_update_color() {
    text_layer_set_text_color(update,
        enable_advanced && persist_exists(KEY_UPDATECOLOR) ? GColorFromHEX(persist_read_int(KEY_UPDATECOLOR)) : base_color);
}

void set_battery_color(int percentage) {
    if (percentage > 10) {
        text_layer_set_text_color(battery, battery_color);
    } else {
        text_layer_set_text_color(battery, battery_low_color);
    }
}

void set_hours_layer_text(char* text) {
    strcpy(hour_text, text);
    text_layer_set_text(hours, hour_text);
}

void set_date_layer_text(char* text) {
    strcpy(date_text, text);
    text_layer_set_text(date, date_text);
}

void set_alt_time_layer_text(char* text) {
    strcpy(alt_time_text, text);
    text_layer_set_text(alt_time, alt_time_text);
}

void set_battery_layer_text(char* text) {
    strcpy(battery_text, text);
    text_layer_set_text(battery, battery_text);
}

void set_bluetooth_layer_text(char* text) {
    strcpy(bluetooth_text, text);
    text_layer_set_text(bluetooth, bluetooth_text);
}

void set_temp_cur_layer_text(char* text) {
    strcpy(temp_cur_text, text);
    text_layer_set_text(temp_cur, temp_cur_text);
}

void set_temp_max_layer_text(char* text) {
    strcpy(temp_max_text, text);
    text_layer_set_text(temp_max, temp_max_text);
}

void set_temp_min_layer_text(char* text) {
    strcpy(temp_min_text, text);
    text_layer_set_text(temp_min, temp_min_text);
}

void set_steps_layer_text(char* text) {
    strcpy(steps_text, text);
    text_layer_set_text(steps, steps_text);
}

void set_dist_layer_text(char* text) {
    strcpy(dist_text, text);
    text_layer_set_text(dist, dist_text);
}

void set_cal_layer_text(char* text) {
    strcpy(cal_text, text);
    text_layer_set_text(cal, cal_text);
}

void set_sleep_layer_text(char* text) {
    strcpy(sleep_text, text);
    text_layer_set_text(sleep, sleep_text);
}

void set_deep_layer_text(char* text) {
    strcpy(deep_text, text);
    text_layer_set_text(deep, deep_text);
}

void set_weather_layer_text(char* text) {
    strcpy(weather_text, text);
    text_layer_set_text(weather, weather_text);
}

void set_max_icon_layer_text(char* text) {
    strcpy(max_icon_text, text);
    text_layer_set_text(max_icon, max_icon_text);
}

void set_min_icon_layer_text(char* text) {
    strcpy(min_icon_text, text);
    text_layer_set_text(min_icon, min_icon_text);
}

void set_update_layer_text(char* text) {
    strcpy(update_text, text);
    text_layer_set_text(update, update_text);
}

void set_wind_speed_layer_text(char* text) {
    strcpy(speed_text, text);
    text_layer_set_text(speed, speed_text);
}

void set_wind_direction_layer_text(char* text) {
    strcpy(direction_text, text);
    text_layer_set_text(direction, direction_text);
}
