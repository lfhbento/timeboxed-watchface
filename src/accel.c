#include <pebble.h>

void accel_data_handler(AccelData *data, uint32_t num_samples) {
    int passX[10];
    int passY[10];
    int passZ[10];

    passX[0] = data[0].x;
    passY[0] = data[0].y;
    passZ[0] = data[0].z;
    APP_LOG(APP_LOG_LEVEL_DEBUG, " aaaa %d", passX[0]);

    float a = 0.5;
    for (int i = 1; i < (int)num_samples; ++i) {
        passX[i] = (int) (a * (passX[i-1] + data[i].x - data[i-1].x));
        passY[i] = (int) (a * (passY[i-1] + data[i].y - data[i-1].y));
        passZ[i] = (int) (a * (passZ[i-1] + data[i].z - data[i-1].z));
    }


    //APP_LOG(APP_LOG_LEVEL_DEBUG, "X> %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,", passX[0], passX[1], passX[2], passX[3], passX[4], passX[5], passX[6], passX[7], passX[8], passX[9]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Y> %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,", passY[0], passY[1], passY[2], passY[3], passY[4], passY[5], passY[6], passY[7], passY[8], passY[9]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Z> %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,", passZ[0], passZ[1], passZ[2], passZ[3], passZ[4], passZ[5], passZ[6], passZ[7], passZ[8], passZ[9]);
}
