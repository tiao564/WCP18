#Nicholas Shanahan

F_CPU := 8000000
CC := avr-gcc
MMCU := atmega1284p
CFLAGS := -g -Os -Wall -Wextra -std=gnu99

EXE := test
HEX := $(EXE).hex

MAIN := ultrasonic_test.c

default: $(EXE)

all: program

INCS = -I . \
       -I ../lcd_driver \

SRCS = . \
       ../lcd_driver \

#VPATH will extract dependencies from the
#listed source directories automatically	   
VPATH = $(SRCS)

OBJS = ultrasonic.o \
       lcd_driver.o \
       itoa.o \

%.o:%.c
	$(CC) -c $(CFLAGS) -mmcu=$(MMCU) -D_FCPU=$(F_CPU) $<

#Builds the target specified by the EXE variable	
$(EXE): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) -mmcu=$(MMCU) -D_FCPU=$(F_CPU) $(OBJS) $(INCS) $(MAIN) -o $(EXE)

#Builds hex file specified by the HEX variable	
$(HEX): $(EXE)
	avr-strip $(EXE)
	avr-objcopy -R .eeprom -O ihex $(EXE) $(HEX)
	
#Writes the hex file to the microncontroller flash memory
program: $(HEX)
	sudo avrdude -p m1284p -c buspirate -P /dev/ttyUSB0 -U flash:w:$(HEX)
	
#Removes the executable, hex file, and object files from PWD	
clean:
	rm $(EXE) $(HEX) $(OBJS)
