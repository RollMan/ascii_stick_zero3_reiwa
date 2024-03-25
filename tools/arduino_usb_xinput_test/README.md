Files inside this directory constitutes a USB device firmware upon the arduino framework.
This is useful to check whether the problem in the current project that computer sometimes fails to recognize the device is related to the circuit.

Conclusion is that firmware contains problems. This sketch works well and let PCs recognize the device.

## Build
Prepare a circuit at first. The hardware described in `hardware/` is compatible.

### Install XInput
Prepare Arduino IDE.

Install of board: dmadison/ArduinoXInput_AVR: AVR boards package for the ArduinoXInput project
https://github.com/dmadison/ArduinoXInput_AVR

Install of library: dmadison/ArduinoXInput: XInput library for USB capable Arduino boards https://github.com/dmadison/ArduinoXInput

### Compile and Upload Firmware
Open Arduino IDE and open the sketch.
Select the board of "Arduino Leonard w/ XInput for ATmega32U4.
"Export Compiled Binary."
Burn the binary to any MCU by "Device Programming" of Atmel Studio.
I comfirmed AVRISP MKⅡ worked well.

Probably CLI tools (arduino-cli and avrdude) may reproduce the effect equivalently.

Burning a bootloader may enable USB uploading but perhaps USB signals by application and bootloader collides.
I did not try that.

## Notes
An XInput button corresponding to PD5 unexpectedly oscillates its signal.
I tried to disable ADC (in software manner) and JTAG (FUSE) but it dit not work.
