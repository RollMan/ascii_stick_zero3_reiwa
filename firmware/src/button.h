#define get_stick_up    (PINB & (1 << PINB4))
#define get_stick_down  (PINB & (1 << PINB5))
#define get_stick_left  (PINB & (1 << PINB6))
#define get_stick_right (PINC & (1 << PINC6))

#define get_buttons_upper ((0x3C & (PINF >> 2)) | (PINF & 3))
#define get_buttons_lower (PIND)

void init_buttons();
