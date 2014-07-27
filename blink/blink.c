#include <avr/io.h>
#include <avr/interrupt.h>

#include "blink.h"

volatile uint8_t lamp_state[2];
volatile uint8_t even_flipper;

volatile uint16_t lamp_counter;
volatile uint16_t toplamp_counter;
volatile uint16_t siren_counter;
volatile uint8_t button_state;
volatile uint8_t siren_cur_freq;

/*
Init timer interrupt and start values (no light e.q both == 0) for 
green and red diodes
*/
void lamp_init(void){
 cli();
 LAMP_CONTROL |= 1 << BLINKER_PIN | 1 << SIREN_PIN | 1 << COP_L_RED_PIN| 1 << COP_L_WHITE_PIN; /* set led pins to out */
 TCCR1B = (0<<CS12)|(0<<CS11)|(1<<CS10); /* 1Ghz ~ 1M tics per second 
					  * 001 - /1
					  * 010 - /8
					  * 011 - /64
					  * 100 - /256
					  * 101 - /1024
					  * (see CS bits of register TCCR */
 TIMSK |= (1<<TOIE1); /* enable timer_1 interrupts */
 TCNT1 = 65535 - TIMER_PERIOD; /* interrupt every .0005 sec or f=2kHz */ 
 TCNT1 = 0;
 sei();                
 
};

void set_magic_on_off(uint8_t state){
	cli();
	if (state == B_ON){
		even_flipper = 0;
		toplamp_counter = 0;
		siren_counter = 0;
		button_state = B_ON;
		siren_cur_freq = 0;
		LAMP_PORT |= 1 << COP_L_WHITE_PIN;
	}else{
		button_state = B_OFF;
		LAMP_PORT &= 0 << COP_L_RED_PIN;
		LAMP_PORT &= 0 << COP_L_WHITE_PIN;
		LAMP_PORT &= 0 << SIREN_PIN;
	}
	sei();
}


		
/*
Timer interrupt for green and red diodes
*/
ISR(TIMER1_OVF_vect)
{ 
 uint8_t lamp_port;
 cli();
 lamp_port = LAMP_PORT;
TCNT1 = 65535 - TIMER_PERIOD; /* run every .0005 sec */
	/* lamp blink */
 if (lamp_counter++ == 2000){
	lamp_counter = 0;
	lamp_port ^= 1 << BLINKER_PIN;
 }
 /* if button pushed run siren and toplamp */
 if (button_state == B_ON){
	if (toplamp_counter++ == 1000){
		lamp_port ^= 1 << COP_L_WHITE_PIN;
		lamp_port ^= 1 << COP_L_RED_PIN;
		toplamp_counter = 0;
	}
	
	if (siren_counter++ == 6000){
		siren_counter = 0;
		siren_cur_freq ^= 1;
	}
	
	lamp_port ^= 1 << SIREN_PIN;
	if (even_flipper && siren_cur_freq)
		lamp_port ^= 1 << SIREN_PIN;
	even_flipper ^= 1;
}
	LAMP_PORT = lamp_port;

sei();
}
