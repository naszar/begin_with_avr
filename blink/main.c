#define F_CPU 1000000UL
#include <avr/delay.h>
#include <avr/io.h>

#include "blink.h"

#define BUTTON_PIN	0	/* see blink.h for outports default is pins 1 to 4
				 * of PORT B */
#define BUTTON_PORT	PORTB
#define BUTTON_CONTROL	DDRB
#define BUTTON_R_PIN	PINB

main()
{

/*init input button*/
	BUTTON_CONTROL &= 0 << BUTTON_PIN;
	BUTTON_PORT &= 0 << BUTTON_PIN;
	lamp_init();
	set_magic_on_off(B_OFF);

	while (1) {
		_delay_ms(500);
		if (BUTTON_R_PIN & 1 << BUTTON_PIN)
			set_magic_on_off(B_ON);
		else
			set_magic_on_off(B_OFF);
	}
}
