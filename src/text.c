#include <pebble.h>
#include "text.h"
#include "keys.h"
#include "configs.h"

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
static TextLayer *health_icon;

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
static char health_icon_text[2];

static uint8_t loaded_font;
static bool enable_advanced;

struct TextPositions {
    GPoint hours;
    GPoint date;
    GPoint alt_time;
    GPoint battery;
    GPoint bluetooth;
    GPoint updates;
    GPoint weather;
    GPoint temp_cur;
    GPoint temp_max;
    GPoint temp_min;
    GPoint icon_max;
    GPoint icon_min;
    GPoint steps_or_sleep;
    GPoint dist_or_deep;
    GPoint health_icon;
};

uint8_t get_loaded_font() {
    return loaded_font;
}

static GPoint create_point(int x, int y) {
    struct GPoint point;
    point.x = x;
    point.y = y;
    return point;
}

static int get_pos(int alignment, int left_pos, int center_pos, int right_pos) {
    switch (alignment) {
        case GTextAlignmentLeft:
            return left_pos;
            break;
        case GTextAlignmentCenter:
            return center_pos;
            break;
        case GTextAlignmentRight:
            return right_pos;
            break;
    }
    return right_pos;
}

static void get_text_positions_blocko(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(46, 38));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, 90));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, 38));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(120, 112));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(70, get_pos(align, 60, 118, 60)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(88, get_pos(align, 78, 118, 78)));
    positions->weather = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions->temp_cur = create_point(PBL_IF_ROUND_ELSE(16, 38), 4);
    positions->temp_max = create_point(PBL_IF_ROUND_ELSE(105, 113), PBL_IF_ROUND_ELSE(22, 4));
    positions->temp_min = create_point(PBL_IF_ROUND_ELSE(70, 80), PBL_IF_ROUND_ELSE(22, 4));
    positions->icon_max = create_point(positions->temp_max.x - 10, PBL_IF_ROUND_ELSE(20, 0));
    positions->icon_min = create_point(positions->temp_min.x - 10, PBL_IF_ROUND_ELSE(20, 0));
    positions->steps_or_sleep = create_point(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148));
    positions->dist_or_deep = create_point(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148));
    positions->health_icon = create_point(PBL_IF_ROUND_ELSE(0, 0), PBL_IF_ROUND_ELSE(144, 148));
}

static void get_text_positions_blocko_big(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 2, 0)), PBL_IF_ROUND_ELSE(40, 32));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(96, 88));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(42, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -4)), PBL_IF_ROUND_ELSE(124, 116));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, 54, 124, 54)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 76, 124, 76)));
    positions->weather = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions->temp_cur = create_point(PBL_IF_ROUND_ELSE(16, 38), PBL_IF_ROUND_ELSE(4, 3));
    positions->temp_max = create_point(PBL_IF_ROUND_ELSE(105, 113), PBL_IF_ROUND_ELSE(22, 3));
    positions->temp_min = create_point(PBL_IF_ROUND_ELSE(70, 80), PBL_IF_ROUND_ELSE(22, 3));
    positions->icon_max = create_point(positions->temp_max.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->icon_min = create_point(positions->temp_min.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->steps_or_sleep = create_point(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148));
    positions->dist_or_deep = create_point(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148));
    positions->health_icon = create_point(PBL_IF_ROUND_ELSE(0, 0), PBL_IF_ROUND_ELSE(144, 148));
}

static void get_text_positions_system(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(54, 42));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, 86));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(124, 112));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
    positions->weather = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions->temp_cur = create_point(PBL_IF_ROUND_ELSE(16, 38), 2);
    positions->temp_max = create_point(PBL_IF_ROUND_ELSE(105, 113), PBL_IF_ROUND_ELSE(20, 2));
    positions->temp_min = create_point(PBL_IF_ROUND_ELSE(70, 80), PBL_IF_ROUND_ELSE(20, 2));
    positions->icon_max = create_point(positions->temp_max.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->icon_min = create_point(positions->temp_min.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->steps_or_sleep = create_point(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148));
    positions->dist_or_deep = create_point(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148));
    positions->health_icon = create_point(PBL_IF_ROUND_ELSE(0, 0), PBL_IF_ROUND_ELSE(144, 148));
}

