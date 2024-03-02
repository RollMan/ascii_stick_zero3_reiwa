#include <avr/io.h>
#include "fmt.h"
#include <util/delay.h>

const uint8_t txt[] = "hello world\r\n";
const uint8_t size = sizeof(txt) / sizeof(txt[0]);

int main(void){
    uint8_t cnt = 0;
    DDRB = (1 << DDB3);
    while(1){
        debug_send_hex(cnt++);
        debug_send_byte('\r');
        debug_send_byte('\n');
    }
}
