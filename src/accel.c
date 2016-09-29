#include <pebble.h>
#include "screen.h"
#include "configs.h"
#include "keys.h"

static bool initialized;
static int lastPassZ;
static int lastZ;
static bool begin_tap;
static bool mid_tap;
static bool end_tap;
static bool neg;
static int mid_tap_count;
static int end_tap_count;
static bool show_tap_mode;
static int timeout_sec;

static Window *watchface_ref;

static void reset_tap() {
    begin_tap = false;
    mid_tap = false;
    end_tap = false;
    end_tap_count = 0;
    mid_tap_count = 0;
}

static void reset_tap_handler(void * data) {
    show_tap_mode = false;
    redraw_screen(watchface_ref);
}

void accel_data_handler(AccelData *data, uint32_t num_samples) {
    if (show_tap_mode || data[0].did_vibrate || data[1].did_vibrate) {
        return;
    }

    int passZ[2];
    int Z[2];
    int X[2];
    int Y[2];

    if (!initialized) {
        passZ[0] = passZ[1] = data[0].z;
        Z[0] = Z[1] = data[0].z;
        initialized = true;
        show_tap_mode = false;
        reset_tap();
    } else {
        passZ[0] = passZ[1] = lastPassZ;
        Z[0] = Z[1] = lastZ;
    }

    X[0] = X[1] = data[0].x;
    Y[0] = Y[1] = data[0].y;

    float a = 0.2;
    int high_threshold = 40;
    int low_threshold = 10;
    int range = 2;
    int x_threshold = 350;
    int y_threshold = 150;
    for (int i = 1; i < (int)num_samples + 1; ++i) {
        Z[1] = data[i-1].z;
        X[1] = data[i-1].x;
        Y[1] = data[i-1].y;

        passZ[1] = (int) (a * (passZ[0] + Z[1] - Z[0]));

        if (!end_tap && mid_tap) {
            if (end_tap_count < 3*range) {
                end_tap_count++;
                if (abs(passZ[1]) <= abs(low_threshold)) {
                    end_tap = true;
                }
            } else {
                reset_tap();
            }
        }

        if (!mid_tap && begin_tap) {
            if (mid_tap_count < range) {
                mid_tap_count++;
                if ((neg && passZ[1] >= high_threshold) || (!neg && passZ[1] <= -1*high_threshold)) {
                    mid_tap = true;
                }
            } else {
                reset_tap();
            }
        }

        if (
            (abs(passZ[0]) <= low_threshold) &&
            ((abs(X[0]) <= x_threshold &&
                Y[0] <= y_threshold)) &&
            abs(passZ[1]) >= high_threshold &&
            !begin_tap
        ) {
            begin_tap = true;
            neg = passZ[1] < 0;
        }

        if (begin_tap && mid_tap && end_tap) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "this is a tap");
            show_tap_mode = true;
            app_timer_register(timeout_sec * 1000, reset_tap_handler, NULL);
            reset_tap();
            redraw_screen(watchface_ref);
        }
        passZ[0] = passZ[1];
        Z[0] = Z[1];
        X[0] = X[1];
        Y[0] = Y[1];
    }

    lastPassZ = passZ[1];
    lastZ = Z[1];
}

bool tap_mode_visible() {
    return show_tap_mode;
}

void init_accel_service(Window * watchface) {
    timeout_sec = persist_exists(KEY_TAPTIME) ? persist_read_int(KEY_TAPTIME) : 7;
    if (is_tap_enabled()) {
        watchface_ref = watchface;
        accel_data_service_subscribe(5, accel_data_handler);
    } else {
        accel_data_service_unsubscribe();
    }
}
