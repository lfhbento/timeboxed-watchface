#ifndef __TIMEBOXED_LOCALE
#define __TIMEBOXED_LOCALE

static char* LOCALE_NAMES[13] = {
    "English",
    "Portuguese",
    "French",
    "German",
    "Spanish",
    "Italian",
    "Dutch",
    "Danish",
    "Turkish",
    "Czech",
    "Polish",
    "Swedish",
    "Finnish"
};

static char* WEEKDAYS[13][7] = {
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
    {"SU", "MA", "TI", "KE", "TO", "PE", "LA"} // fi_FI
};

static char* MONTHS[13][12] = {
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
};

static char* DATE_FORMATS[2] = {
    "%s.%s.%d", // weekday.month.day
    "%s.%d.%s" // weekday.day.month
};

#endif
