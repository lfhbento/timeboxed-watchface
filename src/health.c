#include <pebble.h>
#include "keys.h"
#include "health.h"
#include "text.h"
#include "configs.h"
#include "screen.h"


#if defined(PBL_HEALTH)
static bool health_enabled;
static int woke_up_at;
static bool was_asleep;
static bool sleep_data_visible;
static bool sleep_data_enabled;
static bool useKm;
static bool useCalories;
static bool update_queued;
static bool is_sleeping;
static bool sleep_status_updated;
static char steps_text[8];
static char cal_text[10];
static char dist_text[10];
static char sleep_text[8];
static char deep_text[8];
static char active_text[8];
static char heart_text[10];

static void clear_health_fields() {
    set_steps_layer_text("");
    set_dist_layer_text("");
    set_cal_layer_text("");
    set_sleep_layer_text("");
    set_deep_layer_text("");
    set_active_layer_text("");
    set_heart_layer_text("");
    set_heart_icon_layer_text("");
}

static bool health_permission_granted() {
    if (!health_enabled) {
        return false;
    }
    HealthMetric metric_steps = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);

    return !(mask_steps & HealthServiceAccessibilityMaskNoPermission);
}

static void get_steps_data() {
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;
    int current_steps = 0;
    int steps_last_week = 0;

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);

    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        steps_last_week = 0;
        HealthServiceAccessibilityMask mask_steps_average =
            health_service_metric_averaged_accessible(metric_steps, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if (mask_steps_average & HealthServiceAccessibilityMaskAvailable) {
            steps_last_week = (int)health_service_sum_averaged(metric_steps, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 7; i <= 28; i = i+7) {
                steps_last_week += (int)health_service_sum(metric_steps, start - i*one_day, end - i*one_day);
            }
            steps_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data: %d / %d", current_steps, steps_last_week);

        snprintf(steps_text, sizeof(steps_text), "%d", current_steps);

        set_steps_layer_text(steps_text);

        set_progress_color_steps(current_steps < steps_last_week);
    }
}

static void get_dist_data() {
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_dist = 0;
    int dist_last_week = 0;
    int current_dist_int = 0;
    int current_dist_dec = 0;

    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);

    if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        current_dist = (int)health_service_sum_today(metric_dist);

        dist_last_week = 0;
        HealthServiceAccessibilityMask mask_dist_average =
            health_service_metric_averaged_accessible(metric_dist, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if (mask_dist_average & HealthServiceAccessibilityMaskAvailable) {
            dist_last_week = (int)health_service_sum_averaged(metric_dist, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 7; i <= 28; i = i+7) {
                dist_last_week += (int)health_service_sum(metric_dist, start - i*one_day, end - i*one_day);
            }
            dist_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Dist data: %d / %d", current_dist, dist_last_week);

        if (!useKm) {
            current_dist /= 1.6;
        }
        current_dist_int = current_dist/1000;
        current_dist_dec = (current_dist%1000)/100;

        snprintf(dist_text, sizeof(dist_text), (useKm ? "%d.%dkm" : "%d.%dmi"), current_dist_int, current_dist_dec);

        set_dist_layer_text(dist_text);
        set_progress_color_dist(current_dist < dist_last_week);
    }
}

