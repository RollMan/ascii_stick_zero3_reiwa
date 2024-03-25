/*
 *  Project     Arduino XInput Library
 *  @author     David Madison
 *  @link       github.com/dmadison/ArduinoXInput
 *  @license    MIT - Copyright (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Example:      Blink
 *  Description:  Using XInput, presses and then releases the "A" button
 *                every two seconds. Good for testing that the XInput
 *                library is working correctly.
 */

#include <XInput.h>

void init_ports(){
    /*
   * IO ports should not be used indirectly.
   * https://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_port_pass
   */

  /*
   * Configuration of PORTB.
   *            7      6      5      4     3       2       1      0
   *         none,  left,  left,  down,   up,    n/a,    n/a,   n/a
   */
  const uint8_t PORTB_BITPOS = 0x78;
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
   *          b13,   b12,   b11,   b10,  none,  none,    b9,    low
   */
  const uint8_t PORTF_BITPOS = 0xF2;
  DDRF = ~PORTF_BITPOS;
  PORTF = PORTF_BITPOS;
  pinMode(A5, OUTPUT);
  digitalWrite(A5, LOW);
}

void setup() {
  Serial.end();
  ADCSRA = 0;
	XInput.begin();
  init_ports();
}

#define PRESS(pin, pinidx, button) \
  do { \
    if ((pin) & (1 << (pinidx))) XInput.release((button)); \
    if (~(pin) & (1 << (pinidx))) XInput.press((button)); \
  } while(0)

void loop() {
  PRESS(PINB, PINB4, DPAD_DOWN);
  PRESS(PINB, PINB5, DPAD_RIGHT);
  PRESS(PINB, PINB6, DPAD_UP);
  PRESS(PINC, PINC6, DPAD_LEFT);
  PRESS(PIND, PIND4, TRIGGER_RIGHT);
  // PIND5 is XCK1: USART1 external clock.
  PRESS(PIND, PIND6, BUTTON_LB);
  PRESS(PIND, PIND7, BUTTON_RB);
  PRESS(PIND, PIND0, BUTTON_A);
  PRESS(PIND, PIND1, BUTTON_X);
  PRESS(PIND, PIND2, BUTTON_B);
  PRESS(PIND, PIND3, BUTTON_Y);
  PRESS(PINF, PINF1, BUTTON_START);
  PRESS(PINF, PINF4, BUTTON_BACK);
}
