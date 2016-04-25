/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for system control interface. The system control input
 *    is PWM signal connected to PC3 of the ATmega1284p MCU. There
 *    are three possible input pulse widths that correspond to three
 *    unique system states: OFF, ENABLE, and MANUAL_OVERRIDE.
 *    This driver uses pin change interrupt 19 to monitor the
 *    system control state continuously. 
 *
 **************************************************************/

#ifndef SYSTEM_CTL_H_
#define SYSTEM_CTL_H_

/********************************************
* 		          Includes                  *
 ********************************************/
#include <stdint.h>

/********************************************
* 		           Macros                   *
 ********************************************/
 
/*      System Control States       */
#define SYS_OFF_STATE 				0  // System Shutdown	   
#define SYS_ENABLE_STATE			1  // System Activate
#define SYS_MANUAL_OVERRIDE_STATE  -1  // System Emergency Shutdown 

/********************************************
 * 		      Function Prototypes           *
 ********************************************/
 
/***************************************************************
 * DESCRIPTION:
 *  - Initializes the system control inteface. Enables pin change
 *    interrrupt 19 and configures PC3 as the system control
 *    input pin.
 *
 **************************************************************/
void init_system_cntl(void);
 
/***************************************************************
 * DESCRIPTION:
 *  - Returns the current system state as dictated by the remote
 *    control input. The three possible system state inputs are
 *    defined above.
 *
 **************************************************************/
int8_t get_sys_cntl_state(void);
 
#endif
/* End of system_ctl.h */