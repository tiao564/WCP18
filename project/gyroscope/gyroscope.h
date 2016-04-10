/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver API that enables the use of the Adafruit L3GD20
 *    Gyroscope which is a subcomponent of the Intertial-Measurement
 *    Unit (IMU) breakout board. The driver utilizes a custom I2C 
 *	  library and is intended for use with an AVR microntroller
 *    equipped with TWI hardware. This driver supports functionality
 *    for reading device data only.
 *
 *    *Based on Adafruit Industries implementation developed by
 *     Kevin Townsend.
 *
 **************************************************************/
 
#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_	

#include <stdbool.h>
#include <stdfix.h>

/*Gyroscope Status Codes*/
#define GYRO_INIT_FAIL  0
#define GYRO_INIT_PASS  1
#define GYRO_READ_FAIL	0
#define GYRO_READ_PASS  1
#define GYRO_WRITE_FAIL	0
#define GYRO_WRITE_PASS 1

/*Full-Scale Range Values*/
typedef enum {
	RANGE_245_DPS  = 245,
	RANGE_500_DPS  = 500,
	RANGE_2000_DPS = 2000,
}gyro_range;

/*Gyroscope Data Structure*/
typedef struct {
	accum x;
	accum y;
	accum z;
}gyro_data;

/***************************************************************
 *
 * DESCRIPTION:
 *  - Initializes the Adafruit L3GD20 Gyroscope. This function 
 *    enables x-axis, y-axis, and z-axis data as well as the use
 *    of the I2C protocol. The user must provide a value for the
 *    full-scale range of the gyroscope. If the function
 *    determines the chip ID of the gyroscope is incorrect, it will
 *    return GYRO_INIT_FAILED, else it will return GYRO_INIT_SUCCESS.
 *
 **************************************************************/
bool init_gyro(gyro_range rng);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Notifies the gyroscope reading function that auto-ranging
 *    functionality is not enabled.
 *
 **************************************************************/
void enable_autorange(void);
 
 /***************************************************************
 *
 * DESCRIPTION:
 *  - Notifies the gyroscope reading function that auto-ranging
 *    functionality is enabled.
 *
 **************************************************************/
void disable_autorange(void);

 /***************************************************************
 *
 * DESCRIPTION:
 *  - Reads the L3GD20 Gyroscope x-axis, y-axis, and z-axis data
 *    into a "gyro_data" data structure provided by the user. Each
 *    data element is a fixed point variable. This function provides
 *    support for correction if saturation occurs. It also accounts
 *    for a senistivity factor based on the full-scale range selected.
 *
 **************************************************************/
bool read_gyroscope(gyro_data *data);
 
#endif
/* End of gyro_driver.h */
