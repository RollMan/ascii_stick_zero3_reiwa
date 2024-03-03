#ifndef __FMT_H__
#define __FMT_H__

#define DEBUG_SEND_STR(msg) {const uint8_t m[] = (msg); debug_send_word(m, sizeof(m) / sizeof(m[0]));}

void debug_send_byte(const uint8_t);
void debug_send_word(const uint8_t*, const uint8_t);
void debug_send_hex(const uint8_t);

#endif /* __FMT_H__ */
