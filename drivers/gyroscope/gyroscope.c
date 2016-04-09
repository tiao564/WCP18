/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Implementation of driver for the Adafruit L3GD20 Gyroscope.
 *    Intended for use with an AVR microcontroller. The custom
 *    "i2c_lib.h" API is required. See gyroscope.h for
 *    further details.
 *
 *    *Based on the Adafruit Industries implementation developed by
 *     Kevin Townsend.
 *
 **************************************************************/
 
#include "gyroscope.h"
#include "i2c_lib.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>

/*Device Address*/
#define GYRO_ADDR 0x6B

/*Chip IDs*/
#define L3GD20_ID  0xD4
#define L3GD20H_ID 0xD7

/*Register Definitions*/
#define WHO_AM_I  0x0F
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define OUT_X_L   0x28
#define OUT_X_H   0x29
#define OUT_Y_L   0x2A
#define OUT_Y_H   0x2B
#define OUT_Z_L   0x2C
#define OUT_Z_H   0x2D

/*Control Register 1 Bits*/
#define PD   3
#define Z_EN 2
#define Y_EN 1
#define X_EN 0

/*Control Register 4 Bits*/
#define FS1 5
#define FS0 4 

/*Control Register 5 Bits*/
#define BOOT 7

/*Gyroscope Data Limits*/
#define MAX_READING  32760
#define MIN_READING -32760

/*Gyroscope Sensitivity Factors*/
#define SENS_245DPS  0.00875F
#define SENS_500DPS  0.01750F
#define SENS_2000DPS 0.07000F

/*Automatically Increment Register Number*/
#define AUTO_INCREMENT 7

/*Transmission end selection*/
#define REPEATED_START 1
#define STOP_CONDITION 0

/*Misc. Size Definitions*/
#define BYTE  	  1
#define WORD  	  8
#define HALF_WORD 4
#define WRITE 	  2
#define REG	  	  0
#define VAL	  	  1

/*Buffer positions for read reg functions*/
#define X_LO 0
#define X_HI 1
#define Y_LO 2
#define Y_HI 3
#define Z_LO 4
#define Z_HI 5

/*Full-Scale Range Variable*/
static gyro_range range;
/*Auto-Ranging Variable*/
static bool auto_range;

/* Static Function Prototypes */
static bool read_n_consec_regs(uint8_t *buff, uint8_t reg, uint8_t n);
static bool write_gyro_reg(uint8_t reg, uint8_t value);
static bool is_saturated(int16_t data);

/*Read a byte from n consecutive gyroscope registers into a buffer*/
static bool read_n_consec_regs(uint8_t *buff, uint8_t reg, uint8_t n)
{
	bool mt_init = i2c_init_mt_mode(GYRO_ADDR);
	/*Set the auto-increment bit*/
	uint8_t _reg = (reg | (1 << AUTO_INCREMENT));
	
	if(mt_init == ENTRY_SUCCESS)
	{
		/*Specify the first reg to read and enable auto-increment*/
		uint8_t mt_write = i2c_mt_write(&_reg,BYTE,REPEATED_START);
		
		if(mt_write == MT_WRITE_SUCCESS)
		{
			bool mr_init = i2c_init_mr_mode(GYRO_ADDR);
		
			if(mr_init == ENTRY_SUCCESS)
			{
				/*Read n bytes into the buffer*/
				uint8_t mr_read = i2c_mr_read(buff,n,STOP_CONDITION);
				
				if(mr_read == MR_READ_SUCCESS)
				{
					/*Indicate read was successful*/
					return GYRO_READ_SUCCESS;
				}
			}
		}
	}
	
	return GYRO_READ_ERR;
}

/*Write a gyroscope register*/
static bool write_gyro_reg(uint8_t reg, uint8_t value)
{
	/*Register and data buffer*/
	uint8_t data[WRITE];
	data[REG] = reg;
	data[VAL] = value;
	/*Setup I2C write*/
	bool mt_init = i2c_init_mt_mode(GYRO_ADDR);

	if(mt_init == ENTRY_ERR) return GYRO_WRITE_ERR;
	
	else
	{
		uint8_t mt_write = i2c_mt_write(data,WRITE,STOP_CONDITION);
		/*Verify successful write*/
		if(mt_write == MT_WRITE_SUCCESS) return GYRO_WRITE_SUCCESS;
	}
	
	return GYRO_WRITE_ERR;
}

/*Check if the gyroscope reading is saturating*/
static bool is_saturated(int16_t data)
{
	return (((data > MAX_READING) | (data < MIN_READING)) ? true : false);
}

/* Gyroscope API */

/*See gyro_driver.h for details*/
void enable_autorange(void)
{
	auto_range = true;
}

/*See gyro_driver.h for details*/
void disable_autorange(void)
{
	auto_range = false;
}

