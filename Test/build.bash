#! /bin/bash

#Variables
TEST_FILE=$(basename $1)
EXE_FILE=$(basename -s .c $TEST_FILE)
HEX_FILE=$EXE_FILE.hex
F_CPU=20000000

# 1) Compile Test File
avr-gcc -g -Os -Wall -std=gnu99 -mmcu=atmega1284p -D_FCPU=$F_CPU $TEST_FILE -o $EXE_FILE
# 2) Strip Metadata
avr-strip $EXE_FILE
# 3) Build Hex File
avr-objcopy -R .eeprom -O ihex $EXE_FILE $HEX_FILE
# 4) Program Test File
avrdude -p m1284p -c buspirate -P /dev/buspirate -U flash:w:$HEX_FILE