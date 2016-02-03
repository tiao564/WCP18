Atmega1284p Test Program
========================

The provided script will build either the provided 
test file or some other user specified test file and
load the executable hex file to the Atmega1284p MCU.

Build and Program
=================

1) Established privileged shell

		sudo -s
		
2) Ensure build.bash is executable

		chmod 711 build.bash
		
3) Execute build.bash

		./build.bash <test_file>