/*See gyro_driver.h for details*/
bool init_gyro(gyro_range rng)
{
	/*Set full-scale range variable*/
	range = rng;
	
	uint8_t ctrl_4 = 0;
	uint8_t ctrl_1 = 0;
	uint8_t chip_id = 0;
	
	init_i2c();
	/*Read and verify gyroscope chip ID*/
	read_n_consec_regs(&chip_id,(uint8_t)WHO_AM_I,1);

	if((L3GD20_ID != chip_id) && (L3GD20H_ID != chip_id))
	{
		return GYRO_INIT_FAILED;
	}
	
	/*Turn on gyro and enable x-axis, y-axis, and z-axis data*/
	ctrl_1 = ((1 << X_EN) | (1 << Y_EN) | (1 << Z_EN) | (1 << PD));
	/*Write CTRL_REG1 and to enable gyroscope*/
	bool ctrl1_status = write_gyro_reg((uint8_t)CTRL_REG1,ctrl_1);
	
	/*Determine full-scale range value*/
	switch(range)
	{
		case RANGE_245_DPS:
			ctrl_4 = 0;
			break;
			
		case RANGE_500_DPS:
			ctrl_4 = (1 << FS0);
			break;
			
		case RANGE_2000_DPS:
			ctrl_4 = (1 << FS1);
			break;
			
		default:
			ctrl_4 = 0;
	}
	/*Write CTRL_REG4 to configure full-scale range values*/
	bool ctrl4_status = write_gyro_reg((uint8_t)CTRL_REG4,ctrl_4);
	
	if((ctrl1_status == GYRO_WRITE_ERR) || (ctrl4_status == GYRO_WRITE_ERR))
	{
		return GYRO_INIT_FAILED;
	}
	
	return GYRO_INIT_SUCCESS;
}

/*See gyro_driver.h for details*/
bool read_gyroscope(gyro_data *data)
{
	uint8_t xl = 0, xh = 0;
	uint8_t yl = 0, yh = 0;
	uint8_t zl = 0, zh = 0;
	
	int16_t x_tmp = 0;
	int16_t y_tmp = 0;
	int16_t z_tmp = 0;
	
	accum x_out = 0.0;
	accum y_out = 0.0;
	accum z_out = 0.0;
	
	bool valid_reading = false;
	uint8_t n = 6;
	uint8_t buffer[n];
	
	while(!valid_reading)
	{
		/*Read 6 consecutive data registers beginning with OUT_X_L*/
		bool read_status = read_n_consec_regs(buffer,(uint8_t)OUT_X_L,n);
		
		if(read_status == GYRO_READ_ERR) return GYRO_READ_ERR;
		
		/*Assign register values to temp variables*/
		xl = buffer[X_LO];
		xh = buffer[X_HI];
		yl = buffer[Y_LO];
		yh = buffer[Y_HI];
		zl = buffer[Z_LO];
		zh = buffer[Z_HI];
		
		/*Assemble 16-bit signed integers*/
		x_tmp = (int16_t)(xl | (xh << WORD)) >> HALF_WORD;
		y_tmp = (int16_t)(yl | (yh << WORD)) >> HALF_WORD;
		z_tmp = (int16_t)(zl | (zh << WORD)) >> HALF_WORD;

		if(!auto_range)
		{
			valid_reading = true;
		}
		/*Check for saturation if auto-ranging is enabled*/
		else
		{
			if((is_saturated(x_tmp)) | (is_saturated(y_tmp)) | (is_saturated(z_tmp)))
			{
				uint8_t ctrl_4 = 0;
				//Reenable x,y,z data
				uint8_t ctrl_1 = ((1 << X_EN) | (1 << Y_EN) | (1 << Z_EN) | (1 << PD));
				//Reboot memory content
				uint8_t ctrl_5 = (1 << BOOT);
				
				switch(range)
				{
					/*Increase range to 500DPS to eliminate saturation*/
					case RANGE_245_DPS:
						range = RANGE_500_DPS;
						ctrl_4 = (1 << FS0);
						valid_reading = false;
						break;
						
					/*Increase range to 2000DPS to eliminate saturation*/	
					case RANGE_500_DPS:
						range = RANGE_2000_DPS;
						ctrl_4 = (1 << FS1);
						valid_reading = false;
						break;
						
					default:
						valid_reading = true;
				}
				
				write_gyro_reg((uint8_t)CTRL_REG1,ctrl_1);
				write_gyro_reg((uint8_t)CTRL_REG4,ctrl_4);
				write_gyro_reg((uint8_t)CTRL_REG5,ctrl_5);
			}
			/*Validate reading if saturation has not occurred*/
			else
			{
				valid_reading = true;
			}
		}
	}
	/*Correct readings based on range value*/
	switch(range)
	{
		case RANGE_245_DPS:
			x_out = x_tmp*SENS_245DPS;
			y_out = y_tmp*SENS_245DPS;
			z_out = z_tmp*SENS_245DPS;
			break;
			
		case RANGE_500_DPS:
			x_out = x_tmp*SENS_500DPS;
			y_out = y_tmp*SENS_500DPS;
			z_out = z_tmp*SENS_500DPS;
			break;
			
		case RANGE_2000_DPS:
			x_out = (accum)x_tmp*SENS_2000DPS;
			y_out = (accum)y_tmp*SENS_2000DPS;
			z_out = (accum)z_tmp*SENS_2000DPS;
			break;
	}

	data->x = x_out;
	data->y = y_out;
	data->z = z_out;
	
	return GYRO_READ_SUCCESS;
}
/*End gyro_driver.c */
