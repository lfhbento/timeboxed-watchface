#include <pebble.h>
#include <ctype.h>
#include "keys.h"
#include "locales.h"
#include "text.h"

uint8_t selected_locale;
uint8_t selected_format;

static char* WEEKDAYS[14][7] = {
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}, // en_US
    {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"}, // pt_BR
    {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"}, // fr_FR
    {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"}, // de_DE
    {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"}, // es_ES
    {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"}, // it_IT
    {"Zo", "Ma", "Di", "Wo", "Do", "Vr", "Za"}, // nl_NL
    {"SON", "MAN", "TIR", "ONS", "TOR", "FRE", "LOR"}, // da_DK
    {"PAZ", "PTS", "SAL", "CAR", "PER", "CUM", "CTS"}, // tr_TR
    {"NE", "PO", "UT", "ST", "CT", "PA", "SO"}, // cs_CZ
    {"NDZ", "PON", "WTO", "SRO", "CZW", "PTK", "SOB"}, // pl_PL
    {"SON", "MAN", "TIS", "ONS", "TOR", "FRE", "LOR"}, // sv_SE
    {"SU", "MA", "TI", "KE", "TO", "PE", "LA"}, // fi_FI
    {"NE", "PO", "UT", "ST", "ST", "PI", "SO"}, // sk_SK
};

static char* MONTHS[14][12] = {
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}, // en_US
    {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"}, // pt_BR
    {"Jan", "Fev", "Mar", "Avr", "Mai", "Jui", "Jul", "Aou", "Sep", "Oct", "Nov", "Dec"}, // fr_FR
    {"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"}, // de_DE
    {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"}, // es_ES
    {"Gen", "Feb", "Mar", "Apr", "Mag", "Giu", "Lug", "Ago", "Set", "Ott", "Nov", "Dic"}, // it_IT
    {"Jan", "Feb", "Mrt", "Apr", "Mei", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"}, // nl_NL
    {"Jan", "Feb", "Mar", "Apr", "Maj", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"}, // da_DK
    {"Oca", "Sub", "Mar", "Nis", "May", "Haz", "Tem", "Agu", "Eyl", "Eki", "Kas", "Ara"}, // tr_TR
    {"Led", "Uno", "Bre", "Dub", "Kve", "Crv", "Cvc", "Srp", "Zar", "Rij", "Lis", "Pro"}, // cs_CZ
    {"Sty", "Lut", "Mar", "Kwi", "Maj", "Cze", "Lip", "Sie", "Wrz", "Paz", "Lis", "Gru"}, // pl_PL
    {"Jan", "Feb", "Mar", "Apr", "Maj", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"}, // sv_SE
    {"Tam", "Hel", "Maa", "Huh", "Tou", "Kes", "Hei", "Elo", "Syy", "Lok", "Mar", "Jou"}, // fi_FI
    {"Jan", "Feb", "Mar", "Apr", "Maj", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"}, // sk_SK
};

static char* SEPARATORS[4] = {
    " ", ".", "-", "/"
};

void get_current_date(struct tm* tick_time, char* buffer, int buf_size, int separator) {
    char* weekday = WEEKDAYS[selected_locale][tick_time->tm_wday];
    char* month = MONTHS[selected_locale][tick_time->tm_mon];

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting date: %s %s %d.  %d%03d", weekday, month, tick_time->tm_mday, (int)time(NULL), (int)time_ms(NULL, NULL));
    switch(selected_format) {
        case FORMAT_WMD:
            snprintf(buffer, buf_size, "%s%s%s%s%02d", weekday, SEPARATORS[separator], month, SEPARATORS[separator], tick_time->tm_mday);
            break;
        case FORMAT_WDM:
            snprintf(buffer, buf_size, "%s%s%02d%s%s", weekday, SEPARATORS[separator], tick_time->tm_mday, SEPARATORS[separator], month);
            break;
    }
}

void load_locale() {
    selected_locale = persist_exists(KEY_LOCALE) ? persist_read_int(KEY_LOCALE) : LC_ENGLISH;
    selected_format = persist_exists(KEY_DATEFORMAT) ? persist_read_int(KEY_DATEFORMAT): FORMAT_WMD;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Locale loaded: %d %d.  %d%03d", selected_locale, selected_format, (int)time(NULL), (int)time_ms(NULL, NULL));
}
