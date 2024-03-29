#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <avr/pgmspace.h>

#define REPORT_DESCRIPTOR_SIZE (56)

// https://gist.github.com/DJm00n/a6bbcb810879daa9354dee4a02a6b34e
// https://www.partsnotincluded.com/understanding-the-xbox-360-wired-controllers-usb-data/
static const uint8_t device_descriptor[] PROGMEM = {
    0x12,       // bLength
    0x01,       // bDescriptorType
    0x00, 0x02, // bcdUSB
    0xFF,       // bDeviceClass      (Vendor specific)
    0xFF,       // bDeviceSubClass
    0xFF,       // bDeviceProtocol
    0x08,       // bMaxPacketSize0   (8 bytes)
    0x5E, 0x04, // idVendor
    0x8E, 0x02, // idProduct
    0x14, 0x01, // bcdDevice
    0x01,       // iManufacturer
    0x02,       // iProduct
    0x03,       // iSerialNumber
    0x01,       // bNumConfigurations
};

static const uint8_t configuration_descriptor[] PROGMEM = {
    0x09,       // bLength
    0x02,       // bDescriptorType
    0x00, 0x00, // wTotalLength   TODO:
    0x01,       // bNumInterfaces
    0x01,       // bConfigurationValue
    0x00,       // iConfiguration
    0xA0,       // bmAttributes   (Bus-powered Device, Remote-Wakeup)
    0xFA,       // bMaxPower      (500 mA)
};

#define CONFIGURATION_DESCRIPTOR_SIZE (sizeof(configuration_descriptor) / sizeof(configuration_descriptor[0]))

static const uint8_t INTERFACE_DESCRIPTOR_LENGTH = 9;
static const uint8_t interface_descriptor[] PROGMEM = {
    9,    // bLength
    0x04, // bDescriptorType     (INTERFACE)
    0,    // bInterfaceNumber
    0,    // bAlternateSetting
    2,    // bNumEndpoints
    0xFF, // bInterfaceClass     (Vendor Specific)
    0x5D, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0,    // iInterface
    // another unknown (probably hid) descriptor
    0x09,       // bLength
    0x21,       // bDescriptorType (HID)
    0x11, 0x01, // bcdHID 1.11
    0x00,       // bCountryCode
    0x01,       // bNumDescriptors
    0x22,       // bDescriptorType[0] (HID)
    REPORT_DESCRIPTOR_SIZE, 0x00, // wDescriptorLength[0] // report descriptor length
    // endpoint descriptor
    7,          // bLength
    0x05,       // bDescriptorType (ENDPOINT)
    0x81,       // bEndpointAddressIN Endpoint : 1
    0x03,       // bmAttributes
    0x20, 0x00, // wMaxPacketSize
    4,          // bInterval
};

static const uint8_t endpoint_descriptor[REPORT_DESCRIPTOR_SIZE] PROGMEM = {
    7,          // bLength
    0x05,       // bDescriptorType (ENDPOINT)
    0x81,       // bEndpointAddressIN Endpoint : 1
    0x03,       // bmAttributes
    0x20, 0x00, // wMaxPacketSize
    4,          // bInterval
};

#define ENDPOINT_DESCRIPTOR_LENGTH (7)

#define CONFIGURATION_AND_SUB_DESCRIPTOR_SIZE ((sizeof(configuration_descriptor) + sizeof(endpoint_descriptor)) / sizeof(configuration_descriptor[0]))

static const uint8_t HID_DESCRIPTOR_LENGTH = 9;
static const uint8_t hid_descriptor[] PROGMEM = {
    0x09,       // bLength
    0x21,       // bDescriptorType (HID)
    0x11, 0x01, // bcdHID 1.11
    0x00,       // bCountryCode
    0x01,       // bNumDescriptors
    0x22,       // bDescriptorType[0] (HID)
    REPORT_DESCRIPTOR_SIZE, 0x00, // wDescriptorLength[0] // report descriptor length
};

static const uint8_t REPORT_DESCRIPTOR_LENGTH = 0;
static const uint8_t report_descriptor[] PROGMEM = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x39,                    //   USAGE (Hat switch)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x07,                    //   LOGICAL_MAXIMUM (7)
    0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
    0x46, 0x3b, 0x01,              //   PHYSICAL_MAXIMUM (315)
    0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x42,                    //   INPUT (Data,Var,Abs,Null)
    0x65, 0x00,                    //   UNIT (None)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x0a,                    //   USAGE_MAXIMUM (Button 10)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x95, 0x0a,                    //   REPORT_COUNT (10)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};

// Data types that follows report

typedef struct {
  int8_t BTN_GamePadButton1 : 1; // Usage 0x00090001: Button 1 Primary/trigger,
                                 // Value =  to
  int8_t BTN_GamePadButton2 : 1; // Usage 0x00090002: Button 2 Secondary, Value
                                 // =  to
  int8_t
      BTN_GamePadButton3 : 1; // Usage 0x00090003: Button 3 Tertiary, Value = to
  int8_t BTN_GamePadButton4 : 1;   // Usage 0x00090004: Button 4, Value =  to
  int8_t BTN_GamePadButton5 : 1;   // Usage 0x00090005: Button 5, Value =  to
  int8_t BTN_GamePadButton6 : 1;   // Usage 0x00090006: Button 6, Value =  to
  int8_t BTN_GamePadButton7 : 1;   // Usage 0x00090007: Button 7, Value =  to
  int8_t BTN_GamePadButton8 : 1;   // Usage 0x00090008: Button 8, Value =  to
  int8_t BTN_GamePadButton9 : 1;   // Usage 0x00090009: Button 9, Value =  to
  int8_t BTN_GamePadButton10 : 1;  // Usage 0x0009000A: Button 10, Value =  to
  int8_t : 6;                      // Pad
  uint8_t GD_GamePadHatSwitch : 4; // Usage 0x00010039: Hat switch, Value = 1 to
                                   // 8, Physical = (Value - 1) x 4155 / 7 in
  uint8_t : 4;                     // Pad
  uint8_t pad_5;                   // Pad
} controller_input_t;

#endif