static void get_cal_data() {

    HealthMetric metric_cal_rest = HealthMetricRestingKCalories;
    HealthMetric metric_cal_act = HealthMetricActiveKCalories;

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_cal = 0;
    int cal_last_week = 0;

    HealthServiceAccessibilityMask mask_cal_rest =
        health_service_metric_accessible(metric_cal_rest, start, end);
    HealthServiceAccessibilityMask mask_cal_act =
        health_service_metric_accessible(metric_cal_act, start, end);


    bool has_cal_metric = (mask_cal_rest & HealthServiceAccessibilityMaskAvailable) || (mask_cal_act & HealthServiceAccessibilityMaskAvailable);
    if (has_cal_metric) {
        current_cal = (int)health_service_sum_today(metric_cal_rest) + (int)health_service_sum_today(metric_cal_act);

        cal_last_week = 0;
        HealthServiceAccessibilityMask mask_cal_rest_average =
            health_service_metric_averaged_accessible(metric_cal_rest, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        HealthServiceAccessibilityMask mask_cal_act_average =
            health_service_metric_averaged_accessible(metric_cal_act, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if ((mask_cal_rest_average & HealthServiceAccessibilityMaskAvailable) || mask_cal_act_average & HealthServiceAccessibilityMaskAvailable) {
            cal_last_week += (int)health_service_sum_averaged(metric_cal_rest, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
            cal_last_week += (int)health_service_sum_averaged(metric_cal_act, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 7; i <= 28; i = i+7) {
                cal_last_week += (int)health_service_sum(metric_cal_rest, start - i*one_day, end - i*one_day);
                cal_last_week += (int)health_service_sum(metric_cal_act, start - i*one_day, end - i*one_day);
            }
            cal_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Calories data: %d / %d", current_cal, cal_last_week);

        snprintf(cal_text, sizeof(cal_text), (get_loaded_font() == KONSTRUCT_FONT && current_cal >= 2000) ? "%dcal" : "%d cal", current_cal);

        set_cal_layer_text(cal_text);
        set_progress_color_cal(current_cal < cal_last_week);

    }
}

static void get_sleep_data() {

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_sleep = 0;
    int sleep_last_week = 0;

    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_sleep_average =
        health_service_metric_averaged_accessible(metric_sleep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum_today(metric_sleep);

        sleep_last_week = 0;
        if (mask_sleep_average & HealthServiceAccessibilityMaskAvailable) {
            sleep_last_week = (int)health_service_sum_averaged(metric_sleep, start, start + 24*SECONDS_PER_HOUR-1, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 1; i <= 7; i++) {
                sleep_last_week += (int)health_service_sum(metric_sleep, start - i*one_day, start - (i-1)*one_day);
            }
            sleep_last_week /= 7;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Sleep data: %d / %d", current_sleep, sleep_last_week);

        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = ((current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;

        snprintf(sleep_text, sizeof(sleep_text), "%dh%02dm", current_sleep_hours, current_sleep_minutes);

        set_sleep_layer_text(sleep_text);
        set_progress_color_sleep(current_sleep < sleep_last_week);
    }
}

static void get_deep_data() {

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_deep = 0;
    int deep_last_week = 0;

    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);
    HealthServiceAccessibilityMask mask_deep_average =
        health_service_metric_averaged_accessible(metric_deep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum_today(metric_deep);

        deep_last_week = 0;
        if (mask_deep_average & HealthServiceAccessibilityMaskAvailable) {
            deep_last_week = (int)health_service_sum_averaged(metric_deep, start, start + 24*SECONDS_PER_HOUR-1, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 1; i <= 7; i++) {
                deep_last_week += (int)health_service_sum(metric_deep, start - i*one_day, start - (i-1)*one_day);
            }
            deep_last_week /= 7;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Deep sleep data: %d / %d", current_deep, deep_last_week);

        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = ((current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;

        snprintf(deep_text, sizeof(deep_text), "%dh%02dm", current_deep_hours, current_deep_minutes);

        set_deep_layer_text(deep_text);
        set_progress_color_deep(current_deep < deep_last_week);
    }

}

static void get_active_data() {

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_active = 0;
    int active_last_week = 0;

    HealthMetric metric_active = HealthMetricActiveSeconds;
    HealthServiceAccessibilityMask mask_active =
        health_service_metric_accessible(metric_active, start, end);
    HealthServiceAccessibilityMask mask_active_average =
        health_service_metric_averaged_accessible(metric_active, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

    if (mask_active & HealthServiceAccessibilityMaskAvailable) {
        current_active = (int)health_service_sum_today(metric_active);

        active_last_week = 0;
        if (mask_active_average & HealthServiceAccessibilityMaskAvailable) {
            active_last_week = (int)health_service_sum_averaged(metric_active, start, start + 24*SECONDS_PER_HOUR-1, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            for (int i = 1; i <= 7; i++) {
                active_last_week += (int)health_service_sum(metric_active, start - i*one_day, start - (i-1)*one_day);
            }
            active_last_week /= 7;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Active data: %d / %d", current_active, active_last_week);

        int current_active_hours = current_active / SECONDS_PER_HOUR;
        int current_active_minutes = ((current_active - (current_active_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE) % SECONDS_PER_MINUTE;

        snprintf(active_text, sizeof(active_text), "%dh%02dm", current_active_hours, current_active_minutes);

        set_active_layer_text(active_text);
        set_progress_color_active(current_active < active_last_week);
    }
}

static void get_heart_data() {
    int current_heart = 0;

    HealthMetric metric_heart = HealthMetricHeartRateBPM;

    current_heart = (int)health_service_peek_current_value(metric_heart);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Heart data: %d", current_heart);

    snprintf(heart_text, sizeof(heart_text), "%d", current_heart);


    set_heart_layer_text(heart_text);
    set_heart_icon_layer_text("v");
    set_progress_color_heart(current_heart);
}

void queue_health_update() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Queued health update. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    update_queued = true;
}

void get_health_data() {
    if (health_enabled && update_queued) {
        update_queued = false;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating health data. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        if (is_module_enabled(MODULE_STEPS)) {
            get_steps_data();
        }
        if (is_module_enabled(MODULE_DIST)) {
            get_dist_data();
        }
        if (is_module_enabled(MODULE_CAL)) {
            get_cal_data();
        }
        if (is_module_enabled(MODULE_SLEEP)) {
            get_sleep_data();
        }
        if (is_module_enabled(MODULE_DEEP)) {
            get_deep_data();
        }
        if (is_module_enabled(MODULE_ACTIVE)) {
            get_active_data();
        }
        if (is_module_enabled(MODULE_HEART)) {
            get_heart_data();
        }
    }
}

void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
        case HealthEventMovementUpdate:
        case HealthEventSleepUpdate:
        case HealthEventMetricAlert:
        case HealthEventHeartRateUpdate:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update from event. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            queue_health_update();
            break;
    }
}

static void load_health_data_from_storage() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading health data from storage. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    if (is_module_enabled(MODULE_STEPS)) {
        persist_read_string(KEY_STEPS, steps_text, sizeof(steps_text));
        set_steps_layer_text(steps_text);
        set_progress_color_steps(false);
    }
    if (is_module_enabled(MODULE_DIST)) {
        persist_read_string(KEY_DIST, dist_text, sizeof(dist_text));
        set_dist_layer_text(dist_text);
        set_progress_color_dist(false);
    }
    if (is_module_enabled(MODULE_CAL)) {
        persist_read_string(KEY_CAL, cal_text, sizeof(cal_text));
        set_cal_layer_text(cal_text);
        set_progress_color_cal(false);
    }
    if (is_module_enabled(MODULE_SLEEP)) {
        persist_read_string(KEY_SLEEP, sleep_text, sizeof(sleep_text));
        set_sleep_layer_text(sleep_text);
        set_progress_color_sleep(false);
    }
    if (is_module_enabled(MODULE_DEEP)) {
        persist_read_string(KEY_DEEP, deep_text, sizeof(deep_text));
        set_deep_layer_text(deep_text);
        set_progress_color_deep(false);
    }
    if (is_module_enabled(MODULE_ACTIVE)) {
        persist_read_string(KEY_ACTIVE, active_text, sizeof(active_text));
        set_active_layer_text(active_text);
        set_progress_color_active(false);
    }
    if (is_module_enabled(MODULE_HEART)) {
        set_heart_layer_text("0");
        set_heart_icon_layer_text("v");
        set_progress_color_heart(0);
    }
}

static bool get_health_enabled() {
    return is_health_toggle_enabled() ||
        is_module_enabled(MODULE_STEPS) ||
        is_module_enabled(MODULE_DIST) ||
        is_module_enabled(MODULE_CAL) ||
        is_module_enabled(MODULE_SLEEP) ||
        is_module_enabled(MODULE_DEEP) ||
        is_module_enabled(MODULE_ACTIVE) ||
        is_module_enabled(MODULE_HEART);
}

void toggle_health(uint8_t reload_origin) {
    is_sleeping = false;
    bool has_health = false;
    health_enabled = get_health_enabled();
    sleep_data_enabled = is_sleep_data_enabled();

    if (health_enabled) {
        MeasurementSystem distMeasure = health_service_get_measurement_system_for_display(HealthMetricWalkedDistanceMeters);
        if (distMeasure != MeasurementSystemUnknown) {
            useKm = distMeasure == MeasurementSystemMetric;
        } else {
            useKm = is_use_km_enabled();
        }

        useCalories = is_use_calories_enabled();
        if (health_permission_granted()) {
            has_health = health_service_events_subscribe(health_handler, NULL);
            if (has_health) {
                clear_health_fields();
                queue_health_update();
                if (reload_origin != RELOAD_DEFAULT) {
                    get_health_data();
                } else {
                    load_health_data_from_storage();
                }
            } else {
                health_service_events_unsubscribe();
            }
        } else {
            health_enabled = false;
        }
    }

    if (!health_enabled || !has_health) {
        clear_health_fields();
        health_service_events_unsubscribe();
    }
}

bool is_user_sleeping() {
    if (!health_enabled) {
        return false;
    }
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    if (tick_time->tm_min % 10 == 0) { // only check every 10 minutes
        if (!sleep_status_updated) {
            sleep_status_updated = true;
            HealthActivityMask activities = health_service_peek_current_activities();
            is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
        }
    } else {
        sleep_status_updated = false;
    }
    return is_sleeping;
}

void show_sleep_data_if_visible(Window *watchface) {
    if (health_enabled && sleep_data_enabled) {
        if (is_user_sleeping()) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We are asleep. %d", was_asleep);
            sleep_data_visible = true;
            if (!was_asleep) {
                was_asleep = true;
                woke_up_at = 0;
                queue_health_update();
                redraw_screen(watchface);
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Just went to sleep. %d", was_asleep);
            }
        }

        if (!is_user_sleeping() && was_asleep) {
            sleep_data_visible = true;
            woke_up_at = time(NULL) + SECONDS_PER_MINUTE * 30; // 30 minutes
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We woke up! %d", (int) woke_up_at);
            was_asleep = false;
            queue_health_update();
        }

        if (sleep_data_visible && woke_up_at > 0 && time(NULL) > woke_up_at) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Past half an hour after wake up! %d - %d", (int) time(NULL), (int) woke_up_at);
            sleep_data_visible = false;
            redraw_screen(watchface);
            queue_health_update();
        }
    }
}

void init_sleep_data() {
    was_asleep = false;
    sleep_data_visible = false;
}

void save_health_data_to_storage() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing health data. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    persist_write_string(KEY_STEPS, steps_text);
    persist_write_string(KEY_DIST, dist_text);
    persist_write_string(KEY_CAL, cal_text);
    persist_write_string(KEY_SLEEP, sleep_text);
    persist_write_string(KEY_DEEP, deep_text);
    persist_write_string(KEY_ACTIVE, active_text);
}

bool should_show_sleep_data() {
    return sleep_data_visible;
}

#else // Health not available

bool is_user_sleeping() {
    return false;
}

bool should_show_sleep_data() {
    return false;
}

#endif
