#include "usb.h"
#include "config.h"
#include "descriptor.h"
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
const uint8_t GET_INTERFACE = 0x0A;
const uint8_t SET_INTERFACE = 0x11;
const uint8_t SYNCH_FRAME = 0x12;

// HID Reqest
const uint8_t GET_REPORT = 0x01;
const uint8_t GET_IDLE = 0x02;
const uint8_t GET_PROTOCOL = 0x03;
const uint8_t SET_REPORT = 0x09;
const uint8_t SET_IDLE = 0x0A;
const uint8_t SET_PROTOCOL = 0x0B;

const uint8_t ENDPOINT_SIZE = 32;
const uint8_t ENDPOINT_SIZE_SEL = 0x22;

const uint8_t GAMEPAD_ENDPOINT_NUM = 3;

uint8_t usb_config_status;
uint16_t usb_interface_status;
uint16_t usb_idle_status;

typedef enum {
  G_VBUST,
  G_UPRSM,
  G_EROSM,
  G_WAKEUP,
  G_EROSTI,
  G_SOFI,
  G_SUSPI,
} genintsrc_t;

void send_zero_length_packet() {
  while (!(UEINTX & (1 << TXINI)))
    ; // wait for "data stage" as IN/OUT packets.
  UEINTX &= ~(1 << TXINI) &
            ~(1 << FIFOCON); // send IN 0 Zero Length Packet & an ACK status.
}

void send_ram_bytes(uint8_t const *const dat, uint8_t const len) {
  while (!(UEINTX & (1 << TXINI)))
    ;
  UEINTX &= ~(1 << TXINI);
  for (int i = 0; i < len; i++) {
    UEDATX = dat[i];
  }
  UEINTX &= ~(1 << FIFOCON);
}

void send_stall() { UECONX |= (1 << STALLRQ); }

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
  usb_config_status = 0;

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
    UECFG0X = 0;                  // Control endpoint, OUT direction.
    UECFG1X |= ENDPOINT_SIZE_SEL; // 32 byte endpoint, allocate a memory for the
                                  // endpoint.

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

    usb_config_status = 0;

    // wait for an interrupt of receive-setup-packet and respond the
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
  uint8_t const *descriptor;
  uint8_t descriptor_length;
  switch (wValue & 0xFF00) {
  case 0x0100: // Return device descriptor
    descriptor = device_descriptor;
    descriptor_length = pgm_read_byte(device_descriptor);
    break;
  case 0x0200: // Return the configuration descriptor and sub-descriptors.
    // TODO: connect configuration descriptors and sub-descriptors.
    // Add a wrapper to read single or connected descriptors and call it
    // at the sending block.
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
  uint8_t request_length = min(255, wLength);
  descriptor_length = min(request_length, descriptor_length);
  while (descriptor_length > 0) {
    while (!(UEINTX & (1 << TXINI)))
      ;
    UEINTX &= ~(1 << TXINI);
    uint8_t packet_length = min(ENDPOINT_SIZE, descriptor_length);
    for (int i = 0; i < packet_length; i++) {
      UEDATX = pgm_read_byte(descriptor + i);
    }
    descriptor_length -= packet_length;
    descriptor += packet_length;
    UEINTX &= ~(1 << FIFOCON);
  }
}

void send_report(){
    // TODO:
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
  const uint8_t request_direction = (bmRequestType & 0xC0) >> 6;
  const uint8_t request_kind = (bmRequestType & 0x30) >> 4;
  const uint8_t recipient = bmRequestType & 0x1F;
  if (request_kind == 0x00) { // Hanle any of standard requests
    if (recipient == 0x00) {  // Handle a standard device request
      switch (bRequest) {
      case GET_STATUS: {
        const uint8_t dat[2] = {0x00, 0x00};
        send_ram_bytes(dat, 2);
      } break;
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
        send_zero_length_packet();
        UDADDR |= (1 << ADDEN);
        break;
      case GET_DESCRIPTOR:
        send_descriptor(wValue, wIndex, wLength);
        break;
      case SET_DESCRIPTOR:
        // Set descriptor is not supported:
        // This behavior follows
        // https://github.com/kmani314/ATMega32u4-HID-Keyboard/blob/master/src/usb.c
        UECONX |= (1 << STALLRQ);
        break;
      case GET_CONFIGURATION: {
        const uint8_t dat[1] = {usb_config_status};
        send_ram_bytes(dat, 1);
      } break;
      case SET_CONFIGURATION:
        usb_config_status = (uint8_t)wValue;
        send_zero_length_packet();
        UENUM = GAMEPAD_ENDPOINT_NUM;
        UECONX |= (1 << EPEN);
        UECFG0X = (0x03 << EPTYPE0) | (1 << EPDIR);
        UECFG1X = 0x02; // Single bank, 8 byte, allocate memory.
        UERST = 0x1E;
        UERST = 0;

        // Enable IN interrupt for the gamepad endpoint.
        UEIENX |= (1 << TXINE);
        break;
      default:
        // Unexpected request.
        // Follows
        // https://github.com/kmani314/ATMega32u4-HID-Keyboard/blob/master/src/usb.c
        UECONX |= (1 << STALLRQ);
        break;
      }
    } else if (recipient == 0x01) { // Handle a standard interface request
      if (wIndex == 0) {
        switch (bRequest) {
        case GET_STATUS:
          const uint8_t dat[2] = {0x00, 0x00};
          send_ram_bytes(dat, 2);
          break;
        case GET_INTERFACE: {
          const uint8_t dat[1] = {usb_interface_status};
          send_ram_bytes(dat, 1);
        } break;
        case SET_CONFIGURATION:
          if (wValue == 0) {
            usb_interface_status = (uint8_t)wValue;
          } else {
            send_stall();
          }
          break;
        case SET_FEATURE:
        case CLEAR_FEATURE:
        case SYNCH_FRAME:
        default:
          UECONX |= (1 << STALLRQ);
          break;
        }
      } else {
        UECONX |= (1 << STALLRQ);
      }
    } else if (recipient == 0x02) { // Handle a standard endpoint request
      switch (bRequest) {
      case GET_STATUS: {
        if ((wIndex & (1 << 7)) && (wIndex & 0x0F)) {
          const uint8_t dat[2] = {0x00, 0x00};
          send_ram_bytes(dat, 2);
        } else {
          const uint8_t dat[2] = {0x00, 0x01};
          send_ram_bytes(dat, 2);
        }
      } break;
      }
    }
  }else if(request_kind == 0x01) {  // Handle class requests.
        if(recipient == 0x01 && wIndex == 0){      // handle a class request for interface
            if(bRequest == GET_REPORT){
                send_report();
            }else if(bRequest == GET_IDLE){
                const uint8_t dat[1] = {usb_idle_status};
                send_ram_bytes(dat, 1);
            }else if(bRequest == SET_IDLE){
                usb_idle_status = wValue >> 8;
                // const int report_id = wvalue & 0x00FFU;
                send_zero_length_packet();
            }
        }else{
            send_stall();
        }
  }
}

void send_gamepad_data() {
  // TODO:
}

ISR(USB_COM_vect) {
  if (UEINTX & (1 << RXSTPI)) {
    handle_control_setup();
  }
  // Handle an IN request for the gamepad endpoint interrupt
  if ((UEINT & (1 << GAMEPAD_ENDPOINT_NUM)) && (UEINTX & (1 << TXINI))) {
    send_gamepad_data();
  }
}
