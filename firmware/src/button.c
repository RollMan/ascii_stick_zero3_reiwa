#include "button.h"
#include <avr/io.h>

void init_buttons() {
  /*
   * IO ports should not be used indirectly.
   * https://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_port_pass
   */

  /*
   * Configuration of PORTB.
   *            7      6      5      4     3       2       1      0
   *         none,  left,  left,  down,   up,    n/a,    n/a,   n/a
   */
  const uint8_t PORTB_BITPOS = 0x70;
  DDRB &= ~PORTB_BITPOS;
  PORTB |= PORTB_BITPOS;

  /*
   * Configuration of PORTC.
   *            7      6      5      4     3      2       1      0
   *         none,  right, none,  none, none,  none,   none,  none
   */
  const uint8_t PORTC_BITPOS = 0x40;
  DDRC &= ~PORTC_BITPOS;
  PORTC |= PORTC_BITPOS;

  /*
   * Configuration of PORTD.
   *            7      6      5      4     3      2       1      0
   *           b7,    b6,    b5,    b4,    b3,    b2,    b1,    b0
   */
  DDRD = 0x00;
  PORTD = 0xFF;

  /*
   * Configuration of PORTF.
   *            7      6      5      4     3      2       1      0
   *          b13,   b12,   b11,   b10,  none,  none,    b9,    b8
   */
  const uint8_t PORTF_BITPOS = 0xF3;
  DDRF &= ~PORTF_BITPOS;
  PORTF |= PORTF_BITPOS;
}
