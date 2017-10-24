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

#if defined(PBL_HEALTH)
static TextLayer *steps;
static TextLayer *sleep;
static TextLayer *dist;
static TextLayer *cal;
static TextLayer *deep;
static TextLayer *active;
static TextLayer *heart;
static TextLayer *heart_icon;
#endif

static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static TextLayer *update;
static TextLayer *direction;
static TextLayer *speed;
static TextLayer *wind_unit;
static TextLayer *sunrise;
static TextLayer *sunrise_icon;
static TextLayer *sunset;
static TextLayer *sunset_icon;
static TextLayer *compass;
static TextLayer *degrees;
static TextLayer *seconds;
static TextLayer *crypto;

#if !defined PBL_PLATFORM_APLITE
static TextLayer *alt_time_b;
static TextLayer *crypto_b;
static TextLayer *crypto_c;
static TextLayer *crypto_d;
#endif

static GFont time_font;
static GFont medium_font;
static GFont base_font;
static GFont weather_font;
static GFont weather_font_small;
static GFont custom_font;

static GColor base_color;
static GColor battery_color;
static GColor battery_low_color;

#if defined(PBL_HEALTH)
static GColor steps_color;
static GColor steps_behind_color;
static GColor dist_color;
static GColor dist_behind_color;
static GColor cal_color;
static GColor cal_behind_color;
static GColor sleep_color;
static GColor sleep_behind_color;
static GColor deep_color;
static GColor deep_behind_color;
static GColor active_color;
static GColor active_behind_color;
static GColor heart_color;
static GColor heart_color_off;
#endif

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
static char wind_unit_text[2];
static char sunrise_text[8];
static char sunrise_icon_text[4];
static char sunset_text[8];
static char sunset_icon_text[4];
static char compass_text[4];
static char degrees_text[8];
static char seconds_text[4];
static char crypto_text[8];

#if !defined PBL_PLATFORM_APLITE
static char alt_time_b_text[22];
static char crypto_b_text[8];
static char crypto_c_text[8];
static char crypto_d_text[8];
#endif

#if defined(PBL_HEALTH)
static char steps_text[16];
static char cal_text[16];
static char dist_text[16];
static char sleep_text[16];
static char deep_text[16];
static char active_text[16];
static char heart_text[16];
static char heart_icon_text[4];
static uint8_t heart_low;
static uint8_t heart_high;
#endif

static uint8_t loaded_font;
static bool enable_advanced;

int to_upper_case(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    } else {
        return c;
    }
}

uint8_t get_loaded_font() {
    return loaded_font;
}

static void add_text_layer(Layer * window, TextLayer * text) {
    if (text) {
        layer_add_child(window, text_layer_get_layer(text));
    }
}

static void delete_text_layer(TextLayer * text) {
    if (text) {
        text_layer_destroy(text);
    }
}

static void set_text_font(TextLayer * text, GFont font) {
    if (text) {
        text_layer_set_font(text, font);
    }
}

static void set_text_color(TextLayer * text, GColor color) {
    if (text) {
        text_layer_set_text_color(text, color);
    }
}

static void set_text(TextLayer * text, char * content) {
    if (text) {
        text_layer_set_text(text, content);
    }
}

