#include <pebble.h>
#include "positions.h"
#include "keys.h"


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

static void get_text_positions_blocko(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(46, midpoint - 46));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, midpoint + 6));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, midpoint - 46));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(120, midpoint + 28));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(70, get_pos(align, midpoint - 24, midpoint + 34, midpoint - 24)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(88, get_pos(align, midpoint - 6, midpoint + 34, midpoint - 6)));
}

static void get_text_positions_blocko_big(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 2, 0)), PBL_IF_ROUND_ELSE(40, midpoint - 52));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(96, midpoint + 4));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -2)), PBL_IF_ROUND_ELSE(42, midpoint - 50));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 1, -4)), PBL_IF_ROUND_ELSE(124, midpoint + 32));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, midpoint - 30, midpoint + 40, midpoint - 30)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, midpoint - 8, midpoint + 40, midpoint - 8)));
}

static void get_text_positions_system(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(54, midpoint - 42));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, midpoint + 2));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, midpoint - 50));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(124, midpoint + 28));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, midpoint - 30, midpoint + 40, midpoint - 30)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, midpoint - 8, midpoint + 40, midpoint - 8)));
}

static void get_text_positions_archivo(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(48, midpoint - 44));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(100, midpoint + 8));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(44, midpoint - 50));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(126, midpoint + 34));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, midpoint - 30, midpoint + 40, midpoint - 30)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, midpoint - 8, midpoint + 40, midpoint - 8)));
}

static void get_text_positions_din(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(47, midpoint - 45));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(99, midpoint + 8));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(42, midpoint - 52));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(122, midpoint + 32));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(64, get_pos(align, midpoint - 30, midpoint + 40, midpoint - 30)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(86, get_pos(align, midpoint - 8, midpoint + 40, midpoint - 8)));
}

static void get_text_positions_prototype(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(54, midpoint - 40));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(100, midpoint + 8));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(48, midpoint - 46));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(122, midpoint + 30));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -2, 126, -2)), PBL_IF_ROUND_ELSE(70, get_pos(align, midpoint - 24, midpoint + 34, midpoint - 24)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -2, 112, -2)), PBL_IF_ROUND_ELSE(88, get_pos(align, midpoint - 6, midpoint + 34, midpoint - 6)));
}

static void get_text_positions_leco(GTextAlignment align, struct TextPositions* positions, int width, int height) {
    int midpoint = height / 2;
    positions->hours = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, 0)), PBL_IF_ROUND_ELSE(46, midpoint - 41));
    positions->date = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(98, midpoint + 6));
    positions->alt_time = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -2)), PBL_IF_ROUND_ELSE(46, midpoint - 46));
    positions->battery = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, 2, 0, -4)), PBL_IF_ROUND_ELSE(120, midpoint + 28));
    positions->bluetooth = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 126, 0)), PBL_IF_ROUND_ELSE(70, get_pos(align, midpoint - 24, midpoint + 34, midpoint - 24)));
    positions->updates = create_point(PBL_IF_ROUND_ELSE(0, get_pos(align, -4, 112, 0)), PBL_IF_ROUND_ELSE(88, get_pos(align, midpoint - 6, midpoint + 34, midpoint - 6)));
}

void get_text_positions(int selected_font, GTextAlignment alignment, struct TextPositions* positions, int width, int height) {
    switch(selected_font) {
        case BLOCKO_FONT:
            get_text_positions_blocko(alignment, positions, width, height);
            break;
        case BLOCKO_BIG_FONT:
            get_text_positions_blocko_big(alignment, positions, width, height);
            break;
        case SYSTEM_FONT:
            get_text_positions_system(alignment, positions, width, height);
            break;
        case ARCHIVO_FONT:
            get_text_positions_archivo(alignment, positions, width, height);
            break;
        case DIN_FONT:
            get_text_positions_din(alignment, positions, width, height);
            break;
        case PROTOTYPE_FONT:
            get_text_positions_prototype(alignment, positions, width, height);
            break;
        case LECO_FONT:
            get_text_positions_leco(alignment, positions, width, height);
            break;
        default:
            get_text_positions_blocko(alignment, positions, width, height);
    }
};

static GPoint get_weather_positions(int mode, int font, int width, int height) {
    // weather condition
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-14, 0), 0);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-14, 0), 0);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-14, 0), 0);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-14, 0), 0);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-14, 0), 0);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-16, 0), 0);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(-16, 0), 0);
            }
            break;
        default:
            return create_point(0, 0);
    }

    return create_point(0, 0);
};

static GPoint get_temp_positions(int mode, int font, int width, int height) {
    // current temperature
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 38), 3);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 38), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 38), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 38), 3);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 38), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(18, 40), 3);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(16, 40), 3);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_tempmin_positions(int mode, int font, int width, int height) {
    // min temperature
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(70, 12), 3);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_tempmax_positions(int mode, int font, int width, int height) {
    // max temperature
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(108, 45), 3);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

