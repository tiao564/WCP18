Atmega1284p Test Program
========================

The provided script will build either the provided 
test file or some other user specified test file and
load the executable hex file to the Atmega1284p MCU.

The default port for the LED is D6 but this can be 
changed by modifying the macros in test.c.

If you are using the Debian VM provided by the University,
you will need to change "/dev/ttyUSB0" in line 16 of the 
script to "/dev/buspirate".

Build and Program
=================

1) Established privileged shell

		sudo -s
		
2) Ensure build.bash is executable

		chmod 711 build.bash
		
3) Execute build.bash

		./build.bash <test_file>