void create_text_layers(Window* window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect full_bounds = layer_get_bounds(window_layer);
    GRect bounds = layer_get_unobstructed_bounds(window_layer);

    int selected_font = persist_exists(KEY_FONTTYPE) ? persist_read_int(KEY_FONTTYPE) : LECO_FONT;

    int alignment = PBL_IF_ROUND_ELSE(ALIGN_CENTER, persist_exists(KEY_TEXTALIGN) ? persist_read_int(KEY_TEXTALIGN) : ALIGN_RIGHT);
    int mode = is_simple_mode_enabled() ? MODE_SIMPLE : MODE_NORMAL;

    int width = bounds.size.w - 4;
    int height = bounds.size.h;
    int full_height = full_bounds.size.h;

    #if !defined PBL_PLATFORM_APLITE && !defined PBL_PLATFORM_CHALK
    if (is_quickview_disabled()) {
        height = full_height;
    }
    #endif

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

    GPoint pos;
    int slot;

    struct TextPositions text_positions;
    get_text_positions(selected_font, text_align, &text_positions, width, height);

    int slot_width = is_simple_mode_enabled() ? width : width/2;

    hours = text_layer_create(GRect(text_positions.hours.x, text_positions.hours.y, width, 100));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_alignment(hours, text_align);

    date = text_layer_create(GRect(text_positions.date.x, text_positions.date.y, width, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_alignment(date, text_align);

    slot = get_slot_for_module(MODULE_TIMEZONE);
    if (slot != -1) {
        pos = get_pos_for_item(slot, TIMEZONE_ITEM, mode, selected_font, width, height);
        alt_time = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(alt_time, GColorClear);
        text_layer_set_text_alignment(alt_time, text_align);
        text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    #if !defined PBL_PLATFORM_APLITE
    slot = get_slot_for_module(MODULE_TIMEZONEB);
    if (slot != -1) {
        pos = get_pos_for_item(slot, TIMEZONEB_ITEM, mode, selected_font, width, height);
        alt_time_b = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(alt_time_b, GColorClear);
        text_layer_set_text_alignment(alt_time_b, text_align);
        text_layer_set_text_alignment(alt_time_b, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }
    #endif

    slot = get_slot_for_module(MODULE_BATTERY);
    if (slot != -1) {
        pos = get_pos_for_item(slot, BATTERY_ITEM, mode, selected_font, width, height);
        battery = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(battery, GColorClear);
        text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    bluetooth = text_layer_create(GRect(text_positions.bluetooth.x, text_positions.bluetooth.y, width, 50));
    text_layer_set_background_color(bluetooth, GColorClear);
    text_layer_set_text_alignment(bluetooth, text_align == GTextAlignmentLeft ? GTextAlignmentRight : GTextAlignmentLeft);
    layer_set_hidden(text_layer_get_layer(bluetooth), height != full_height);

    update = text_layer_create(GRect(text_positions.updates.x, text_positions.updates.y, width, 50));
    text_layer_set_background_color(update, GColorClear);
    text_layer_set_text_alignment(update, text_align == GTextAlignmentLeft ? GTextAlignmentRight : GTextAlignmentLeft);
    layer_set_hidden(text_layer_get_layer(update), height != full_height);


    slot = get_slot_for_module(MODULE_WEATHER);
    if (slot != -1) {
        pos = get_pos_for_item(slot, WEATHER_ITEM, mode, selected_font, width, height);
        weather = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, 40), 50));
        text_layer_set_background_color(weather, GColorClear);
        text_layer_set_text_alignment(weather, GTextAlignmentCenter);

        pos = get_pos_for_item(slot, TEMP_ITEM, mode, selected_font, width, height);
        temp_cur = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(temp_cur, GColorClear);
        text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    }

    slot = get_slot_for_module(MODULE_FORECAST);
    if (slot != -1) {
        pos = get_pos_for_item(slot, TEMPMAX_ITEM, mode, selected_font, width, height);
        temp_max = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(temp_max, GColorClear);
        text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, TEMPMAXICON_ITEM, mode, selected_font, width, height);
        max_icon = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(max_icon, GColorClear);
        text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, TEMPMIN_ITEM, mode, selected_font, width, height);
        temp_min = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(temp_min, GColorClear);
        text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, TEMPMINICON_ITEM, mode, selected_font, width, height);
        min_icon = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(min_icon, GColorClear);
        text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);
    }

    slot = get_slot_for_module(MODULE_WIND);
    if (slot != -1) {
        pos = get_pos_for_item(slot, SPEED_ITEM, mode, selected_font, width, height);
        speed = text_layer_create(GRect(pos.x, pos.y, 42, 50));
        text_layer_set_background_color(speed, GColorClear);
        text_layer_set_text_alignment(speed, GTextAlignmentRight);

        pos = get_pos_for_item(slot, DIRECTION_ITEM, mode, selected_font, width, height);
        direction = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(direction, GColorClear);
        text_layer_set_text_alignment(direction, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, WIND_UNIT_ITEM, mode, selected_font, width, height);
        wind_unit = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(wind_unit, GColorClear);
        text_layer_set_text_alignment(wind_unit, GTextAlignmentLeft);
    }

    slot = get_slot_for_module(MODULE_SUNRISE);
    if (slot != -1) {
        pos = get_pos_for_item(slot, SUNRISE_ITEM, mode, selected_font, width, height);
        sunrise = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
        text_layer_set_background_color(sunrise, GColorClear);
        text_layer_set_text_alignment(sunrise, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, SUNRISEICON_ITEM, mode, selected_font, width, height);
        sunrise_icon = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, 34), 50));
        text_layer_set_background_color(sunrise_icon, GColorClear);
        text_layer_set_text_alignment(sunrise_icon, GTextAlignmentLeft);
    }

    slot = get_slot_for_module(MODULE_SUNSET);
    if (slot != -1) {
        pos = get_pos_for_item(slot, SUNSET_ITEM, mode, selected_font, width, height);
        sunset = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
        text_layer_set_background_color(sunset, GColorClear);
        text_layer_set_text_alignment(sunset, GTextAlignmentRight);

        pos = get_pos_for_item(slot, SUNSETICON_ITEM, mode, selected_font, width, height);
        sunset_icon = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
        text_layer_set_background_color(sunset_icon, GColorClear);
        text_layer_set_text_alignment(sunset_icon, GTextAlignmentRight);
    }

    slot = get_slot_for_module(MODULE_COMPASS);
    if (slot != -1) {
        pos = get_pos_for_item(slot, DEGREES_ITEM, mode, selected_font, width, height);
        degrees = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(degrees, GColorClear);
        text_layer_set_text_alignment(degrees, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, COMPASS_ITEM, mode, selected_font, width, height);
        compass = text_layer_create(GRect(pos.x, pos.y, width, 50));
        text_layer_set_background_color(compass, GColorClear);
        text_layer_set_text_alignment(compass, GTextAlignmentLeft);
    }

    slot = get_slot_for_module(MODULE_SECONDS);
    if (slot != -1) {

        pos = get_pos_for_item(slot, SECONDS_ITEM, mode, selected_font, width, height);
        seconds = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(seconds, GColorClear);
        text_layer_set_text_alignment(seconds, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_CRYPTO);
    if (slot != -1) {
        pos = get_pos_for_item(slot, CRYPTO_ITEM, mode, selected_font, width, height);
        crypto = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(crypto, GColorClear);
        text_layer_set_text_alignment(crypto, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    #if !defined PBL_PLATFORM_APLITE
    slot = get_slot_for_module(MODULE_CRYPTOB);
    if (slot != -1) {
        pos = get_pos_for_item(slot, CRYPTO_ITEM, mode, selected_font, width, height);
        crypto_b = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(crypto_b, GColorClear);
        text_layer_set_text_alignment(crypto_b, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_CRYPTOC);
    if (slot != -1) {
        pos = get_pos_for_item(slot, CRYPTO_ITEM, mode, selected_font, width, height);
        crypto_c = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(crypto_c, GColorClear);
        text_layer_set_text_alignment(crypto_c, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_CRYPTOD);
    if (slot != -1) {
        pos = get_pos_for_item(slot, CRYPTO_ITEM, mode, selected_font, width, height);
        crypto_d = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(crypto_d, GColorClear);
        text_layer_set_text_alignment(crypto_d, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }
    #endif

    #if defined(PBL_HEALTH)
    slot = get_slot_for_module(MODULE_STEPS);
    if (slot != -1) {
        pos = get_pos_for_item(slot, STEPS_ITEM, mode, selected_font, width, height);
        steps = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width: slot_width), 50));
        text_layer_set_background_color(steps, GColorClear);
        text_layer_set_text_alignment(steps, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_DIST);
    if (slot != -1) {
        pos = get_pos_for_item(slot, DIST_ITEM, mode, selected_font, width, height);
        dist = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(dist, GColorClear);
        text_layer_set_text_alignment(dist, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_CAL);
    if (slot != -1) {
        pos = get_pos_for_item(slot, CAL_ITEM, mode, selected_font, width, height);
        cal = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(cal, GColorClear);
        text_layer_set_text_alignment(cal, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_SLEEP);
    if (slot != -1) {
        pos = get_pos_for_item(slot, SLEEP_ITEM, mode, selected_font, width, height);
        sleep = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(sleep, GColorClear);
        text_layer_set_text_alignment(sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_DEEP);
    if (slot != -1) {
        pos = get_pos_for_item(slot, DEEP_ITEM, mode, selected_font, width, height);
        deep = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(deep, GColorClear);
        text_layer_set_text_alignment(deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_ACTIVE);
    if (slot != -1) {
        pos = get_pos_for_item(slot, ACTIVE_ITEM, mode, selected_font, width, height);
        active = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot > 3 ? width : slot_width), 50));
        text_layer_set_background_color(active, GColorClear);
        text_layer_set_text_alignment(active, PBL_IF_ROUND_ELSE(GTextAlignmentCenter,
                    is_simple_mode_enabled() || slot > 3 ? text_align : (slot % 2 == 0 ? GTextAlignmentLeft : GTextAlignmentRight)));
    }

    slot = get_slot_for_module(MODULE_HEART);
    if (slot != -1) {
        pos = get_pos_for_item(slot, HEART_ITEM, mode, selected_font, width, height);
        heart = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, slot_width), 50));
        text_layer_set_background_color(heart, GColorClear);
        text_layer_set_text_alignment(heart, GTextAlignmentLeft);

        pos = get_pos_for_item(slot, HEARTICON_ITEM, mode, selected_font, width, height);
        heart_icon = text_layer_create(GRect(pos.x, pos.y, PBL_IF_ROUND_ELSE(width, 34), 50));
        text_layer_set_background_color(heart_icon, GColorClear);
        text_layer_set_text_alignment(heart_icon, GTextAlignmentLeft);
    }
    #endif

    add_text_layer(window_layer, hours);
    add_text_layer(window_layer, date);
    add_text_layer(window_layer, alt_time);
    add_text_layer(window_layer, battery);
    add_text_layer(window_layer, bluetooth);
    add_text_layer(window_layer, update);
    add_text_layer(window_layer, weather);
    add_text_layer(window_layer, min_icon);
    add_text_layer(window_layer, max_icon);
    add_text_layer(window_layer, temp_cur);
    add_text_layer(window_layer, temp_min);
    add_text_layer(window_layer, temp_max);
    add_text_layer(window_layer, speed);
    add_text_layer(window_layer, direction);
    add_text_layer(window_layer, wind_unit);
    add_text_layer(window_layer, sunrise);
    add_text_layer(window_layer, sunrise_icon);
    add_text_layer(window_layer, sunset);
    add_text_layer(window_layer, sunset_icon);
    add_text_layer(window_layer, compass);
    add_text_layer(window_layer, degrees);
    add_text_layer(window_layer, seconds);
    add_text_layer(window_layer, crypto);
    #if !defined PBL_PLATFORM_APLITE
    add_text_layer(window_layer, alt_time_b);
    add_text_layer(window_layer, crypto_b);
    add_text_layer(window_layer, crypto_c);
    add_text_layer(window_layer, crypto_d);
    #endif

    #if defined(PBL_HEALTH)
    add_text_layer(window_layer, steps);
    add_text_layer(window_layer, dist);
    add_text_layer(window_layer, cal);
    add_text_layer(window_layer, sleep);
    add_text_layer(window_layer, deep);
    add_text_layer(window_layer, active);
    add_text_layer(window_layer, heart);
    add_text_layer(window_layer, heart_icon);
    #endif
}

void destroy_text_layers() {
    delete_text_layer(hours);
    hours = NULL;
    delete_text_layer(date);
    date = NULL;
    delete_text_layer(alt_time);
    alt_time = NULL;
    delete_text_layer(battery);
    battery = NULL;
    delete_text_layer(bluetooth);
    bluetooth = NULL;
    delete_text_layer(update);
    update = NULL;
    delete_text_layer(weather);
    weather = NULL;
    delete_text_layer(min_icon);
    min_icon = NULL;
    delete_text_layer(max_icon);
    max_icon = NULL;
    delete_text_layer(temp_cur);
    temp_cur = NULL;
    delete_text_layer(temp_min);
    temp_min = NULL;
    delete_text_layer(temp_max);
    temp_max = NULL;
    delete_text_layer(speed);
    speed = NULL;
    delete_text_layer(direction);
    direction = NULL;
    delete_text_layer(wind_unit);
    wind_unit = NULL;
    delete_text_layer(sunrise);
    sunrise = NULL;
    delete_text_layer(sunrise_icon);
    sunrise_icon = NULL;
    delete_text_layer(sunset);
    sunset = NULL;
    delete_text_layer(sunset_icon);
    sunset_icon = NULL;
    delete_text_layer(compass);
    compass = NULL;
    delete_text_layer(degrees);
    degrees = NULL;
    delete_text_layer(seconds);
    seconds = NULL;
    delete_text_layer(crypto);
    crypto = NULL;

    #if !defined PBL_PLATFORM_APLITE
    delete_text_layer(alt_time_b);
    alt_time_b = NULL;
    delete_text_layer(crypto_b);
    crypto_b = NULL;
    delete_text_layer(crypto_c);
    crypto_c = NULL;
    delete_text_layer(crypto_d);
    crypto_d = NULL;
    #endif

    #if defined(PBL_HEALTH)
    delete_text_layer(steps);
    steps = NULL;
    delete_text_layer(dist);
    dist = NULL;
    delete_text_layer(sleep);
    sleep = NULL;
    delete_text_layer(cal);
    cal = NULL;
    delete_text_layer(deep);
    deep = NULL;
    delete_text_layer(active);
    active = NULL;
    delete_text_layer(heart);
    heart = NULL;
    delete_text_layer(heart_icon);
    heart_icon = NULL;
    #endif
}

void load_face_fonts() {
    int selected_font = persist_exists(KEY_FONTTYPE) ? persist_read_int(KEY_FONTTYPE) : LECO_FONT;

    if (selected_font == SYSTEM_FONT) {
        time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        medium_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
        base_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        loaded_font = SYSTEM_FONT;
    } else if (selected_font == ARCHIVO_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_28));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_18));
        loaded_font = ARCHIVO_FONT;
    } else if (selected_font == DIN_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_58));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_26));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_20));
        loaded_font = DIN_FONT;
    } else if (selected_font == PROTOTYPE_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_48));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_22));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PROTOTYPE_16));
        loaded_font = PROTOTYPE_FONT;
    } else if (selected_font == BLOCKO_BIG_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_64));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_32));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_19));
        loaded_font = BLOCKO_BIG_FONT;
    } else if (selected_font == LECO_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_47));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_21));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_14));
        loaded_font = LECO_FONT;
    } else if (selected_font == KONSTRUCT_FONT) {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KONSTRUCT_33));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KONSTRUCT_17));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KONSTRUCT_11));
        loaded_font = KONSTRUCT_FONT;
    } else {
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
        loaded_font = BLOCKO_FONT;
    }

    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
    weather_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_16));
    custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ICONS_20));
}

