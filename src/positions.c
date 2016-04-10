#include <pebble.h>
#include "positions.h"
#include "keys.h"

static GPoint slot_positions[1][4];
static GPoint positions[9][1][6];

GPoint create_point(int x, int y) {
    struct GPoint point;
    point.x = x;
    point.y = y;
    return point;
}

static int get_pos(int alignment, int left_pos, int center_pos, int right_pos) {
    switch (alignment) {
        case GTextAlignmentLeft:
            return left_pos;
            break;
        case GTextAlignmentCenter:
            return center_pos;
            break;
        case GTextAlignmentRight:
            return right_pos;
            break;
    }
    return right_pos;
}

static void get_text_positions_blocko(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(46, 38));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, 90));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, 38));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(120, 112));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(70, get_pos(align, 60, 118, 60)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(88, get_pos(align, 78, 118, 78)));
}

static void get_text_positions_blocko_big(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 2, 0)), PBL_IF_ROUND_ELSE(40, 32));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(96, 88));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(42, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -4)), PBL_IF_ROUND_ELSE(124, 116));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, 54, 124, 54)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 76, 124, 76)));
}

static void get_text_positions_system(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(54, 42));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, 86));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(124, 112));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
}

static void get_text_positions_archivo(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(48, 40));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(100, 92));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(44, 34));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(126, 118));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
}

static void get_text_positions_din(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(47, 39));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(99, 92));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(42, 32));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(122, 116));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(68, get_pos(align, 56, 124, 56)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, 74, 124, 74)));
}

static void get_text_positions_prototype(GTextAlignment align, struct TextPositions* positions) {
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(54, 44));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(100, 92));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(48, 38));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(122, 114));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -2, 126, -2)), PBL_IF_ROUND_ELSE(70, get_pos(align, 60, 118, 60)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -2, 112, -2)), PBL_IF_ROUND_ELSE(88, get_pos(align, 78, 118, 78)));
}

void get_text_positions(int selected_font, GTextAlignment alignment, struct TextPositions* positions) {
    switch(selected_font) {
        case BLOCKO_FONT:
            get_text_positions_blocko(alignment, positions);
            break;
        case BLOCKO_BIG_FONT:
            get_text_positions_blocko_big(alignment, positions);
            break;
        case SYSTEM_FONT:
            get_text_positions_system(alignment, positions);
            break;
        case ARCHIVO_FONT:
            get_text_positions_archivo(alignment, positions);
            break;
        case DIN_FONT:
            get_text_positions_din(alignment, positions);
            break;
        case PROTOTYPE_FONT:
            get_text_positions_prototype(alignment, positions);
            break;
        default:
            get_text_positions_blocko(alignment, positions);
    }
};

void init_positions() {
    slot_positions[MODE_NORMAL][SLOT_A] = create_point(2, 0);
    slot_positions[MODE_NORMAL][SLOT_B] = create_point(PBL_IF_ROUND_ELSE(0, 72), PBL_IF_ROUND_ELSE(22, 0));
    slot_positions[MODE_NORMAL][SLOT_C] = create_point(2, PBL_IF_ROUND_ELSE(138, 142));
    slot_positions[MODE_NORMAL][SLOT_D] = create_point(PBL_IF_ROUND_ELSE(0, 72), PBL_IF_ROUND_ELSE(152, 142));

    // weather condition
    positions[WEATHER_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions[WEATHER_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions[WEATHER_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions[WEATHER_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions[WEATHER_ITEM][MODE_NORMAL][DIN_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);
    positions[WEATHER_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(PBL_IF_ROUND_ELSE(-14, 4), 0);

    // current temperature
    positions[TEMP_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
    positions[TEMP_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
    positions[TEMP_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
    positions[TEMP_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
    positions[TEMP_ITEM][MODE_NORMAL][DIN_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
    positions[TEMP_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(PBL_IF_ROUND_ELSE(16, 40), 3);

    // min temperature
    positions[TEMPMIN_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
    positions[TEMPMIN_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
    positions[TEMPMIN_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
    positions[TEMPMIN_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
    positions[TEMPMIN_ITEM][MODE_NORMAL][DIN_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
    positions[TEMPMIN_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(PBL_IF_ROUND_ELSE(70, 12), 3);

    // max temperature
    positions[TEMPMAX_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);
    positions[TEMPMAX_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);
    positions[TEMPMAX_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);
    positions[TEMPMAX_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);
    positions[TEMPMAX_ITEM][MODE_NORMAL][DIN_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);
    positions[TEMPMAX_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(PBL_IF_ROUND_ELSE(113, 45), 3);

    // steps
    positions[STEPS_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(0, 3);
    positions[STEPS_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(0, 3);
    positions[STEPS_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(0, 3);
    positions[STEPS_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(0, 3);
    positions[STEPS_ITEM][MODE_NORMAL][DIN_FONT] = create_point(0, 3);
    positions[STEPS_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(0, 3);

    // distance
    positions[DIST_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(0, 3);
    positions[DIST_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(0, 3);
    positions[DIST_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(0, 3);
    positions[DIST_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(0, 3);
    positions[DIST_ITEM][MODE_NORMAL][DIN_FONT] = create_point(0, 3);
    positions[DIST_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(0, 3);

    // calories
    positions[CAL_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(0, 3);
    positions[CAL_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(0, 3);
    positions[CAL_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(0, 3);
    positions[CAL_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(0, 3);
    positions[CAL_ITEM][MODE_NORMAL][DIN_FONT] = create_point(0, 3);
    positions[CAL_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(0, 3);

    // sleep
    positions[SLEEP_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(0, 3);
    positions[SLEEP_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(0, 3);
    positions[SLEEP_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(0, 3);
    positions[SLEEP_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(0, 3);
    positions[SLEEP_ITEM][MODE_NORMAL][DIN_FONT] = create_point(0, 3);
    positions[SLEEP_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(0, 3);

    // deep sleep
    positions[DEEP_ITEM][MODE_NORMAL][BLOCKO_FONT] = create_point(0, 3);
    positions[DEEP_ITEM][MODE_NORMAL][BLOCKO_BIG_FONT] = create_point(0, 3);
    positions[DEEP_ITEM][MODE_NORMAL][SYSTEM_FONT] = create_point(0, 3);
    positions[DEEP_ITEM][MODE_NORMAL][ARCHIVO_FONT] = create_point(0, 3);
    positions[DEEP_ITEM][MODE_NORMAL][DIN_FONT] = create_point(0, 3);
    positions[DEEP_ITEM][MODE_NORMAL][PROTOTYPE_FONT] = create_point(0, 3);
}

GPoint get_pos_for_item(int slot, int item, int mode, int font) {
    GPoint slot_pos = slot_positions[mode][slot];
    GPoint item_pos = positions[item][mode][font];
    return create_point(slot_pos.x + item_pos.x, slot_pos.y + item_pos.y);
}
