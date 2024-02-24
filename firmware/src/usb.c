#include "descriptor.h"
#include "usb.h"
#include <avr/interrupt.h>
#include <avr/io.h>

const uint8_t GET_STATUS = 0x00;
const uint8_t CLEAR_FEATURE = 0x01;
const uint8_t SET_FEATURE = 0x03;
const uint8_t SET_ADDRESS = 0x05;
const uint8_t GET_DESCRIPTOR = 0x06;
const uint8_t SET_DESCRIPTOR = 0x07;
const uint8_t GET_CONFIGURATION = 0x08;
const uint8_t SET_CONFIGURATION = 0x09;

typedef enum {
  G_VBUST,
  G_UPRSM,
  G_EROSM,
  G_WAKEUP,
  G_EROSTI,
  G_SOFI,
  G_SUSPI,
} genintsrc_t;

void usb_power_on() {
  cli();
  // Power-On USB pads regulator
  UHWCON |=
      (1
       << UVREGE); // Enable USB Regulator to use 5V power source of a USB host.

  // Configure PLL interface & enable PLL
  PLLFRQ = 0x4A;
  PLLCSR |= (1 << PINDIV) | (1 << PLLE);
  // Check PLL lock
  while (!(PLLCSR & (1 << PLOCK)))
    ;
  // Enable USB interface
  USBCON |= 1 << USBE;
  // Configure USB interface (USB speed, Endpoints configuration...)
  //     Speed configuration; disable low-speed mode.
  USBCON = ((1 << USBE) | (1 << OTGPADE) | (1 << VBUSTE)) |
           (USBCON & ~(1 << FRZCLK));
  UDCON &= ~(1 << LSM);
  // Wait for an interruption of USB VBUS information connection and
  // attach USB device by:
  //     UDCON &= ~(1 << DETACH);
  sei();
}

void handle_udint() {
  if (UDINT & (1 << EORSTI)) {
    /*
     * End of reset is detected.
     * Prepare to respond the first 64 bytes of the device descriptor on the
     * address of 0.
     */
    // Activate the endpoint 0.
    UENUM = 0;
    UECONX = (1 << EPEN);
    UECFG0X = 0;     // Control endpoint, OUT direction.
    UECFG1X |= 0x22; // 32 byte endpoint, allocate a memory for the endpoint.

    if (UESTA0X & (1 << CFGOK)) {
      while (1)
        ;
    }

    // Endpoint 0 is activated.

    // Reset the endpoint.
    UERST = 1;
    UERST = 0;

    // Confirm the receive-setup-packet interrupt is enabled.
    UEIENX = (1 << RXSTPE);

    // TODO: wait for an interrupt of receive-setup-packet and respond the
    // device descriptor.
  }
}

ISR(USB_GEN_vect) {

  // Handle the VBUS pad transition.
  if (USBINT & (1 << VBUSTI)) {
    USBINT &= ~(1 << VBUSTI);
    if (USBSTA & (1 << VBUS)) {
      UDCON &= ~(1 << DETACH); // Attach the USB bus.
    }
  }

  // Handle UDINT
  if (UDINT) {
    /*
     *  Do NOT `return` inside this block.
     *  Otherwise, UDINT is never cleard, which
     *  is described at the bottom of this block.
     */

    handle_udint();

    UDINT = 0; // Do not forget to clear the interruption bits.
  }
}

void send_descriptor(const uint8_t wValue, const uint8_t wIndex,
                     const uint8_t wLength) {
  uint8_t const * descriptor;
  uint8_t descriptor_length;
  switch (wValue & 0xFF00) {
  case 0x0100: // Return device descriptor
    descriptor = device_descriptor;
    descriptor_length = pgm_read_byte(device_descriptor);
    break;
  case 0x0200: // Return the configuration descriptor
    descriptor = configuration_descriptor;
    descriptor_length = CONFIGURATION_DESCRIPTOR_LENGTH;
    break;
  case 0x0400: // Return the interface descriptor
    descriptor = interface_descriptor;
    descriptor_length = INTERFACE_DESCRIPTOR_LENGTH;
    break;
  case 0x0500: // Return the Endpoint descriptor
    descriptor = endpoint_descriptor;
    descriptor_length = ENDPOINT_DESCRIPTOR_LENGTH;
    break;
  case 0x2100: // Return the HID descriptor
    descriptor = hid_descriptor;
    descriptor_length = HID_DESCRIPTOR_LENGTH;
    break;
  case 0x2200: // Return the  report descriptor
    descriptor = report_descriptor;
    descriptor_length = REPORT_DESCRIPTOR_LENGTH;
    break;
  default:
    // Unexpected descriptor type.
    UECONX |= (1 << STALLRQ);
    break;
  }
}