void unload_face_fonts() {
    if (loaded_font != SYSTEM_FONT) {
        fonts_unload_custom_font(time_font);
        fonts_unload_custom_font(medium_font);
        fonts_unload_custom_font(base_font);
    }
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_font_small);
    fonts_unload_custom_font(custom_font);
}

void set_face_fonts() {
    set_text_font(hours, time_font);
    set_text_font(date, medium_font);
    set_text_font(alt_time, base_font);
    set_text_font(battery, base_font);
    set_text_font(bluetooth, custom_font);
    set_text_font(update, custom_font);
    set_text_font(weather, weather_font);
    set_text_font(min_icon, custom_font);
    set_text_font(max_icon, custom_font);
    set_text_font(temp_cur, base_font);
    set_text_font(temp_min, base_font);
    set_text_font(temp_max, base_font);
    set_text_font(speed, base_font);
    set_text_font(direction, custom_font);
    set_text_font(wind_unit, custom_font);
    set_text_font(sunrise, base_font);
    set_text_font(sunrise_icon, weather_font_small);
    set_text_font(sunset, base_font);
    set_text_font(sunset_icon, weather_font_small);
    set_text_font(compass, custom_font);
    set_text_font(degrees, base_font);
    set_text_font(seconds, base_font);
    set_text_font(crypto, base_font);

    #if !defined PBL_PLATFORM_APLITE
    set_text_font(alt_time_b, base_font);
    set_text_font(crypto_b, base_font);
    set_text_font(crypto_c, base_font);
    set_text_font(crypto_d, base_font);
    #endif

    #if defined(PBL_HEALTH)
    set_text_font(steps, base_font);
    set_text_font(dist, base_font);
    set_text_font(cal, base_font);
    set_text_font(sleep, base_font);
    set_text_font(deep, base_font);
    set_text_font(active, base_font);
    set_text_font(heart, base_font);
    set_text_font(heart_icon, custom_font);
    #endif

}