static void get_text_positions_archivo(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(48, 40));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(100, 92));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(44, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(126, 118));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
    positions->weather = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions->temp_cur = create_point(PBL_IF_ROUND_ELSE(16, 38), 2);
    positions->temp_max = create_point(PBL_IF_ROUND_ELSE(105, 113), PBL_IF_ROUND_ELSE(24, 2));
    positions->temp_min = create_point(PBL_IF_ROUND_ELSE(70, 80), PBL_IF_ROUND_ELSE(24, 2));
    positions->icon_max = create_point(positions->temp_max.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->icon_min = create_point(positions->temp_min.x - 10, PBL_IF_ROUND_ELSE(18, -2));
    positions->steps_or_sleep = create_point(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(144, 148));
    positions->dist_or_deep = create_point(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(158, 148));
    positions->health_icon = create_point(PBL_IF_ROUND_ELSE(0, 0), PBL_IF_ROUND_ELSE(144, 148));
}

static void get_text_positions_din(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(47, 39));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(99, 92));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(42, 32));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(122, 116));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
    positions->weather = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions->temp_cur = create_point(PBL_IF_ROUND_ELSE(16, 38), 2);
    positions->temp_max = create_point(PBL_IF_ROUND_ELSE(105, 113), PBL_IF_ROUND_ELSE(22, 2));
    positions->temp_min = create_point(PBL_IF_ROUND_ELSE(70, 80), PBL_IF_ROUND_ELSE(22, 2));
    positions->icon_max = create_point(positions->temp_max.x - 10, PBL_IF_ROUND_ELSE(20, 0));
    positions->icon_min = create_point(positions->temp_min.x - 10, PBL_IF_ROUND_ELSE(20, 0));
    positions->steps_or_sleep = create_point(PBL_IF_ROUND_ELSE(0, 4), PBL_IF_ROUND_ELSE(145, 148));
    positions->dist_or_deep = create_point(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(160, 148));
    positions->health_icon = create_point(PBL_IF_ROUND_ELSE(0, 0), PBL_IF_ROUND_ELSE(144, 148));
}

static void get_text_positions(int selected_font, GTextAlignment alignment, struct TextPositions* positions) {
    switch(selected_font) {
        case BLOCKO_FONT:
            get_text_positions_blocko(alignment, positions);
            break;
        case BLOCKO_BIG_FONT:
            get_text_positions_blocko_big(alignment, positions);
            break;
        case SYSTEM_FONT:
            get_text_positions_system(alignment, positions);
            break;
        case ARCHIVO_FONT:
            get_text_positions_archivo(alignment, positions);
            break;
        case DIN_FONT:
            get_text_positions_din(alignment, positions);
            break;
        default:
            get_text_positions_blocko(alignment, positions);
    }
};