void handle_control_setup() {
  // reset STALL at SETUP PID.
  UECONX |= (1 << STALLRQC);
  // disable interrupts of OUTI/INI to process the DATA0 in this function.
  UEIENX &= ~(1 << RXOUTE) & ~(1 << TXINE);

  // begin of the setup packets
  const uint8_t bmRequestType = UEDATX;
  const uint8_t bRequest = UEDATX;
  const uint8_t wValue_l = UEDATX;
  const uint8_t wValue_h = UEDATX;
  const uint8_t wIndex_l = UEDATX;
  const uint8_t wIndex_h = UEDATX;
  const uint8_t wLength_l = UEDATX;
  const uint8_t wLength_h = UEDATX;

  const uint16_t wValue = ((uint16_t)(wValue_h) << 8) | wValue_l;
  const uint16_t wIndex = ((uint16_t)(wIndex_h) << 8) | wIndex_l;
  const uint16_t wLength = ((uint16_t)(wLength_h) << 8) | wLength_l;

  // clear RSTPI
  UEINTX &= ~(1 << RXSTPI);

  // clear the endpoint bank
  UEINTX &= ~(1 << FIFOCON);

  UENUM = 0;
  const uint8_t recipient = bmRequestType & 0x1F;
  if (recipient == 0x00) { // Handle a standard device request
    switch (bRequest) {
    case GET_STATUS:
      while (!(UEINTX & (1 << TXINI)))
        ; // wait for "data stage" as IN/OUT packets.
      UEINTX &= ~(1 << TXINI);
      UEDATX = 0x00;
      UEDATX = 0x00; // no remote wakeup for device, bus-powered.
      UEINTX &= ~(1 << FIFOCON);
      break;
    case CLEAR_FEATURE:
      // Clear feature is not supported:
      // It is not allowed to change disabled remote-wakeup and bus-powered.
      UECONX |= (1 << STALLRQ);
      return;
      break;
    case SET_FEATURE:
      // Set feature is not supported:
      // It is not allowed to change disabled remote-wakeup and bus-powered.
      UECONX |= (1 << STALLRQ);
      break;
    case SET_ADDRESS:
      UDADDR = wValue & ~(1 << ADDEN);
      while (!(UEINTX & (1 << TXINI)))
        ; // wait for "data stage" as IN/OUT packets.
      UEINTX &=
          ~(1 << TXINI) &
          ~(1 << FIFOCON); // send IN 0 Zero Length Packet & an ACK status.
      UDADDR |= (1 << ADDEN);
      break;
    case GET_DESCRIPTOR:
      send_descriptor(wValue, wIndex, wLength);
      break;
    case SET_DESCRIPTOR:
      // Set descriptor is not supported:
      // Follows
      // https://github.com/kmani314/ATMega32u4-HID-Keyboard/blob/master/src/usb.c
      UECONX |= (1 << STALLRQ);
      break;
    case GET_CONFIGURATION:
      break;
    case SET_CONFIGURATION:
      break;
    default:
      // Unexpected request.
      // Follows
      // https://github.com/kmani314/ATMega32u4-HID-Keyboard/blob/master/src/usb.c
      UECONX |= (1 << STALLRQ);
      break;
    }
  } else if (recipient == 0x01) { // Handle a standard interface request
  } else if (recipient == 0x02) { // Handle a standard endpoint request
  }
}

ISR(USB_COM_vect) {
  if (UEINTX & (1 << RXSTPI)) {
    handle_control_setup();
  }
}
