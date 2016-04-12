/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for Adafruit vibration sensor switches. Utilizies the
 *    pin change interrupt functionality of AVR microcontrollers
 *    to determine when the sensor has tripped. When the vibration
 *    threshold of a sensor is exceeded, the the switch closes,
 *    triggering the pin change ISR.
 *
 **************************************************************/
 
#ifndef VIBRATION_H_
#define VIBRATION_H_

#include <stdbool.h>

/*Medium Vibration Sensor A Port Locations*/
#define MED_A_PORT B
#define MED_A_POS  3

/*Medium Vibration Sensor B Port Locations*/
#define MED_B_PORT B
#define MED_B_POS  1

/*Slow Vibration Sensor A Port Locations*/
#define SLOW_A_PORT B
#define SLOW_A_POS  2

/*Slow Vibration Sensor B Port Locations*/
#define SLOW_B_PORT B
#define SLOW_B_POS  0

/***************************************************************
 *
 * DESCRIPTION:
 *  - Enables global interrupts and pin change interrupts on port B.
 *    Specifically enables PB[3:0] pin change interrupt functionality.
 *    Additionally configures the vibration sensor ports as inputs.
 *
 **************************************************************/
void init_vibration_sensors(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of the sensor Medium A status flag.
 *    A logical one indicates the sensor has tripped. A logical
 *    zero indicates it has not.
 *
 **************************************************************/
bool check_vs_med_a(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of the sensor Medium B status flag.
 *    A logical one indicates the sensor has tripped. A logical
 *    zero indicates it has not.
 *
 **************************************************************/
bool check_vs_med_b(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of the sensor Slow A status flag.
 *    A logical one indicates the sensor has tripped. A logical
 *    zero indicates it has not.
 *
 **************************************************************/
bool check_vs_slow_a(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of the sensor Slow B status flag.
 *    A logical one indicates the sensor has tripped. A logical
 *    zero indicates it has not.
 *
 **************************************************************/
bool check_vs_slow_b(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Disables the use of the vibration sensors by turning off_type
 *    pin change interrupts. This prevents the pin change interrupt
 *    from firing when the user no longer wishes to the use the
 *    sensors.
 *
 **************************************************************/
void disable_vibration_sensors(void);

#endif
/* End of vibration.h */