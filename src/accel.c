#include <pebble.h>

static bool initialized;
static int lastPassZ;
static int lastZ;
static bool is_tap;
static bool begin_tap;
static bool mid_tap;
static bool end_tap;
static int mid_tap_count;
static int end_tap_count;
static int hit[12];
static int hit_count;
AppTimer * timer;

static void reset_tap() {
    begin_tap = false;
    mid_tap = false;
    end_tap = false;
    end_tap_count = 0;
    mid_tap_count = 0;
    if (timer) {
        app_timer_cancel(timer);
    }
}

static void reset_tap_handler(void * data) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "resetting tap detection");
    reset_tap();
    is_tap = false;
}

void accel_data_handler(AccelData *data, uint32_t num_samples) {
    int passZ[2];
    int Z[2];

    if (!initialized) {
        passZ[0] = passZ[1] = data[0].z;
        Z[0] = Z[1] = data[0].z;
        initialized = true;
        is_tap = false;
        reset_tap();
        hit[0] = hit[1] = hit[2] = hit[3] = hit[4] = hit[5] = hit[6] = hit[7] = hit[8] = hit[9] = hit[10] = hit[11] = 0;
        hit_count = 0;
    } else {
        passZ[0] = passZ[1] = lastPassZ;
        Z[0] = Z[1] = lastZ;
    }

    float a = 0.2;
    int high_threshold = 70;
    int low_threshold = 15;
    int range = 3;
    int timeout = 800;
    if (hit_count == 12) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Z> %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", hit[0], hit[1], hit[2], hit[3], hit[4], hit[5], hit[6], hit[7], hit[8], hit[9], hit[10], hit[11]);
        hit[0] = hit[1] = hit[2] = hit[3] = hit[4] = hit[5] = hit[6] = hit[7] = hit[8] = hit[9] = hit[10] = hit[11] = 0;
        hit_count = 0;
    }
    hit[hit_count++] = passZ[0];
    for (int i = 1; i < (int)num_samples + 1; ++i) {
        if (hit_count == 12) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Z> %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", hit[0], hit[1], hit[2], hit[3], hit[4], hit[5], hit[6], hit[7], hit[8], hit[9], hit[10], hit[11]);
            hit[0] = hit[1] = hit[2] = hit[3] = hit[4] = hit[5] = hit[6] = hit[7] = hit[8] = hit[9] = hit[10] = hit[11] = 0;
            hit_count = 0;
        }
        Z[1] = data[i-1].z;

        passZ[1] = (int) (a * (passZ[0] + Z[1] - Z[0]));

        if (!end_tap && mid_tap) {
            if (end_tap_count < range) {
                if (passZ[0] < -1*high_threshold && abs(passZ[i]) < low_threshold) {
                    end_tap = true;
                }
                end_tap_count++;
                //hit[hit_count++] = passZ[1];
            } else {
                reset_tap();
            }
        }

        if (!mid_tap && begin_tap) {
            if (mid_tap_count < range) {
                if (passZ[1] <= -1*high_threshold) {
                    mid_tap = true;
                }
                mid_tap_count++;
                //hit[hit_count++] = passZ[1];
            } else {
                reset_tap();
            }
        }

        if (abs(passZ[0]) <= low_threshold && passZ[1] > high_threshold && !begin_tap) {
            begin_tap = true;
            //hit[hit_count++] = passZ[0];
            //hit[hit_count++] = passZ[1];
        }

        if (begin_tap && mid_tap && end_tap) {
            if (!is_tap) {
                is_tap = true;
                if (!timer) {
                    app_timer_register(timeout, reset_tap_handler, NULL);
                } else {
                    bool reschedule = app_timer_reschedule(timer, timeout);
                    if (!reschedule) {
                        app_timer_register(timeout, reset_tap_handler, NULL);
                    }
                }
                APP_LOG(APP_LOG_LEVEL_DEBUG, "this is a tap");
            } else {
                is_tap = false;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "this is a second tap");
                vibes_short_pulse();
            }
            reset_tap();
        }
        passZ[0] = passZ[1];
        Z[0] = Z[1];
        hit[hit_count++] = passZ[1];
    }


    lastPassZ = passZ[1];
    lastZ = Z[1];
}
