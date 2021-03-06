#Nicholas Shanahan

# Makefile shell for compiling/programming with the ATmega1284p.

# Options:
# 1) To create the executable specified by the EXE variable, type "make".
# 2) To program the microncontroller, type "make program".
# 3) To build and then program, type "make all".

F_CPU := 8000000
CC := avr-gcc
MMCU := atmega1284p
CFLAGS := -g -Os -Wall -Wextra -std=gnu99

# *** PATHS MUST EITHER BE ABSOLUTE OR RELATIVE TO THE MAKEFILE DIRECTORY! ***

#The name you wish to give to the executable
EXE := gyro_test
HEX := $(EXE).hex

#Target file, the "main"
#Ex/ "../main.c"
MAIN := gyro_test.c

default: $(EXE)

all: program

INCS = -I../i2c_lib \
       -I. \
	   -I../lcd_driver \

SRCS = ../i2c_lib \
       . \
	   ../lcd_driver \

#VPATH will extract dependencies from the
#listed source directories automatically	   
VPATH = $(SRCS)

OBJS = i2c_lib.o \
       gyroscope.o \
	   lcd_driver.o \
	   
%.o:%.c
	$(CC) -c $(CFLAGS) -mmcu=$(MMCU) -D_FCPU=$(F_CPU) $(INCS) $<

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
