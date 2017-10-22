#ifndef __TIMEBOXED_CRYPTO_
#define __TIMEBOXED_CRYPTO_

#include <pebble.h>

bool is_crypto_enabled();
void update_crypto_price(char*);
void update_crypto_price_b(char*);
void update_crypto_price_c(char*);
void update_crypto_price_d(char*);
void toggle_crypto(uint8_t);
void store_crypto_price(char*);
void store_crypto_price_b(char*);
#if !defined PBL_PLATFORM_APLITE
void store_crypto_price_c(char*);
void store_crypto_price_d(char*);
#endif
void update_crypto(bool);

#endif
