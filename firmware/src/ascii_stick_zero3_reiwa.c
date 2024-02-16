#include <avr/io.h>

void usb_power_on(){
    // Power-On USB pads regulator
    UHWCON = (1 << UVREGE);                 // Enable USB Regulator to use 5V power source of a USB host.
    USBCON = (1 << USBE) && (1 << FRZCLK) && (1 << OTGPADE) && (1 << VBUSTE);

    // Configure PLL interface
    // Enable PLL
    // Check PLL lock
    // Enable USB interface
    // Configure USB interface (USB speed, Endpoints configuration...)
    // Wait for USB VBUS information connection
    // Attach USB device
}

int main(void){
    usb_power_on();
    return 0;
}
