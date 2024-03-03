#include "fmt.h"
#include "suart.h"
#include <avr/io.h>

uint8_t int_to_hex_char(const uint8_t v) {
  if (0x00 <= v && v <= 0x09) {
    return v + '0';
  } else if (0x0A <= v && v <= 0x0F) {
    return v + '0' + 7;
  } else {
    return v + '0';
  }
}

void debug_send_byte(const uint8_t v) { xmit(v); }

void debug_send_word(const uint8_t *v, const uint8_t len) {
  for (int i = 0; i < len; i++) {
    debug_send_byte(v[i]);
  }
}

#define HEX_LEN 2
void debug_send_hex(uint8_t v) {
  uint8_t str[HEX_LEN];
  int8_t digits = 1;
  while (digits >= 0) {
    const uint8_t c = int_to_hex_char(v & 0x0F);
    str[digits] = c;
    v >>= 4;
    digits--;
  }
  debug_send_word(str, HEX_LEN);
}
