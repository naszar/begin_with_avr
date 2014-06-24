#Target binary name 
TARG=test_charge
 
CC = avr-gcc
OBJCOPY = avr-objcopy
 
# progect files
SRCS= main.c adc.c blink.c
HEADERS= adc.h blink.h
 
OBJS = $(SRCS:.c=.o)
 
# target platform
MCU=atmega8
 

CFLAGS = -mmcu=$(MCU) -Wall -Os  
LDFLAGS = -mmcu=$(MCU)  -Wall -Os  
 
all: $(TARG) 
 
$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS) -lm
	$(OBJCOPY) -j .text -j .data -O ihex $@.elf $@.hex
#	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
#	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
 
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm *.o *.elf	