void set_colors(Window *window) {
    base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    enable_advanced = is_advanced_colors_enabled();

    text_layer_set_text_color(date,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_DATECOLOR)) : base_color);

    if (is_module_enabled(MODULE_TIMEZONE)) {
        set_text_color(alt_time,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_ALTHOURSCOLOR)) : base_color);
    }

    #if !defined PBL_PLATFORM_APLITE
    if (is_module_enabled(MODULE_TIMEZONEB)) {
        set_text_color(alt_time_b,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_ALTHOURSBCOLOR)) : base_color);
    }
    #endif

    if (is_module_enabled(MODULE_BATTERY)) {
        battery_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color;
        battery_low_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color;
    }

    window_set_background_color(window, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);

    #if defined(PBL_HEALTH)
    if (is_module_enabled(MODULE_STEPS)) {
        steps_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_STEPSCOLOR)) : base_color;
        steps_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_STEPSBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_DIST)) {
        dist_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color;
        dist_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DISTBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_CAL)) {
        cal_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_CALCOLOR)) : base_color;
        cal_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_CALBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_SLEEP)) {
        sleep_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_SLEEPCOLOR)) : base_color;
        sleep_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_SLEEPBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_DEEP)) {
        deep_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DEEPCOLOR)) : base_color;
        deep_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_DEEPBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_ACTIVE)) {
        active_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_ACTIVECOLOR)) : base_color;
        active_behind_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_ACTIVEBEHINDCOLOR)) : base_color;
    }
    if (is_module_enabled(MODULE_HEART)) {
        heart_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_HEARTCOLOR)) : base_color;
        heart_color_off = enable_advanced ? GColorFromHEX(persist_read_int(KEY_HEARTCOLOROFF)) : base_color;
        heart_low = persist_exists(KEY_HEARTLOW) ? persist_read_int(KEY_HEARTLOW) : 0;
        heart_high = persist_exists(KEY_HEARTHIGH) ? persist_read_int(KEY_HEARTHIGH) : 0;
    }
    #endif

    if (is_module_enabled(MODULE_WEATHER)) {
        set_text_color(weather,
                enable_advanced ? GColorFromHEX(persist_read_int(KEY_WEATHERCOLOR)) : base_color);
        set_text_color(temp_cur,
                enable_advanced ? GColorFromHEX(persist_read_int(KEY_TEMPCOLOR)) : base_color);
    }

    if (is_module_enabled(MODULE_FORECAST)) {
        GColor min_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_MINCOLOR)) : base_color;
        GColor max_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_MAXCOLOR)) : base_color;
        set_text_color(temp_min, min_color);
        set_text_color(min_icon, min_color);
        set_text_color(temp_max, max_color);
        set_text_color(max_icon, max_color);
    }

    if (is_module_enabled(MODULE_WIND)) {
        set_text_color(speed, enable_advanced ? GColorFromHEX(persist_read_int(KEY_WINDSPEEDCOLOR)) : base_color);
        set_text_color(wind_unit, enable_advanced ? GColorFromHEX(persist_read_int(KEY_WINDSPEEDCOLOR)) : base_color);
        set_text_color(direction, enable_advanced ? GColorFromHEX(persist_read_int(KEY_WINDDIRCOLOR)) : base_color);
    }

    if (is_module_enabled(MODULE_COMPASS)) {
        GColor compass_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_COMPASSCOLOR)) : base_color;
        set_text_color(compass, compass_color);
        set_text_color(degrees, compass_color);
    }

    if (is_module_enabled(MODULE_SUNRISE)) {
        GColor sunrise_color = enable_advanced && persist_read_int(KEY_SUNRISECOLOR) ? GColorFromHEX(persist_read_int(KEY_SUNRISECOLOR)) : base_color;
        set_text_color(sunrise, sunrise_color);
        set_text_color(sunrise_icon, sunrise_color);
    }
    if (is_module_enabled(MODULE_SUNSET)) {
        GColor sunset_color = enable_advanced && persist_read_int(KEY_SUNSETCOLOR) ? GColorFromHEX(persist_read_int(KEY_SUNSETCOLOR)) : base_color;
        set_text_color(sunset, sunset_color);
        set_text_color(sunset_icon, sunset_color);
    }
    if (is_module_enabled(MODULE_SECONDS)) {
        set_text_color(seconds, enable_advanced && persist_read_int(KEY_SECONDSCOLOR) ? GColorFromHEX(persist_read_int(KEY_SECONDSCOLOR)) : base_color);
    }
    if (is_module_enabled(MODULE_CRYPTO)) {
        set_text_color(crypto, enable_advanced && persist_read_int(KEY_CRYPTOCOLOR) ? GColorFromHEX(persist_read_int(KEY_CRYPTOCOLOR)) : base_color);
    }

    #if !defined PBL_PLATFORM_APLITE
    if (is_module_enabled(MODULE_CRYPTOB)) {
        set_text_color(crypto_b, enable_advanced && persist_read_int(KEY_CRYPTOBCOLOR) ? GColorFromHEX(persist_read_int(KEY_CRYPTOBCOLOR)) : base_color);
    }
    if (is_module_enabled(MODULE_CRYPTOC)) {
        set_text_color(crypto_c, enable_advanced && persist_read_int(KEY_CRYPTOCCOLOR) ? GColorFromHEX(persist_read_int(KEY_CRYPTOCCOLOR)) : base_color);
    }
    if (is_module_enabled(MODULE_CRYPTOD)) {
        set_text_color(crypto_d, enable_advanced && persist_read_int(KEY_CRYPTODCOLOR) ? GColorFromHEX(persist_read_int(KEY_CRYPTODCOLOR)) : base_color);
    }
    #endif
}

