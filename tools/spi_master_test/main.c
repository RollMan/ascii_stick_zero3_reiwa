#include <avr/io.h>

void spi_master_init(void){
	DDRB = (1 << DDB0) | (1 << DDB1) | (1 << DDB2);		// set PB0 (SS), PB1 (SCK), and PB2 (MOSI) output.
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);     // SPI enabled, slave, prescale 128.
}

void spi_send_byte(const uint8_t v){
	SPDR = v;
}


uint8_t spi_tranceiver(uint8_t v){
    SPDR = v;
    while(!(SPSR & (1 << SPIF)));
    return SPDR;
}

void spi_send_word(const uint8_t* v, const uint8_t len){
	for(int i = 0; i < len; i++){
        spi_tranceiver(v[i]);
	}
}

void spi_send_hex(uint8_t v){
	uint8_t str[2];
	int8_t digits = 1;
	while(digits >= 0){
		const uint8_t c = (v & 0x0F) + '0';
		str[digits] = c;
		v >>= 4;
		digits--;
	}
	spi_send_word(str, 2);
}

int main(void){
	uint8_t cnt = 0;
    uint8_t pre[] = "cnt: ";
    spi_master_init();
	while(1){
        spi_send_word(pre, 5);
        spi_send_hex(cnt);
        spi_tranceiver('\r');
        spi_tranceiver('\n');
        cnt++;
	}
}
