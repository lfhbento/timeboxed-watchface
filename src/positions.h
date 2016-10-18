#ifndef __TIMEBOXED_POSITIONS
#define __TIMEBOXED_POSITIONS

#include <pebble.h>

struct TextPositions {
    GPoint hours;
    GPoint date;
    GPoint bluetooth;
    GPoint updates;
};

GPoint create_point(int x, int y);
GPoint get_pos_for_item(int slot, int item, int mode, int font, int width, int height);
void get_text_positions(int selected_font, GTextAlignment alignment, struct TextPositions* positions, int width, int height);

#endif