void set_bluetooth_color() {
    set_text_color(bluetooth,
        enable_advanced && persist_exists(KEY_BLUETOOTHCOLOR) ? GColorFromHEX(persist_read_int(KEY_BLUETOOTHCOLOR)) : base_color);
}

void set_update_color() {
    set_text_color(update,
        enable_advanced && persist_exists(KEY_UPDATECOLOR) ? GColorFromHEX(persist_read_int(KEY_UPDATECOLOR)) : base_color);
}

void set_battery_color(int percentage) {
    if (percentage > 10) {
        set_text_color(battery, battery_color);
    } else {
        set_text_color(battery, battery_low_color);
    }
}

void set_hours_layer_text(char* text) {
    strcpy(hour_text, text);
    set_text(hours, hour_text);
}

void set_date_layer_text(char* text) {
    strcpy(date_text, text);
    if (loaded_font == LECO_FONT || loaded_font == KONSTRUCT_FONT) {
        for (unsigned char i = 0; date_text[i]; ++i) {
            date_text[i] = to_upper_case((unsigned char)date_text[i]);
        }
    }
    set_text(date, date_text);
}

void set_alt_time_layer_text(char* text) {
    strcpy(alt_time_text, text);
    set_text(alt_time, alt_time_text);
}

#if !defined PBL_PLATFORM_APLITE
void set_alt_time_b_layer_text(char* text) {
    strcpy(alt_time_b_text, text);
    set_text(alt_time_b, alt_time_b_text);
}
#endif

