#include <pebble.h>
#include "text.h"
#include "keys.h"

static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *bluetooth;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps_or_sleep;
static TextLayer *dist_or_deep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static TextLayer *update;

static GFont time_font;
static GFont medium_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_big_font;
static GFont awesome_font;

static char hour_text[13];
static char date_text[13];
static char temp_cur_text[8];
static char temp_max_text[8];
static char max_icon_text[4];
static char temp_min_text[8];
static char min_icon_text[4];
static char weather_text[4];
static char bluetooth_text[4];
static char update_text[4];
static char battery_text[8];
static char alt_time_text[22];
static char steps_or_sleep_text[16];
static char dist_or_deep_text[16];

static uint8_t loaded_font;

void create_text_layers(Window* window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating text layers. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    int selected_font = BLOCKO_FONT;
    
    if (persist_exists(KEY_FONTTYPE)) {
        selected_font = persist_read_int(KEY_FONTTYPE);
    }
    int width = bounds.size.w;

    int hours_top;
    int date_left;
    int date_top;
    int alt_top;
    int battery_top;
    int bt_top;
    int temp_cur_top;
    int temp_min_max_top;
    int temp_icon_min_max_top;
    int update_top;

    if (selected_font == BLOCKO_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(46, 38);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(98, 90);
        alt_top = PBL_IF_ROUND_ELSE(46, 38);
        battery_top = PBL_IF_ROUND_ELSE(120, 112);
        bt_top = PBL_IF_ROUND_ELSE(70, 60);
        update_top = PBL_IF_ROUND_ELSE(88, 78);
        temp_cur_top = PBL_IF_ROUND_ELSE(4, 4);
        temp_min_max_top = PBL_IF_ROUND_ELSE(22, 4);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(20, 0);
    } else if (selected_font == BLOCKO_BIG_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(40, 32);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(96, 88);
        alt_top = PBL_IF_ROUND_ELSE(42, 34);
        battery_top = PBL_IF_ROUND_ELSE(124, 116);
        bt_top = PBL_IF_ROUND_ELSE(64, 54);
        update_top = PBL_IF_ROUND_ELSE(86, 76);
        temp_cur_top = PBL_IF_ROUND_ELSE(4, 3);
        temp_min_max_top = PBL_IF_ROUND_ELSE(22, 3);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    } else if (selected_font == ARCHIVO_FONT) {
        hours_top = PBL_IF_ROUND_ELSE(48, 40);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(100, 92);
        alt_top = PBL_IF_ROUND_ELSE(44, 34);
        battery_top = PBL_IF_ROUND_ELSE(126, 118);
        bt_top = PBL_IF_ROUND_ELSE(68, 56);
        update_top = PBL_IF_ROUND_ELSE(86, 74);
        temp_cur_top = PBL_IF_ROUND_ELSE(2, 2);
        temp_min_max_top = PBL_IF_ROUND_ELSE(24, 2);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    } else {
        hours_top = PBL_IF_ROUND_ELSE(54, 42);
        date_left = PBL_IF_ROUND_ELSE(0, -2);
        date_top = PBL_IF_ROUND_ELSE(98, 86);
        alt_top = PBL_IF_ROUND_ELSE(46, 34);
        battery_top = PBL_IF_ROUND_ELSE(124, 112);
        bt_top = PBL_IF_ROUND_ELSE(68, 56);
        update_top = PBL_IF_ROUND_ELSE(86, 74);
        temp_cur_top = PBL_IF_ROUND_ELSE(2, 2);
        temp_min_max_top = PBL_IF_ROUND_ELSE(20, 2);
        temp_icon_min_max_top = PBL_IF_ROUND_ELSE(18, -2);
    }

    hours = text_layer_create(GRect(0, hours_top, width, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_alignment(hours, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));

    date = text_layer_create(GRect(date_left, date_top, width, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_alignment(date, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
    alt_time = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -2), alt_top, width, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));

    battery = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -4), battery_top, width, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
    bluetooth = text_layer_create(GRect(PBL_IF_ROUND_ELSE(2, 0), bt_top, width, 50));
    text_layer_set_background_color(bluetooth, GColorClear);
    text_layer_set_text_alignment(bluetooth, GTextAlignmentLeft);

    update = text_layer_create(GRect(PBL_IF_ROUND_ELSE(2, 0), update_top, width, 50));
    text_layer_set_background_color(update, GColorClear);
    text_layer_set_text_alignment(update, GTextAlignmentLeft);

    weather = text_layer_create(GRect(PBL_IF_ROUND_ELSE(-14, 4), 0, width, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_cur = text_layer_create(GRect(PBL_IF_ROUND_ELSE(16, 38), temp_cur_top, width, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_min = text_layer_create(GRect(PBL_IF_ROUND_ELSE(70, 80), temp_min_max_top, width, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);

    min_icon = text_layer_create(GRect(PBL_IF_ROUND_ELSE(60, 70), temp_icon_min_max_top, width, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);

    temp_max = text_layer_create(GRect(PBL_IF_ROUND_ELSE(105, 113), temp_min_max_top, width, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);

    max_icon = text_layer_create(GRect(PBL_IF_ROUND_ELSE(95, 103), temp_icon_min_max_top, width, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);

    steps_or_sleep = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148), width, 50));
    text_layer_set_background_color(steps_or_sleep, GColorClear);
    text_layer_set_text_alignment(steps_or_sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    dist_or_deep = text_layer_create(GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148), width, 50));
    text_layer_set_background_color(dist_or_deep, GColorClear);
    text_layer_set_text_alignment(dist_or_deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    
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
    layer_add_child(window_layer, text_layer_get_layer(steps_or_sleep));
    layer_add_child(window_layer, text_layer_get_layer(dist_or_deep));
    
}

void destroy_text_layers() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying text layers. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    text_layer_destroy(steps_or_sleep);
    text_layer_destroy(dist_or_deep);
}

void load_face_fonts() {
    int selected_font = BLOCKO_FONT;
    
    if (persist_exists(KEY_FONTTYPE)) {
        selected_font = persist_read_int(KEY_FONTTYPE);
    }

    if (selected_font == SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading system fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        medium_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
        base_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = SYSTEM_FONT;
    } else if (selected_font == ARCHIVO_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Archivo font. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_28));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_18));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = ARCHIVO_FONT;
    } else if (selected_font == BLOCKO_BIG_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font (big). %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_64));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_32));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_19));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = BLOCKO_BIG_FONT;
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = BLOCKO_FONT;
    }
    
    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
    awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AWESOME_18));
}