#if defined(PBL_HEALTH)
static GPoint get_steps_positions(int mode, int font, int width, int height) {
    // steps
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(0, 3);
                case BLOCKO_BIG_FONT:
                    return create_point(0, 3);
                case SYSTEM_FONT:
                    return create_point(0, 3);
                case ARCHIVO_FONT:
                    return create_point(0, 3);
                case DIN_FONT:
                    return create_point(0, 3);
                case PROTOTYPE_FONT:
                    return create_point(0, 3);
                case LECO_FONT:
                    return create_point(0, 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_dist_positions(int mode, int font, int width, int height) {
    // distance
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(0, 3);
                case BLOCKO_BIG_FONT:
                    return create_point(0, 3);
                case SYSTEM_FONT:
                    return create_point(0, 3);
                case ARCHIVO_FONT:
                    return create_point(0, 3);
                case DIN_FONT:
                    return create_point(0, 3);
                case PROTOTYPE_FONT:
                    return create_point(0, 3);
                case LECO_FONT:
                    return create_point(0, 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_cal_positions(int mode, int font, int width, int height) {
    // calories
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(0, 3);
                case BLOCKO_BIG_FONT:
                    return create_point(0, 3);
                case SYSTEM_FONT:
                    return create_point(0, 3);
                case ARCHIVO_FONT:
                    return create_point(0, 3);
                case DIN_FONT:
                    return create_point(0, 3);
                case PROTOTYPE_FONT:
                    return create_point(0, 3);
                case LECO_FONT:
                    return create_point(0, 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_sleep_positions(int mode, int font, int width, int height) {
    // sleep
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(0, 3);
                case BLOCKO_BIG_FONT:
                    return create_point(0, 3);
                case SYSTEM_FONT:
                    return create_point(0, 3);
                case ARCHIVO_FONT:
                    return create_point(0, 3);
                case DIN_FONT:
                    return create_point(0, 3);
                case PROTOTYPE_FONT:
                    return create_point(0, 3);
                case LECO_FONT:
                    return create_point(0, 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_deep_positions(int mode, int font, int width, int height) {
    // deep sleep
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(0, 3);
                case BLOCKO_BIG_FONT:
                    return create_point(0, 3);
                case SYSTEM_FONT:
                    return create_point(0, 3);
                case ARCHIVO_FONT:
                    return create_point(0, 3);
                case DIN_FONT:
                    return create_point(0, 3);
                case PROTOTYPE_FONT:
                    return create_point(0, 3);
                case LECO_FONT:
                    return create_point(0, 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};
#endif

static GPoint get_speed_positions(int mode, int font, int width, int height) {
    // wind speed
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 3);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 6), 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_direction_positions(int mode, int font, int width, int height) {
    // wind direction
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 3);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(56, 4), 5);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

static GPoint get_wind_unit_positions(int mode, int font, int width, int height) {
    // wind unit
    switch (mode) {
        case MODE_NORMAL:
            switch(font) {
                case BLOCKO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 1);
                case BLOCKO_BIG_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 3);
                case SYSTEM_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 3);
                case ARCHIVO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 2);
                case DIN_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 3);
                case PROTOTYPE_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 1);
                case LECO_FONT:
                    return create_point(PBL_IF_ROUND_ELSE(100, 48), 1);
            }
            break;
        default:
            return create_point(0, 0);
    }
    
    return create_point(0, 0);
};

GPoint get_slot_positions(int mode, int slot, int width, int height) {
    switch (mode) {
        case MODE_NORMAL:
            switch(slot) {
                case SLOT_A:
                    return create_point(2, 0);
                case SLOT_B:
                    return create_point(PBL_IF_ROUND_ELSE(0, width / 2), PBL_IF_ROUND_ELSE(22, 0));
                case SLOT_C:
                    return create_point(2, PBL_IF_ROUND_ELSE(138, height - 26));
                case SLOT_D:
                    return create_point(PBL_IF_ROUND_ELSE(0, width / 2), PBL_IF_ROUND_ELSE(152, height - 26));
            }
            break;
        default:
            return create_point(0, 0);
    }

    return create_point(0, 0);
};

GPoint get_pos_for_item(int slot, int item, int mode, int font, int width, int height) {
    if (slot == -1) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid slot %d and item %d. Skipping. %d%03d", slot, item, (int)time(NULL), (int)time_ms(NULL, NULL));
        return create_point(0, 0);
    }
    GPoint slot_pos = get_slot_positions(mode, slot, width, height);
    GPoint item_pos;
    switch (item) {
        case 0:
            item_pos = get_weather_positions(mode, font, width, height);
            break;
        case 1:
            item_pos = get_temp_positions(mode, font, width, height);
            break;
        case 2:
            item_pos = get_tempmin_positions(mode, font, width, height);
            break;
        case 3:
            item_pos = get_tempmax_positions(mode, font, width, height);
            break;
        #if defined(PBL_HEALTH)
        case 4:
            item_pos = get_steps_positions(mode, font, width, height);
            break;
        case 5:
            item_pos = get_dist_positions(mode, font, width, height);
            break;
        case 6:
            item_pos = get_cal_positions(mode, font, width, height);
            break;
        case 7:
            item_pos = get_sleep_positions(mode, font, width, height);
            break;
        case 8:
            item_pos = get_deep_positions(mode, font, width, height);
            break;
        #endif
        case 9:
            item_pos = get_speed_positions(mode, font, width, height);
            break;
        case 10:
            item_pos = get_direction_positions(mode, font, width, height);
            break;
        case 11:
            item_pos = get_wind_unit_positions(mode, font, width, height);
            break;
    }
    return create_point(slot_pos.x + item_pos.x, slot_pos.y + item_pos.y);
}
