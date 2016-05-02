/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver that enables reading of an ultrasonic ping sensor
 *    such as those made by Polulu or Parallax. The driver utilizes
 *    the pin change interrupt functionality of an AVR micrcontroller
 *    to determine distance of an obstacle. Timer overflow interrupts
 *    are used to provide a timeout feature to the software. 
 *
 **************************************************************/
 
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

/********************************************
 * 		          Includes                  *
 ********************************************/ 
#include <stdfix.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
/*Ultrasonic Sensor A Port Definitions*/
#define TRIGGER_A_PORT A
#define ECHO_A_PORT    A
#define TRIGGER_A_POS  0
#define ECHO_A_POS     1

/*Ultrasonic Sensor B Port Defintions*/
#define TRIGGER_B_PORT A
#define ECHO_B_PORT    A
#define TRIGGER_B_POS  2 
#define ECHO_B_POS     3

/*Ultrasonic Sensor Error Code*/
#define ULTRASONIC_TIMEOUT 0

/********************************************
 * 		         Typedefs                   *
 ********************************************/
/*Sensor Identifiers*/
typedef enum {A,B} sensor;

/********************************************
 * 		      Function Prototypes           *
 ********************************************/
 
/***************************************************************
 *
 * DESCRIPTION:
 *  - Performs necessary setup for ultrasonic sensors A and B.
 *    Enables global and pin-change interrupt functionality and
 *    configures the trigger and echo input/output ports.
 *    Additionally, it enables timer overflow interrupts (TOV)
 *    to be used for the sensor timeout feature.
 *
 **************************************************************/
void init_ultrasonic_sensors(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the distance of an obstacle in centimeters. Includes
 *    timeout functionality if an obstacle cannot be detected. The
 *    user can select between reading sensor A and B by passing the
 *    sensor ID letter "A" or "B" to function. 
 *
 **************************************************************/
accum get_obstacle_distance_cm(sensor id);

#endif
/* End of ultrasonic.h */
