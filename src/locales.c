#include <pebble.h>
#include <ctype.h>
#include "keys.h"
#include "locales.h"

uint8_t selected_locale;
uint8_t selected_format;

static char* WEEKDAYS[14][7] = {
    {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"}, // en_US
    {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"}, // pt_BR
    {"DIM", "LUN", "MAR", "MER", "JEU", "VEN", "SAM"}, // fr_FR
    {"SO", "MO", "DI", "MI", "DO", "FR", "SA"}, // de_DE
    {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"}, // es_ES
    {"DOM", "LUN", "MAR", "MER", "GIO", "VEN", "SAB"}, // it_IT
    {"ZO", "MA", "DI", "WO", "DO", "VR", "ZA"}, // nl_NL
    {"SON", "MAN", "TIR", "ONS", "TOR", "FRE", "LOR"}, // da_DK
    {"PAZ", "PTS", "SAL", "CAR", "PER", "CUM", "CTS"}, // tr_TR
    {"NE", "PO", "UT", "ST", "CT", "PA", "SO"}, // cs_CZ
    {"NDZ", "PON", "WTO", "SRO", "CZW", "PTK", "SOB"}, // pl_PL
    {"SON", "MAN", "TIS", "ONS", "TOR", "FRE", "LOR"}, // sv_SE
    {"SU", "MA", "TI", "KE", "TO", "PE", "LA"}, // fi_FI
    {"NE", "PO", "UT", "ST", "ST", "PI", "SO"}, // sk_SK
};

static char* MONTHS[14][12] = {
    {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"}, // en_US
    {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN", "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"}, // pt_BR
    {"JAN", "FEV", "MAR", "AVR", "MAI", "JUI", "JUL", "AOU", "SEP", "OCT", "NOV", "DEC"}, // fr_FR
    {"JAN", "FEB", "MAR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEZ"}, // de_DE
    {"ENE", "FEB", "MAR", "ABR", "MAY", "JUN", "JUL", "AGO", "SEP", "OCT", "NOV", "DIC"}, // es_ES
    {"GEN", "FEB", "MAR", "APR", "MAG", "GIU", "LUG", "AGO", "SET", "OTT", "NOV", "DIC"}, // it_IT
    {"JAN", "FEB", "MRT", "APR", "MEI", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"}, // nl_NL
    {"JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"}, // da_DK
    {"OCA", "SUB", "MAR", "NIS", "MAY", "HAZ", "TEM", "AGU", "EYL", "EKI", "KAS", "ARA"}, // tr_TR
    {"LED", "UNO", "BRE", "DUB", "KVE", "CRV", "CVC", "SRP", "ZAR", "RIJ", "LIS", "PRO"}, // cs_CZ
    {"STY", "LUT", "MAR", "KWI", "MAJ", "CZE", "LIP", "SIE", "WRZ", "PAZ", "LIS", "GRU"}, // pl_PL
    {"JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"}, // sv_SE
    {"TAM", "HEL", "MAA", "HUH", "TOU", "KES", "HEI", "ELO", "SYY", "LOK", "MAR", "JOU"}, // fi_FI
    {"JAN", "FEB", "MAR", "APR", "MAJ", "JUN", "JUL", "AUG", "SEP", "OKT", "NOV", "DEC"}, // sk_SK
};

void get_current_date(struct tm* tick_time, char* buffer, int buf_size) {
    char* weekday = WEEKDAYS[selected_locale][tick_time->tm_wday];
    char* month = MONTHS[selected_locale][tick_time->tm_mon];

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting date: %s %s %d.  %d%d", weekday, month, tick_time->tm_mday, (int)time(NULL), (int)time_ms(NULL, NULL));
    switch(selected_format) {
        case FORMAT_WMD:
            snprintf(buffer, buf_size, "%s.%s.%02d", weekday, month, tick_time->tm_mday);
            break;
        case FORMAT_WDM:
            snprintf(buffer, buf_size, "%s.%02d.%s", weekday, tick_time->tm_mday, month);
            break;
    }

    for (unsigned char i = 0; buffer[i]; ++i) {
        buffer[i] = tolower((unsigned char)buffer[i]);
    }
}

void load_locale() {
   selected_locale = persist_exists(KEY_LOCALE) ? persist_read_int(KEY_LOCALE) : LC_ENGLISH;
   selected_format = persist_exists(KEY_DATEFORMAT) ? persist_read_int(KEY_DATEFORMAT): FORMAT_WMD;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Locale loaded: %d %d.  %d%d", selected_locale, selected_format, (int)time(NULL), (int)time_ms(NULL, NULL));
}
