#include "button.h"
#include "usb.h"
#include <avr/io.h>

int main(void) {
  init_buttons();
  usb_power_on();
  return 0;
}
