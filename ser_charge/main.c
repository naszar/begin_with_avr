#define F_CPU 1000000UL
#include <avr/delay.h>
#include <avr/io.h>

#include "blink.h"
#include "adc.h"

#define V_TO_COUNTS(X)	(int) (X*256/5)
#define V_BAT_MIN	V_TO_COUNTS(0.7)
#define V_BAT_MAX_MAX	V_TO_COUNTS(4)
#define V_BAT_MAX_MIN	V_TO_COUNTS(3.7)

#define TEMP_BAT_MIN	V_TO_COUNTS(0.1)
#define TEMP_BAT_MAX	V_TO_COUNTS(2)
#define CP_BAT_MIN	V_TO_COUNTS(0.1)
#define CP_BAT_MAX	V_TO_COUNTS(1)

#define V_BAT_PIN	0
#define TEMP_BAT_PIN	1
#define CP_BAT_PIN	2

#define POWER_PIN	0
#define POWER_PORT	PORTB
#define POWER_CONTROL	DDRB

main()
{
	uint8_t v_val;
	uint8_t temp_val;
	uint8_t cp_val;

	uint8_t charge_max = V_BAT_MAX_MAX;

	lamp_init();
	POWER_CONTROL |= 1 << POWER_PIN;
	lamp_change_state(L_ALL_OFF);

	while (1) {
		v_val = adc_get_value(V_BAT_PIN);
		if (v_val > V_BAT_MIN) {
			temp_val = adc_get_value(TEMP_BAT_PIN);
			if (temp_val > TEMP_BAT_MIN && temp_val < TEMP_BAT_MAX) {
				if (v_val < charge_max) {
					POWER_PORT |= 1 << POWER_PIN;
					cp_val = adc_get_value(CP_BAT_PIN);
					if (cp_val > CP_BAT_MIN
					    && cp_val < CP_BAT_MAX) {
						lamp_change_state
						    (L_GREEN_BLINK);
						_delay_ms(500);
					} else
						lamp_change_state(L_RED_BLINK);
					charge_max = V_BAT_MAX_MAX;
					POWER_PORT &= ~(1 << POWER_PIN);
				} else
					lamp_change_state(L_GREEN_CONST);
				charge_max = V_BAT_MAX_MIN;
			} else if (v_val < charge_max)
				lamp_change_state(L_RED_CONST);
			else
				lamp_change_state(L_GREEN_CONST);
		} else {
			lamp_change_state(L_ALL_OFF);
			charge_max = V_BAT_MAX_MAX;
			_delay_ms(1000);
		}
		_delay_ms(500);
	};
};
