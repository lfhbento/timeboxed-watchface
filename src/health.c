#include <pebble.h>
#include "keys.h"
#include "health.h"
#include "text.h"
#include "configs.h"


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
static char steps_or_sleep_text[16];
static char dist_or_deep_text[16];

static bool health_permission_granted() {
    HealthMetric metric_steps = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);

    return !(mask_steps & HealthServiceAccessibilityMaskNoPermission);
}

static void update_steps_data() {

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    HealthMetric metric_cal_rest = HealthMetricRestingKCalories;
    HealthMetric metric_cal_act = HealthMetricActiveKCalories;

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_steps = 0;
    int steps_last_week = 0;
    int current_cal_or_dist = 0;
    int cal_or_dist_last_week = 0;
    int current_dist_int = 0;
    int current_dist_dec = 0;

    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);
    HealthServiceAccessibilityMask mask_cal_rest =
        health_service_metric_accessible(metric_cal_rest, start, end);
    HealthServiceAccessibilityMask mask_cal_act =
        health_service_metric_accessible(metric_cal_act, start, end);


    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        steps_last_week = 0;
        HealthServiceAccessibilityMask mask_steps_average =
            health_service_metric_averaged_accessible(metric_steps, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if (mask_steps_average & HealthServiceAccessibilityMaskAvailable) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API average steps");
            steps_last_week = (int)health_service_sum_averaged(metric_steps, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using manual average steps");
            for (int i = 7; i <= 28; i = i+7) {
                steps_last_week += (int)health_service_sum(metric_steps, start - i*one_day, end - i*one_day);
            }
            steps_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data: %d / %d", current_steps, steps_last_week);

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%d", current_steps);

        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    bool has_cal_metric = (mask_cal_rest & HealthServiceAccessibilityMaskAvailable) || (mask_cal_act & HealthServiceAccessibilityMaskAvailable);
    if (useCalories && has_cal_metric) {
        current_cal_or_dist = (int)health_service_sum_today(metric_cal_rest) + (int)health_service_sum_today(metric_cal_act);

        cal_or_dist_last_week = 0;
        HealthServiceAccessibilityMask mask_cal_rest_average =
            health_service_metric_averaged_accessible(metric_cal_rest, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        HealthServiceAccessibilityMask mask_cal_act_average =
            health_service_metric_averaged_accessible(metric_cal_act, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if ((mask_cal_rest_average & HealthServiceAccessibilityMaskAvailable) || mask_cal_act_average & HealthServiceAccessibilityMaskAvailable) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API average calories");
            cal_or_dist_last_week += (int)health_service_sum_averaged(metric_cal_rest, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
            cal_or_dist_last_week += (int)health_service_sum_averaged(metric_cal_act, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using manual average calories");
            for (int i = 7; i <= 28; i = i+7) {
                cal_or_dist_last_week += (int)health_service_sum(metric_cal_rest, start - i*one_day, end - i*one_day);
                cal_or_dist_last_week += (int)health_service_sum(metric_cal_act, start - i*one_day, end - i*one_day);
            }
            cal_or_dist_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Calories data: %d / %d", current_cal_or_dist, cal_or_dist_last_week);

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), "%dk", current_cal_or_dist);

        set_dist_or_deep_layer_text(dist_or_deep_text);

    } else if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        current_cal_or_dist = (int)health_service_sum_today(metric_dist);

        cal_or_dist_last_week = 0;
        HealthServiceAccessibilityMask mask_dist_average =
            health_service_metric_averaged_accessible(metric_dist, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

        if (mask_dist_average & HealthServiceAccessibilityMaskAvailable) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API average dist");
            cal_or_dist_last_week = (int)health_service_sum_averaged(metric_dist, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using manual average dist");
            for (int i = 7; i <= 28; i = i+7) {
                cal_or_dist_last_week += (int)health_service_sum(metric_dist, start - i*one_day, end - i*one_day);
            }
            cal_or_dist_last_week /= 4;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Dist data: %d / %d", current_cal_or_dist, cal_or_dist_last_week);

        if (!useKm) {
            current_cal_or_dist /= 1.6;
        }
        current_dist_int = current_cal_or_dist/1000;
        current_dist_dec = (current_cal_or_dist%1000)/100;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), (useKm ? "%d.%dkm" : "%d.%dmi"), current_dist_int, current_dist_dec);

        set_dist_or_deep_layer_text(dist_or_deep_text);
    }

    set_steps_dist_color(current_steps < steps_last_week, current_cal_or_dist < cal_or_dist_last_week);
}

static void update_sleep_data() {
    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    int current_sleep = 0;
    int current_deep = 0;
    int sleep_last_week = 0;
    int deep_last_week = 0;

    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);
    HealthServiceAccessibilityMask mask_sleep_average =
        health_service_metric_averaged_accessible(metric_sleep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
    HealthServiceAccessibilityMask mask_deep_average =
        health_service_metric_averaged_accessible(metric_deep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum_today(metric_sleep);

        sleep_last_week = 0;
        if (mask_sleep_average & HealthServiceAccessibilityMaskAvailable) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API average sleep");
            sleep_last_week = (int)health_service_sum_averaged(metric_sleep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using manual average sleep");
            for (int i = 1; i <= 7; i++) {
                sleep_last_week += (int)health_service_sum(metric_sleep, start - i*one_day, start - (i-1)*one_day);
            }
            sleep_last_week /= 7;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Sleep data: %d / %d", current_sleep, sleep_last_week);

        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = (current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%dh%dm", current_sleep_hours, current_sleep_minutes);

        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum_today(metric_deep);

        deep_last_week = 0;
        if (mask_deep_average & HealthServiceAccessibilityMaskAvailable) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API average deep sleep");
            deep_last_week = (int)health_service_sum_averaged(metric_deep, start, end, HealthServiceTimeScopeDailyWeekdayOrWeekend);
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Using manual average deep sleep");
            for (int i = 1; i <= 7; i++) {
                deep_last_week += (int)health_service_sum(metric_deep, start - i*one_day, start - (i-1)*one_day);
            }
            deep_last_week /= 7;
        }
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Deep sleep data: %d / %d", current_deep, deep_last_week);

        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = (current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), "%dh%dm", current_deep_hours, current_deep_minutes);

        set_dist_or_deep_layer_text(dist_or_deep_text);
    }

    set_steps_dist_color(current_sleep < sleep_last_week, current_deep < deep_last_week);
}

void queue_health_update() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Queued health update. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    update_queued = true;
}

void get_health_data() {
    if (health_enabled && update_queued) {
        update_queued = false;
        if (!sleep_data_visible) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating steps data. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_steps_data();
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data updated. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating sleep data. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_sleep_data();
        }
    }
}

