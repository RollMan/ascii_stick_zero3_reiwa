#include "usb.h"
#include <avr/interrupt.h>
#include <avr/io.h>

typedef enum {
    G_VBUST,
    G_UPRSM,
    G_EROSM,
    G_WAKEUP,
    G_EROSTI,
    G_SOFI,
    G_SUSPI,
} genintsrc_t;

void usb_power_on(){
    cli();
    // Power-On USB pads regulator
    UHWCON |= (1 << UVREGE);                 // Enable USB Regulator to use 5V power source of a USB host.

    // Configure PLL interface & enable PLL
    PLLFRQ = 0x4A;
    PLLCSR |= (1 << PINDIV) | (1 << PLLE);
    // Check PLL lock
    while(!(PLLCSR & (1 << PLOCK)));
    // Enable USB interface
    USBCON |= 1 << USBE;
    // Configure USB interface (USB speed, Endpoints configuration...)
    //     Speed configuration; disable low-speed mode.
    USBCON = ((1 << USBE) | (1 << OTGPADE) | (1 << VBUSTE)) | (USBCON & ~(1 << FRZCLK));
    UDCON &= ~(1 << LSM);
    // Wait for an interruption of USB VBUS information connection and
    // attach USB device by:
    //     UDCON &= ~(1 << DETACH);
    sei();
}

void handle_udint(){
    if(UDINT & (1 << EORSTI)){
        /*
         * End of reset is detected.
         * Prepare to respond the first 64 bytes of the device descriptor on the address of 0.
        */
        // Activate the endpoint 0.
        UENUM = 0;
        UECONX = (1 << EPEN);
        UECFG0X = 0;        // Control endpoint, OUT direction.
        UECFG1X |= 0x22;    // 32 byte endpoint, allocate a memory for the endpoint.

        if(UESTA0X & (1 << CFGOK)){
            while(1);
        }

        // Endpoint 0 is activated.

        // Reset the endpoint.
        UERST = 1;
        UERST = 0;

        // Confirm the receive-setup-packet interrupt is enabled.
        UEIENX = (1 << RXSTPE);

        // TODO: wait for an interrupt of receive-setup-packet and respond the device descriptor.
    }
}

ISR(USB_GEN_vect){

    // Handle the VBUS pad transition.
    if(USBINT & (1 << VBUSTI)){
        USBINT &= ~(1 << VBUSTI);
        if(USBSTA & (1 << VBUS)){
            UDCON &= ~(1 << DETACH);            // Attach the USB bus.
        }
    }

    // Handle UDINT
    if (UDINT){
        /*
         *  Do NOT `return` inside this block.
         *  Otherwise, UDINT is never cleard, which
         *  is described at the bottom of this block.
         */

        handle_udint();

        UDINT = 0;  // Do not forget to clear the interruption bits.
    }
}

ISR(USB_COM_vect){
}
