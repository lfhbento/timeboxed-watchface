#ifndef __TIMEBOXED_COMPASS
#define __TIMEBOXED_COMPASS

#if defined PBL_COMPASS
void compass_handler(CompassHeadingData heading);
void init_compass_service(Window * watchface);
#endif

#endif
