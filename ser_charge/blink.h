
#define LAMP_PORT       PORTB                                                                                                                              
#define LAMP_CONTROL    DDRB                                                                                                                               

#define RED_PIN         1                                                                                                                                  
#define GREEN_PIN       2  

#define L_GREEN_BLINK   1                                                                                                                                  
#define L_GREEN_CONST   2                                                                                                                                  
#define L_ALL_OFF       0                                                                                                                                  
#define L_RED_BLINK     3  

void lamp_init(void);
void lamp_change_state(int);

