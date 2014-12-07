#Target binary name 
TARG=freq
 
CC = avr-gcc
OBJCOPY = avr-objcopy
 
# progect files
SRCS= main.c 
HEADERS= lcd8bit.h
 
OBJS = $(SRCS:.c=.o)
 
# target platform
MCU=atmega8
 

CFLAGS = -mmcu=$(MCU) -Wall -ffunction-sections -fdata-sections -Os  
LDFLAGS = -mmcu=$(MCU)  -Wall -Wl,--gc-sections -Os  
 
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
