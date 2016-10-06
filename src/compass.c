#include <pebble.h>
#include "screen.h"
#include "configs.h"
#include "keys.h"
#include "text.h"
#include "weather.h"

#if defined PBL_COMPASS
void compass_handler(CompassHeadingData data) {
    CompassStatus status = data.compass_status;
    int heading;
    char degrees[6];
    char compass[2];
    switch(status) {
        case CompassStatusCalibrated:
        case CompassStatusCalibrating:
            heading = TRIGANGLE_TO_DEG(TRIG_MAX_ANGLE - (int)data.magnetic_heading);

            strcpy(compass, get_wind_direction((TRIGANGLE_TO_DEG((int)data.magnetic_heading) + 180) % 360));
            snprintf(degrees, sizeof(degrees), "%s", get_wind_direction_text((heading + 180) % 360));

            set_compass_layer_text(compass);
            set_degrees_layer_text(degrees);
            break;
        case CompassStatusUnavailable:
        case CompassStatusDataInvalid:
            set_compass_layer_text("N");
            set_degrees_layer_text("NA");
            break;
    }
}

void init_compass_service(Window * watchface) {
    if (is_module_enabled(MODULE_COMPASS)) {
        compass_service_subscribe((CompassHeadingHandler)compass_handler);
        compass_service_set_heading_filter(TRIG_MAX_ANGLE/64);
    } else {
        compass_service_unsubscribe();
    }
}
#endif
