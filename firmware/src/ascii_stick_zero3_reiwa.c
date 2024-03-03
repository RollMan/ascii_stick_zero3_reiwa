#include "button.h"
#include "debug/fmt.h"
#include "usb.h"
#include <avr/io.h>

int main(void) {
  DDRB |= (1 << DDB3);
  DEBUG_SEND_STR("power on.\r\n");
  init_buttons();
  DDRB |= (1 << DDB3);
  DEBUG_SEND_STR("button initialized.\r\n");
  usb_power_on();
  DEBUG_SEND_STR("usb powered on.\r\n");
  while (1)
    ;
}