void unload_face_fonts() {
    if (loaded_font != SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading custom fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        fonts_unload_custom_font(time_font);
        fonts_unload_custom_font(medium_font);
        fonts_unload_custom_font(base_font);
    }
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_big_font);
    fonts_unload_custom_font(awesome_font);
}

void set_face_fonts() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting fonts. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    text_layer_set_font(steps_or_sleep, base_font);
    text_layer_set_font(dist_or_deep, base_font);
}

void set_colors(Window *window) {
    GColor base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    bool enableAdvanced = persist_exists(KEY_ENABLEADVANCED) ? persist_read_int(KEY_ENABLEADVANCED) : false;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Advanced colors %d", enableAdvanced);
    text_layer_set_text_color(date, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);
    text_layer_set_text_color(alt_time, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_ALTHOURSCOLOR)) : base_color);
    text_layer_set_text_color(weather, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_WEATHERCOLOR)) : base_color);
    text_layer_set_text_color(temp_cur, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_TEMPCOLOR)) : base_color);
    text_layer_set_text_color(temp_min, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(min_icon, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color);
    text_layer_set_text_color(temp_max, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(max_icon, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color);
    text_layer_set_text_color(steps_or_sleep, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_STEPSCOLOR)) : base_color);
    text_layer_set_text_color(dist_or_deep, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color);

    BatteryChargeState charge_state = battery_state_service_peek();
    if (charge_state.charge_percent > 20) {
        text_layer_set_text_color(battery, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color);
    } else {
        text_layer_set_text_color(battery, 
            enableAdvanced ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color);
    }
    
    text_layer_set_text_color(bluetooth, 
        enableAdvanced && persist_exists(KEY_BLUETOOTHCOLOR) ? GColorFromHEX(persist_read_int(KEY_BLUETOOTHCOLOR)) : base_color);

    text_layer_set_text_color(update, 
        enableAdvanced && persist_exists(KEY_UPDATECOLOR) ? GColorFromHEX(persist_read_int(KEY_UPDATECOLOR)) : base_color);

    window_set_background_color(window, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Defined colors. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
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

void set_steps_or_sleep_layer_text(char* text) {
    strcpy(steps_or_sleep_text, text);
    text_layer_set_text(steps_or_sleep, steps_or_sleep_text);
}

void set_dist_or_deep_layer_text(char* text) {
    strcpy(dist_or_deep_text, text);
    text_layer_set_text(dist_or_deep, dist_or_deep_text);
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
