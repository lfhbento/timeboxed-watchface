#include <pebble.h>

static Window *watchface;
static TextLayer *hours;
static TextLayer *date;
static TextLayer *alt_time;
static TextLayer *battery;
static TextLayer *temp_cur;
static TextLayer *temp_max;
static TextLayer *temp_min;
static TextLayer *steps;
static TextLayer *dist;
static TextLayer *sleep;
static TextLayer *deep_sleep;
static TextLayer *weather;
static TextLayer *max_icon;
static TextLayer *min_icon;
static GFont time_font;
static GFont date_font;
static GFont base_font;
static GFont weather_font;

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Write the current hours and minutes into a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer), (clock_is_24h_style() ? "%H:%M" : "%I:%M"), tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(hours, s_buffer);
}

static void watchface_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMAGINE_42));
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMAGINE_17));
    base_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMAGINE_13));
    weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_20));

    GRect bounds = layer_get_bounds(window_layer);

    hours = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    text_layer_set_background_color(hours, GColorClear);
    text_layer_set_text_color(hours, GColorWhite);
    text_layer_set_text_alignment(hours, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(hours, time_font);
    
    date = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(100, 94), bounds.size.w, 50));
    text_layer_set_background_color(date, GColorClear);
    text_layer_set_text_color(date, GColorWhite);
    text_layer_set_text_alignment(date, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(date, date_font);
    text_layer_set_text(date, "fri.05.feb");
    
    alt_time = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(56, 50), bounds.size.w, 50));
    text_layer_set_background_color(alt_time, GColorClear);
    text_layer_set_text_color(alt_time, GColorWhite);
    text_layer_set_text_alignment(alt_time, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(alt_time, base_font);
    text_layer_set_text(alt_time, "15:54+1.cst");

    battery = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(0, -4), PBL_IF_ROUND_ELSE(117, 111), bounds.size.w, 50));
    text_layer_set_background_color(battery, GColorClear);
    text_layer_set_text_color(battery, GColorWhite);
    text_layer_set_text_alignment(battery, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentRight));
    text_layer_set_font(battery, base_font);
    text_layer_set_text(battery, "70%");

    weather = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(-14, 4), 4, bounds.size.w, 50));
    text_layer_set_background_color(weather, GColorClear);
    text_layer_set_text_color(weather, GColorWhite);
    text_layer_set_text_alignment(weather, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(weather, weather_font);
    text_layer_set_text(weather, "\U0000F002");

    temp_cur = text_layer_create(
        GRect(PBL_IF_ROUND_ELSE(19, 36), 6, bounds.size.w, 50));
    text_layer_set_background_color(temp_cur, GColorClear);
    text_layer_set_text_color(temp_cur, GColorWhite);
    text_layer_set_text_alignment(temp_cur, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(temp_cur, base_font);
    text_layer_set_text(temp_cur, "14c");

    layer_add_child(window_layer, text_layer_get_layer(hours));
    layer_add_child(window_layer, text_layer_get_layer(date));
    layer_add_child(window_layer, text_layer_get_layer(alt_time));
    layer_add_child(window_layer, text_layer_get_layer(battery));
    layer_add_child(window_layer, text_layer_get_layer(weather));
    layer_add_child(window_layer, text_layer_get_layer(temp_cur));
}

static void watchface_unload(Window *window) {
    text_layer_destroy(hours);
    text_layer_destroy(date);
    text_layer_destroy(alt_time);
    text_layer_destroy(battery);
    text_layer_destroy(weather);
    text_layer_destroy(temp_cur);
    fonts_unload_custom_font(time_font);
    fonts_unload_custom_font(date_font);
    fonts_unload_custom_font(base_font);
    fonts_unload_custom_font(weather_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init(void) {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    watchface = window_create();

    window_set_window_handlers(watchface, (WindowHandlers) {
        .load = watchface_load,
        .unload = watchface_unload,
    });

    window_set_background_color(watchface, GColorBlack);

    window_stack_push(watchface, true);
    update_time();
}

static void deinit(void) {
    window_destroy(watchface);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