void set_battery_layer_text(char* text) {
    strcpy(battery_text, text);
    set_text(battery, battery_text);
}

void set_bluetooth_layer_text(char* text) {
    strcpy(bluetooth_text, text);
    set_text(bluetooth, bluetooth_text);
}

void set_temp_cur_layer_text(char* text) {
    strcpy(temp_cur_text, text);
    set_text(temp_cur, temp_cur_text);
}

void set_temp_max_layer_text(char* text) {
    strcpy(temp_max_text, text);
    set_text(temp_max, temp_max_text);
}

void set_temp_min_layer_text(char* text) {
    strcpy(temp_min_text, text);
    set_text(temp_min, temp_min_text);
}

#if defined(PBL_HEALTH)
void set_progress_color_steps(bool falling_behind) {
    set_text_color(steps, falling_behind ? steps_behind_color : steps_color);
}

void set_progress_color_dist(bool falling_behind) {
    set_text_color(dist, falling_behind ? dist_behind_color : dist_color);
}

void set_progress_color_cal(bool falling_behind) {
    set_text_color(cal, falling_behind ? cal_behind_color : cal_color);
}

void set_progress_color_sleep(bool falling_behind) {
    set_text_color(sleep, falling_behind ? sleep_behind_color : sleep_color);
}

void set_progress_color_deep(bool falling_behind) {
    set_text_color(deep, falling_behind ? deep_behind_color : deep_color);
}