void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
        case HealthEventMovementUpdate:
        case HealthEventSleepUpdate:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update from event. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            queue_health_update();
            break;
    }
}

static void load_health_data_from_storage() {
    if (persist_exists(KEY_STEPS)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading health data from storage. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        persist_read_string(KEY_STEPS, steps_or_sleep_text, sizeof(steps_or_sleep_text));
        persist_read_string(KEY_DIST, dist_or_deep_text, sizeof(dist_or_deep_text));
        set_steps_or_sleep_layer_text(steps_or_sleep_text);
        set_dist_or_deep_layer_text(dist_or_deep_text);
    }
}

void toggle_health(bool from_configs) {
    is_sleeping = false;
    bool has_health = false;
    bool using_configs = get_config_toggles() != -1;
    health_enabled = using_configs ? is_health_toggle_enabled() : persist_read_int(KEY_ENABLEHEALTH);
    sleep_data_enabled = using_configs ? is_sleep_data_enabled() : persist_exists(KEY_SHOWSLEEP) && persist_read_int(KEY_SHOWSLEEP);

    MeasurementSystem distMeasure = health_service_get_measurement_system_for_display(HealthMetricWalkedDistanceMeters);
    if (distMeasure != MeasurementSystemUnknown) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Using API measure system. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        useKm = distMeasure == MeasurementSystemMetric;
    } else {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Using config measure system. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        useKm = using_configs ? is_use_km_enabled() : persist_exists(KEY_USEKM) && persist_read_int(KEY_USEKM);
    }

    useCalories = is_use_calories_enabled();
    if (health_enabled) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health enabled. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        if (health_permission_granted()) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Health permission granted. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            has_health = health_service_events_subscribe(health_handler, NULL);
            set_steps_or_sleep_layer_text("");
            set_dist_or_deep_layer_text("");
            queue_health_update();
            if (from_configs) {
                get_health_data();
            } else {
                load_health_data_from_storage();
            }
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Health permission not granted. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
            health_enabled = false;
        }
    }

    if (!health_enabled || !has_health) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health disabled. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_steps_or_sleep_layer_text("");
        set_dist_or_deep_layer_text("");
        health_service_events_unsubscribe();
    }
}

bool is_user_sleeping() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    if (tick_time->tm_min % 10 == 0) { // only check every 10 minutes
        if (!sleep_status_updated) {
            sleep_status_updated = true;
            HealthActivityMask activities = health_service_peek_current_activities();
            APP_LOG(APP_LOG_LEVEL_DEBUG,
                "Sleeping data. %d %d %d",
                (int)activities & HealthActivitySleep,
                (int)activities & HealthActivityRestfulSleep,
                (int)activities & HealthActivitySleep || activities & HealthActivityRestfulSleep);
            is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
        }
    } else {
        sleep_status_updated = false;
    }

    return is_sleeping;
}

void show_sleep_data_if_visible() {
    if (health_enabled && sleep_data_enabled) {
        if (is_user_sleeping()) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We are asleep. %d", was_asleep);
            sleep_data_visible = true;
            if (!was_asleep) {
                was_asleep = true;
                woke_up_at = 0;
                queue_health_update();
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
    persist_write_string(KEY_STEPS, steps_or_sleep_text);
    persist_write_string(KEY_DIST, dist_or_deep_text);
}

#else // Health not available

void toggle_health(bool from_configs) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Health disabled. %d%03d", (int)time(NULL), (int)time_ms(NULL, NULL));
    set_steps_or_sleep_layer_text("");
    set_dist_or_deep_layer_text("");
}

bool is_user_sleeping() {
    return false;
}

void get_health_data() {
    return;
}

void show_sleep_data_if_visible() {
    return;
}

void init_sleep_data() {
    return;
}

void queue_health_update() {
    return;
}

void save_health_data_to_storage() {
    return;
}

#endif
