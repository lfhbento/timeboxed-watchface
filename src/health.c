#include <pebble.h>
#include "keys.h"
#include "health.h"
#include "text.h"


#if defined(PBL_HEALTH)
static bool health_enabled;
static int woke_up_at;
static bool was_asleep;
static bool sleep_data_visible;

static void update_steps_data() {

    HealthMetric metric_steps = HealthMetricStepCount;
    HealthMetric metric_dist = HealthMetricWalkedDistanceMeters;
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    uint16_t current_steps = 0;
    uint16_t current_dist = 0;
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

        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%d", current_steps);

        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    if (mask_dist & HealthServiceAccessibilityMaskAvailable) {
        bool useKm = persist_exists(KEY_USEKM) && persist_read_int(KEY_USEKM);
        current_dist = (int)health_service_sum_today(metric_dist);
        if (!useKm) {
            current_dist /= 1.6;
        }
        current_dist_int = current_dist/1000;
        current_dist_dec = (current_dist%1000)/100;

        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), (useKm ? "%d.%dkm" : "%d.%dmi"), current_dist_int, current_dist_dec);
        
        set_dist_or_deep_layer_text(dist_or_deep_text);
    }
    
    HealthActivityMask activities = health_service_peek_current_activities();
    bool is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sleeping data. %d %d %d", (int)activities & HealthActivitySleep, (int)activities & HealthActivityRestfulSleep, (int)is_sleeping);
    if (is_sleeping) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "We are asleep. %d", was_asleep);
        if (!was_asleep) {
            was_asleep = true;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Just went to sleep. %d", was_asleep);
        }
    }
    persist_write_string(KEY_STEPS, steps_or_sleep_text);
    persist_write_string(KEY_DIST, dist_or_deep_text);
}

static void update_sleep_data() {
    HealthMetric metric_sleep = HealthMetricSleepSeconds;
    HealthMetric metric_deep = HealthMetricSleepRestfulSeconds;
    
    time_t start = time_start_of_today();
    time_t end = time(NULL);

    uint16_t current_sleep = 0;
    uint16_t current_deep = 0;
    char steps_or_sleep_text[16];
    char dist_or_deep_text[16];
    
    HealthServiceAccessibilityMask mask_sleep =
        health_service_metric_accessible(metric_sleep, start, end);
    HealthServiceAccessibilityMask mask_deep =
        health_service_metric_accessible(metric_deep, start, end);

    if (mask_sleep & HealthServiceAccessibilityMaskAvailable) {
        current_sleep = (int)health_service_sum(metric_sleep, start, end);
        int current_sleep_hours = current_sleep / SECONDS_PER_HOUR;
        int current_sleep_minutes = (current_sleep - (current_sleep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(steps_or_sleep_text, sizeof(steps_or_sleep_text), "%dh%dm", current_sleep_hours, current_sleep_minutes);
        
        set_steps_or_sleep_layer_text(steps_or_sleep_text);
    }

    if (mask_deep & HealthServiceAccessibilityMaskAvailable) {
        current_deep = (int)health_service_sum(metric_deep, start, end);
        int current_deep_hours = current_deep / SECONDS_PER_HOUR;
        int current_deep_minutes = (current_deep - (current_deep_hours * SECONDS_PER_HOUR))/SECONDS_PER_MINUTE;
        
        snprintf(dist_or_deep_text, sizeof(dist_or_deep_text), "%dh%dm", current_deep_hours, current_deep_minutes);
        
        set_dist_or_deep_layer_text(dist_or_deep_text);
    }

}

void get_health_data() {
    if (health_enabled) {
        if (!sleep_data_visible) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating steps data. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
            update_steps_data();
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
            get_health_data(context);
            break;
    }
}

void toggle_health(bool from_configs) {
    bool has_health = false;
    health_enabled = persist_read_int(KEY_ENABLEHEALTH);
    if (health_enabled) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health enabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        has_health = health_service_events_subscribe(health_handler, NULL);
        set_steps_or_sleep_layer_text("0");
        set_dist_or_deep_layer_text("0");
        
    } else {
        has_health = false;
        health_service_events_unsubscribe();
    }

    if (!health_enabled || !has_health) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health disabled. %d%d", (int)time(NULL), (int)time_ms(NULL, NULL));
        set_steps_or_sleep_layer_text("");
        set_dist_or_deep_layer_text("");
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
    bool sleep_data_enabled = persist_exists(KEY_SHOWSLEEP) && persist_read_int(KEY_SHOWSLEEP);
    if (health_enabled && sleep_data_enabled) {
        bool is_sleeping = is_user_sleeping();
        
        if (!is_sleeping && was_asleep) {
            sleep_data_visible = true;
            woke_up_at = time(NULL) + SECONDS_PER_MINUTE * 30; //half an hour
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We woke up! %d", (int) woke_up_at);
            was_asleep = false;
            get_health_data();
        }

        if (sleep_data_visible && time(NULL) > woke_up_at) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Past half an hour after wake up! %d - %d", (int) time(NULL), (int) woke_up_at);
            sleep_data_visible = false;
            get_health_data();
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

#endif
