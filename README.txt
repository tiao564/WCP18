**************************************
* 									 *
*   WCP18 Industrial Multicopter	 *
*   Binghamton University (2106)     *
*								     *
**************************************

Overview
========
Control software developed for custom soil sampling system
designed by the Binghamton University Watson Capstone Project Team 18. 
Consists of device drivers for sensor components as well as application
level software to govern sampling mechanism operation. 

This software targets AVR microcontroller hardware, specifically
the ATmega1284p microcontroller developed by Atmel. All device-level
drivers are located the in the "drivers" directory. Source (.c) files
reside in the "src" directory while header files (.h) reside in the
"inc" directory. The Makefile required the build the project is
located the "bin" directory. Objects (.o) and the final executable
are also found in "bin".

Compilation
===========

	cd bin
	make
	