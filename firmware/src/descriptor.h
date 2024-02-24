#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <avr/pgmspace.h>
static const uint8_t device_descriptor[] PROGMEM = {
};

static const uint8_t CONFIGURATION_DESCRIPTOR_LENGTH = 9;
static const uint8_t configuration_descriptor[] PROGMEM = {
};


static const uint8_t INTERFACE_DESCRIPTOR_LENGTH = 9;
static const uint8_t interface_descriptor[] PROGMEM = {
};

static const uint8_t ENDPOINT_DESCRIPTOR_LENGTH = 7;
static const uint8_t endpoint_descriptor[] PROGMEM = {
};

static const uint8_t HID_DESCRIPTOR_LENGTH = 9;
static const uint8_t hid_descriptor[]  PROGMEM = {
};

static const uint8_t REPORT_DESCRIPTOR_LENGTH = 0;
static const uint8_t report_descriptor[] PROGMEM = {
};

#endif
