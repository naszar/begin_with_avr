#include <avr/io.h>
#include <avr/interrupt.h>

#include "blink.h"

volatile uint8_t lamp_state[2];
volatile uint8_t flipper;


/*
Init timer interrupt and start values (no light e.q both == 0) for 
green and red diodes
*/
void lamp_init(void){
 cli();
 LAMP_CONTROL |= 1 << RED_PIN | 1 << GREEN_PIN; /* set led pins to out */
 TCCR1B = (0<<CS12)|(1<<CS11)|(1<<CS10); /* 1Ghz/64 ~ 11661 tics per second (see CS bits of register TCCR */
 TIMSK |= (1<<TOIE1); /* enable timer_1 interrupts */
 TCNT1 = 64456 - 11661; /* blink every 1 sec */ 
 sei();                
 
};
/*
Changes state of diodes: blink or const for red 
and green
*/
void lamp_change_state(int state){
 cli();
 switch(state){
	case L_GREEN_CONST:
		lamp_state[0] = ~(0|1<<RED_PIN) & 0xff; /*turn off red pin */
		lamp_state[1] = 0 | 1 << GREEN_PIN; /*set green on*/
		break;
	case L_RED_CONST:
		lamp_state[0] = ~(0|1<<GREEN_PIN) & 0xff; /*turn off green pin */
		lamp_state[1] = 0 | 1 << RED_PIN; /*set green on*/
		break;
	case L_GREEN_BLINK:
		lamp_state[1] = 0 | 1 << GREEN_PIN; /* set green to on */
		lamp_state[0] = 0xff & ~(1<<GREEN_PIN) & ~(1<<RED_PIN); /* set  red and green pin off */ 
		break;
	case L_RED_BLINK:
		lamp_state[1] = 0 | 1 << RED_PIN;
		lamp_state[0] = 0xff & ~(1<<GREEN_PIN) & ~(1<<RED_PIN);
		break;
	case L_ALL_OFF:
		lamp_state[0] = 0xff & ~(1<<GREEN_PIN) & ~(1<<RED_PIN); /* set all leds off */
		lamp_state[1] = 0; /* no leds to on */
		break;
	};
 sei();
};

/*
Timer interrupt for green and red diodes
*/
ISR(TIMER1_OVF_vect)
{
/* cli();  in case of war or DAC */
 TCNT1 = 64456 - (int)(11661*.8); /* blink every .8 sec */
 flipper ^= 1;
 if(flipper)
	LAMP_PORT |= lamp_state[1];
 else
	LAMP_PORT &= lamp_state[0];
/* sei();*/
};
