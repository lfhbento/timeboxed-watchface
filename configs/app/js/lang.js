const lang = {};

const locales = [
    'en_US',
    'pt_BR',
    'fr_FR',
    'de_DE',
    'es_ES',
    'it_IT',
    'nl_NL',
    'da_DK',
    'tr_TR',
    'cs_CZ',
    'pl_PL',
    'sv_SE',
    'fi_FI',
    'sl_SK',
];

const getText = (locale, text, params = {}) => {
    if (locale === 'en_US' || !lang[text] || !lang[text][locale]) {
        return replaceParams(text, params);
    }
    return replaceParams(lang[text][locale], params);
};

const replaceParams = (text, params) => {
    return Object.keys(params).reduce((str, key) => {
        return str.replace(`\${${key}}`, params[key]);
    }, text);
};

const getLocaleById = (loc) => {
    return locales[parseInt(loc, 10)];
};

export { getText, getLocaleById };