void set_progress_color_active(bool falling_behind) {
    set_text_color(active, falling_behind ? active_behind_color : active_color);
}

void set_progress_color_heart(int heart_value) {
    bool is_above_low = heart_low == 0 ||
        (heart_low > 0 && heart_value >= heart_low);
    bool is_below_high = heart_high == 0 ||
        (heart_high > 0 && heart_value <= heart_high);
    if (is_above_low && is_below_high) {
        set_text_color(heart, heart_color);
        set_text_color(heart_icon, heart_color);
    } else {
        set_text_color(heart, heart_color_off);
        set_text_color(heart_icon, heart_color_off);
    }
}

void set_steps_layer_text(char* text) {
    strcpy(steps_text, text);
    set_text(steps, steps_text);
}

void set_dist_layer_text(char* text) {
    strcpy(dist_text, text);
    if (loaded_font == LECO_FONT || loaded_font == KONSTRUCT_FONT) {
        for (unsigned char i = 0; dist_text[i]; ++i) {
            dist_text[i] = to_upper_case((unsigned char)dist_text[i]);
        }
    }
    set_text(dist, dist_text);
}

void set_cal_layer_text(char* text) {
    strcpy(cal_text, text);
    if (loaded_font == LECO_FONT) {
        for (unsigned char i = 0; cal_text[i]; ++i) {
            cal_text[i] = to_upper_case((unsigned char)cal_text[i]);
        }
    }
    set_text(cal, cal_text);
}

