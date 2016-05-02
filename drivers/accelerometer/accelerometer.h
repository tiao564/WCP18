/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver API that enables use of the Adafruit LSM303 Accelerometer
 *    which is a subcomponent of the Intertial Measurement Unit
 *    (IMU) breakout board. The driver utilizes a custom I2C 
 *	  library and is intended for use with an AVR microntroller
 *    equipped with TWI hardware. This driver supports functionality
 *    for reading device data only.
 *
 *	  *Based on the Adafruit Industries implementation developed by
 *     Kevin Townsend.
 *
 **************************************************************/
 
#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

/********************************************
 * 		          Includes                  *
 ********************************************/ 
#include <stdfix.h>
#include <stdbool.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
/*Accelerometer Status Codes*/
#define ACCEL_INIT_FAIL  0
#define ACCEL_INIT_PASS  1
#define ACCEL_READ_FAIL	 0
#define ACCEL_READ_PASS  1
#define ACCEL_WRITE_FAIL 0
#define ACCEL_WRITE_PASS 1

/********************************************
 * 		          Structs                   *
 ********************************************/
/*Accelerometer Data Structure*/
typedef struct {
	accum x;
	accum y;
	accum z;
}accel_data;

/********************************************
 * 		      Function Prototypes           *
 ********************************************/
 
/***************************************************************
 *
 * DESCRIPTION:
 *  - Initializes the Adafruit LSM303 accelerometer. This function
 *    initializes I2C communication. It also sets the device data
 *    rate to 10Hz and enables x-axis, y-axis, and z-axis data.
 *    The device address has been hard-coded into the implementation.
 *    This function must be called before attempting to read the
 *    the accelerometer data. If initialization is successful, the
 *    function will return ACCEL_INIT_SUCCESS, else it will return
 *    ACCEL_INIT_FAILED.
 *
 **************************************************************/
bool init_accel(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Reads LSM303 Accelerometer x-axis, y-axis, and z-axis data
 *    into an "accel_data" data structure. Each data element is
 *    a signed fixed-point value. A accel_data structure should be 
 *    declared by the caller and a pointer to this structure passed 
 *	  to the function.
 *
 **************************************************************/
bool read_accel(accel_data *data);

#endif
/* End of accelerometer_driver.h */
