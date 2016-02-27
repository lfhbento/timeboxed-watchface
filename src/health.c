#include <pebble.h>
#include "keys.h"
#include "health.h"
#include "text.h"


#if defined(PBL_HEALTH)
static bool health_enabled;
static int woke_up_at;
static bool was_asleep;
static bool sleep_data_visible;
static bool sleep_data_enabled;
static bool useKm;
static bool update_queued;

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
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    uint16_t current_steps = 0;
    uint16_t steps_last_week = 0;
    uint16_t current_dist = 0;
    uint16_t dist_last_week = 0;
    uint16_t current_dist_int = 0;
    uint16_t current_dist_dec = 0;
    char steps_or_sleep_text[16];
    char dist_or_deep_text[16];

    HealthServiceAccessibilityMask mask_steps =
        health_service_metric_accessible(metric_steps, start, end);
    HealthServiceAccessibilityMask mask_dist =
        health_service_metric_accessible(metric_dist, start, end);

    if (mask_steps & HealthServiceAccessibilityMaskAvailable) {
        current_steps = (int)health_service_sum_today(metric_steps);

        steps_last_week = 0;
        for (int i = 7; i <= 28; i = i+7) {
            steps_last_week += (int)health_service_sum(metric_steps, start - i*one_day, end - i*one_day);
        }
        steps_last_week /= 4;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data: %d / %d", current_steps, steps_last_week);

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%d", current_steps);

        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        current_dist = (int)health_service_sum_today(metric_dist);

        dist_last_week = 0;
        for (int i = 7; i <= 28; i = i+7) {
            dist_last_week += (int)health_service_sum(metric_dist, start - i*one_day, end - i*one_day);
        }
        dist_last_week /= 4;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Dist data: %d / %d", current_dist, dist_last_week);

        if (!useKm) {
            current_dist /= 1.6;
        }
        current_dist_int = current_dist/1000;
        current_dist_dec = (current_dist%1000)/100;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), (useKm ? "%d.%dkm" : "%d.%dmi"), current_dist_int, current_dist_dec);

        set_dist_or_deep_layer_text(dist_or_deep_text);
    }

    set_health_icon_text("");
    set_steps_dist_color(current_steps < steps_last_week, current_dist < dist_last_week);
    persist_write_string(KEY_STEPS, steps_or_sleep_text);
    persist_write_string(KEY_DIST, dist_or_deep_text);
}

static void update_sleep_data() {
    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;

    time_t start = time_start_of_today();
    time_t end = time(NULL);
    int one_day = 24 * SECONDS_PER_HOUR;

    uint16_t current_sleep = 0;
    uint16_t current_deep = 0;
    uint16_t sleep_last_week = 0;
    uint16_t deep_last_week = 0;
    char steps_or_sleep_text[16];
    char dist_or_deep_text[16];

    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum(metric_sleep, start, end);

        sleep_last_week = 0;
        for (int i = 1; i <= 7; i++) {
            sleep_last_week += (int)health_service_sum(metric_sleep, start - i*one_day, end - i*one_day);
        }
        sleep_last_week /= 7;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Sleep data: %d / %d", current_sleep, sleep_last_week);

        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = (current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%dh%dm", current_sleep_hours, current_sleep_minutes);

        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum(metric_deep, start, end);

        deep_last_week = 0;
        for (int i = 1; i <= 7; i++) {
            deep_last_week += (int)health_service_sum(metric_deep, start - i*one_day, end - i*one_day);
        }
        deep_last_week /= 7;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Sleep data: %d / %d", current_deep, deep_last_week);

        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = (current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), "%dh%dm", current_deep_hours, current_deep_minutes);

        set_dist_or_deep_layer_text(dist_or_deep_text);
    }

    set_health_icon_text("");
    set_steps_dist_color(current_sleep < sleep_last_week, current_deep < deep_last_week);
    persist_write_string(KEY_SLEEP, steps_or_sleep_text);
    persist_write_string(KEY_DEEP, dist_or_deep_text);
}

void queue_health_update() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Queued health update. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    update_queued = true;
}

void get_health_data() {
    if (health_enabled && update_queued) {
        update_queued = false;
        if (!sleep_data_visible) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating steps data. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_steps_data();
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data updated. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating sleep data. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_sleep_data();
        }
    }
}

void health_handler(HealthEventType event, void *context) {
    switch(event) {
        case HealthEventSignificantUpdate:
        case HealthEventMovementUpdate:
        case HealthEventSleepUpdate:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting update from event. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            queue_health_update();
            break;
    }
}

static void load_steps_from_storage() {
    if (persist_exists(KEY_STEPS)) {
        char steps[10];
        char dist[10];
        persist_read_string(KEY_STEPS, steps, sizeof(steps));
        persist_read_string(KEY_DIST, dist, sizeof(dist));
        set_steps_or_sleep_layer_text(steps);
        set_dist_or_deep_layer_text(dist);
        set_health_icon_text("");
    }
}

static void load_sleep_from_storage() {
    if (persist_exists(KEY_SLEEP)) {
        char sleep[14];
        char deep[14];
        persist_read_string(KEY_SLEEP, sleep, sizeof(sleep));
        persist_read_string(KEY_DEEP, deep, sizeof(deep));
        set_steps_or_sleep_layer_text(sleep);
        set_dist_or_deep_layer_text(deep);
        set_health_icon_text("");
    }
}

void toggle_health(bool from_configs) {
    bool has_health = false;
    health_enabled = persist_read_int(KEY_ENABLEHEALTH);
    sleep_data_enabled = persist_exists(KEY_SHOWSLEEP) && persist_read_int(KEY_SHOWSLEEP);
    useKm = persist_exists(KEY_USEKM) && persist_read_int(KEY_USEKM);
    if (health_enabled) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        if (health_permission_granted()) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Health permission granted. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            has_health = health_service_events_subscribe(health_handler, NULL);
            set_steps_or_sleep_layer_text("");
            set_dist_or_deep_layer_text("");
            set_health_icon_text("");
            queue_health_update();
            if (from_configs) {
                get_health_data();
            } else {
                if (!sleep_data_visible) {
                    load_steps_from_storage();
                } else {
                    load_sleep_from_storage();
                }
            }
        } else {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Health permission not granted. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            health_enabled = false;
        }
    }

    if (!health_enabled || !has_health) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health disabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_steps_or_sleep_layer_text("");
        set_dist_or_deep_layer_text("");
        set_health_icon_text("");
        health_service_events_unsubscribe();
    }
}

bool is_user_sleeping() {
    HealthActivityMask activities = health_service_peek_current_activities();
    APP_LOG(APP_LOG_LEVEL_DEBUG,
        "Sleeping data. %d %d %d",
        (int)activities & HealthActivitySleep,
        (int)activities & HealthActivityRestfulSleep,
        (int)activities & HealthActivitySleep || activities & HealthActivityRestfulSleep);
    return activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
}

void show_sleep_data_if_visible() {
    if (health_enabled && sleep_data_enabled) {
        bool is_sleeping = is_user_sleeping();

        if (is_sleeping) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We are asleep. %d", was_asleep);
            sleep_data_visible = true;
            if (!was_asleep) {
                was_asleep = true;
                woke_up_at = 0;
                queue_health_update();
                APP_LOG(APP_LOG_LEVEL_DEBUG, "Just went to sleep. %d", was_asleep);
            }
        }

        if (!is_sleeping && was_asleep) {
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

#else // Health not available

void toggle_health(bool from_configs) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Health disabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
    set_steps_or_sleep_layer_text("");
    set_dist_or_deep_layer_text("");
    set_health_icon_text("");
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

#endif