void set_sleep_layer_text(char* text) {
    strcpy(sleep_text, text);
    if (loaded_font == LECO_FONT) {
        for (unsigned char i = 0; sleep_text[i]; ++i) {
            sleep_text[i] = to_upper_case((unsigned char)sleep_text[i]);
        }
    }
    set_text(sleep, sleep_text);
}

void set_deep_layer_text(char* text) {
    strcpy(deep_text, text);
    if (loaded_font == LECO_FONT || loaded_font == KONSTRUCT_FONT) {
        for (unsigned char i = 0; deep_text[i]; ++i) {
            deep_text[i] = to_upper_case((unsigned char)deep_text[i]);
        }
    }
    set_text(deep, deep_text);
}

void set_active_layer_text(char* text) {
    strcpy(active_text, text);
    if (loaded_font == LECO_FONT || loaded_font == KONSTRUCT_FONT) {
        for (unsigned char i = 0; active_text[i]; ++i) {
            active_text[i] = to_upper_case((unsigned char)active_text[i]);
        }
    }
    set_text(active, active_text);
}

void set_heart_layer_text(char* text) {
    strcpy(heart_text, text);
    if (loaded_font == LECO_FONT || loaded_font == KONSTRUCT_FONT) {
        for (unsigned char i = 0; active_text[i]; ++i) {
            active_text[i] = to_upper_case((unsigned char)active_text[i]);
        }
    }
    set_text(heart, heart_text);
}

void set_heart_icon_layer_text(char* text) {
    strcpy(heart_icon_text, text);
    set_text(heart_icon, heart_icon_text);
}
#endif

void set_weather_layer_text(char* text) {
    strcpy(weather_text, text);
    set_text(weather, weather_text);
}

void set_max_icon_layer_text(char* text) {
    strcpy(max_icon_text, text);
    set_text(max_icon, max_icon_text);
}

void set_min_icon_layer_text(char* text) {
    strcpy(min_icon_text, text);
    set_text(min_icon, min_icon_text);
}

void set_update_layer_text(char* text) {
    strcpy(update_text, text);
    set_text(update, update_text);
}

void set_wind_speed_layer_text(char* text) {
    strcpy(speed_text, text);
    set_text(speed, speed_text);
}

void set_wind_direction_layer_text(char* text) {
    strcpy(direction_text, text);
    set_text(direction, direction_text);
}

void set_wind_unit_layer_text(char* text) {
    strcpy(wind_unit_text, text);
    set_text(wind_unit, wind_unit_text);
}

void set_sunrise_layer_text(char* text) {
    strcpy(sunrise_text, text);
    set_text(sunrise, sunrise_text);
}

void set_sunrise_icon_layer_text(char* text) {
    strcpy(sunrise_icon_text, text);
    set_text(sunrise_icon, sunrise_icon_text);
}

void set_sunset_layer_text(char* text) {
    strcpy(sunset_text, text);
    set_text(sunset, sunset_text);
}

void set_sunset_icon_layer_text(char* text) {
    strcpy(sunset_icon_text, text);
    set_text(sunset_icon, sunset_icon_text);
}

void set_degrees_layer_text(char* text) {
    strcpy(degrees_text, text);
    set_text(degrees, degrees_text);
}

void set_compass_layer_text(char* text) {
    strcpy(compass_text, text);
    set_text(compass, compass_text);
}

void set_seconds_layer_text(char* text) {
    strcpy(seconds_text, text);
    set_text(seconds, seconds_text);
}

void set_crypto_layer_text(char* text) {
    strcpy(crypto_text, text);
    set_text(crypto, crypto_text);
}

#if !defined PBL_PLATFORM_APLITE
void set_crypto_b_layer_text(char* text) {
    strcpy(crypto_b_text, text);
    set_text(crypto_b, crypto_b_text);
}

void set_crypto_c_layer_text(char* text) {
    strcpy(crypto_c_text, text);
    set_text(crypto_c, crypto_c_text);
}

void set_crypto_d_layer_text(char* text) {
    strcpy(crypto_d_text, text);
    set_text(crypto_d, crypto_d_text);
}
#endif
