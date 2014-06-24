/* Based on http://letsmakerobots.com/files/ATMega8-ADC-test.c */
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "adc.h"

uint8_t adc_get_value(uint8_t channel)
{

/*
  ADMUX IS |REFS1|REFS0|ADLAR| – |MUX3|MUX2|MUX1|MUX0|
  REFS1|REFS0 = 1 +> AV CC with external capacitor at AREF pin
  ADLAR = 1 => left adjusted resul, can take in ADCH
  MUX3|MUX2|MUX1|MUX0 = channel - pin for analog value
*/
 ADMUX = _BV(ADLAR) | _BV(REFS0) + channel;
 uint8_t adc_result;
/*
 ADCRA is |ADEN|ADSC|ADFR|ADIF|ADIE|ADPS2|ADPS1|ADPS0|
 ADPS2|ADPS1|ADPS0 = 111 => frequency division factor is 128 e.q. 8Hz in my case 
 ADIE = 0 => no interrupt
*/
 ADCSRA =  _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); 
/* ADEN = 1 => Enable ADC, ADSC = 1 => start conversion */
 ADCSRA |= _BV(ADEN) | _BV(ADSC);
/* wait for end of conversion, maybe I must use ADIF - set when conversion is complite */
 while(ADCSRA & _BV(ADSC));
 adc_result = ADCH;
 return adc_result;
}