void create_text_layers(Window* window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating text layers. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    int selected_font = persist_exists(KEY_FONTTYPE) ? persist_read_int(KEY_FONTTYPE) : BLOCKO_FONT;
    int alignment = PBL_IF_ROUND_ELSE(ALIGN_CENTER, persist_exists(KEY_TEXTALIGN) ? persist_read_int(KEY_TEXTALIGN) : ALIGN_RIGHT);

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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Alignment set: %d. %d%2d", text_align, (int)time(NULL), (int)time_ms(NULL, NULL));

    struct TextPositions text_positions;
    get_text_positions(selected_font, text_align, &text_positions);

    int width = bounds.size.w;

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

    weather = text_layer_create(GRect(text_positions.weather.x, text_positions.weather.y, width, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_cur = text_layer_create(GRect(text_positions.temp_cur.x, text_positions.temp_cur.y, width, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    temp_min = text_layer_create(GRect(text_positions.temp_min.x, text_positions.temp_min.y, width, 50));
    text_layer_set_background_color(temp_min, GColorClear);
    text_layer_set_text_alignment(temp_min, GTextAlignmentLeft);

    min_icon = text_layer_create(GRect(text_positions.icon_min.x, text_positions.icon_min.y, width, 50));
    text_layer_set_background_color(min_icon, GColorClear);
    text_layer_set_text_alignment(min_icon, GTextAlignmentLeft);

    temp_max = text_layer_create(GRect(text_positions.temp_max.x, text_positions.temp_max.y, width, 50));
    text_layer_set_background_color(temp_max, GColorClear);
    text_layer_set_text_alignment(temp_max, GTextAlignmentLeft);

    max_icon = text_layer_create(GRect(text_positions.icon_max.x, text_positions.icon_max.y, width, 50));
    text_layer_set_background_color(max_icon, GColorClear);
    text_layer_set_text_alignment(max_icon, GTextAlignmentLeft);

    steps_or_sleep = text_layer_create(GRect(text_positions.steps_or_sleep.x, text_positions.steps_or_sleep.y, width, 50));
    text_layer_set_background_color(steps_or_sleep, GColorClear);
    text_layer_set_text_alignment(steps_or_sleep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));

    dist_or_deep = text_layer_create(GRect(text_positions.dist_or_deep.x, text_positions.dist_or_deep.y, width, 50));
    text_layer_set_background_color(dist_or_deep, GColorClear);
    text_layer_set_text_alignment(dist_or_deep, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));

    health_icon = text_layer_create(GRect(text_positions.health_icon.x, text_positions.health_icon.y, width, 50));
    text_layer_set_background_color(health_icon, GColorClear);
    text_layer_set_text_alignment(health_icon, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentCenter));

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
    layer_add_child(window_layer, text_layer_get_layer(health_icon));

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text layers created. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void destroy_text_layers() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroying text layers. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    text_layer_destroy(health_icon);
}

void load_face_fonts() {
    int selected_font = BLOCKO_FONT;

    if (persist_exists(KEY_FONTTYPE)) {
        selected_font = persist_read_int(KEY_FONTTYPE);
    }

    if (selected_font == SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading system fonts. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        medium_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
        base_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = SYSTEM_FONT;
    } else if (selected_font == ARCHIVO_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Archivo font. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_28));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHIVO_18));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = ARCHIVO_FONT;
    } else if (selected_font == DIN_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading DIN font. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_58));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_26));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DIN_20));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = DIN_FONT;
    } else if (selected_font == BLOCKO_BIG_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font (big). %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_64));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_32));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_19));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_28));
        loaded_font = BLOCKO_BIG_FONT;
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading Blocko font. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_56));
        medium_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_24));
        base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BLOCKO_16));
        weather_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
        loaded_font = BLOCKO_FONT;
    }

    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_24));
    awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ICONS_20));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Fonts loaded. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void unload_face_fonts() {
    if (loaded_font != SYSTEM_FONT) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading custom fonts. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
        fonts_unload_custom_font(time_font);
        fonts_unload_custom_font(medium_font);
        fonts_unload_custom_font(base_font);
    }
    fonts_unload_custom_font(weather_font);
    fonts_unload_custom_font(weather_big_font);
    fonts_unload_custom_font(awesome_font);
}

void set_face_fonts() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting fonts. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
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
    text_layer_set_font(health_icon, awesome_font);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Fonts set. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void set_colors(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Defining colors. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
    base_color = persist_exists(KEY_HOURSCOLOR) ? GColorFromHEX(persist_read_int(KEY_HOURSCOLOR)) : GColorWhite;
    text_layer_set_text_color(hours, base_color);
    enable_advanced = get_config_toggles() != -1 ? is_advanced_colors_enabled() : persist_exists(KEY_ENABLEADVANCED) ? persist_read_int(KEY_ENABLEADVANCED) : false;
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
    text_layer_set_text_color(steps_or_sleep,
            enable_advanced ? steps_color : base_color);
    text_layer_set_text_color(dist_or_deep,
            enable_advanced ? GColorFromHEX(persist_read_int(KEY_DISTCOLOR)) : base_color);
    text_layer_set_text_color(health_icon, steps_color);
    battery_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYCOLOR)) : base_color;
    battery_low_color = enable_advanced ? GColorFromHEX(persist_read_int(KEY_BATTERYLOWCOLOR)) : base_color;

    window_set_background_color(window, persist_read_int(KEY_BGCOLOR) ? GColorFromHEX(persist_read_int(KEY_BGCOLOR)) : GColorBlack);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Defined colors. %d%2d", (int)time(NULL), (int)time_ms(NULL, NULL));
}

void set_steps_dist_color(bool falling_behind_steps, bool falling_behind_dist) {
    GColor steps_cur_color = falling_behind_steps ? steps_behind_color : steps_color;
    text_layer_set_text_color(steps_or_sleep, steps_cur_color);
    text_layer_set_text_color(health_icon, steps_cur_color);
    text_layer_set_text_color(dist_or_deep, falling_behind_dist ? dist_behind_color : dist_color);
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
    if (percentage > 20) {
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

void set_health_icon_text(char* text) {
    strcpy(health_icon_text, text);
    text_layer_set_text(health_icon, health_icon_text);
}
