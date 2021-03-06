#Nicholas Shanahan

F_CPU := 8000000
CC := avr-gcc
MMCU := atmega1284p
CFLAGS := -g -Os -Wall -Wextra -std=gnu99

#The name you wish to give to the executable
EXE :=
HEX := $(EXE).hex

MAIN :=

default: $(EXE)

all: program

INCS = -I ../drivers/i2c \
       -I ../drivers/accelerometer \
	   -I ../drivers/gyroscope \
	   -I ../drivers/encoders
	   -I ../drivers/motors \
	   -I ../drivers/ultrasonic_sensor \
	   -I ../drivers/vibration_sensor \

SRCS = ../drivers/i2c \
       ../drivers/accelerometer \
	   ../drivers/gyroscope \
	   ../drivers/encoders
	   ../drivers/motors \
	   ../drivers/ultrasonic_sensor \
	   ../drivers/vibration_sensor \

#VPATH will extract dependencies from the
#listed source directories automatically	   
VPATH = $(SRCS)

OBJS = i2c_lib.o \
	   accelerometer.o \
	   gyroscope.o \
	   encoder.o \
	   motor.o \
	   ultrasonic.o \
	   vibration.o \
	   
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
