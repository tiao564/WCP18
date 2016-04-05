/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Implementation of I2C API that facilitates use of the AVR
 *    microcontroller TWI hardware. See i2c_lib.h for additional
 *    details.
 *
 **************************************************************/
 
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "i2c_lib.h"

/*Bit Rate Register value
 *Assumes 8MHz system clock!*/
#define BIT_RATE 0x20 //32 --> 100KHz (Standard Mode)

/*TWI Status Register Pre-scaler bits*/
#define TWPS1 1
#define TWPS0 0

/*TWI Control Register Bits*/
#define TWINT 7
#define TWEA  6
#define TWSTA 5
#define TWSTO 4
#define TWEN  2

/*Power Reduction TWI Bit*/
#define PRTWI 7

/*Mask off TWSR status bits*/
#define STATUS_MSK 0xF8

/*Mask for clearing control registers*/
#define CLEAR_TWCR 0x04

/*Apply to slave address*/
#define READ   0x01
#define WRITE  0xFE

/*Used to select between MT and MR modes*/
#define R 0
#define W 1

/*Check for Repeated Start*/
#define REPEATED_START 	  1
#define NO_REPEATED_START 0

/* Static Function Prototypes */
static void send_start_cond(void);
static void send_stop_cond(void);
static void send_repeated_start(void);
static void transmit_byte(void);
static void send_ack(void);
static void send_not_ack(void);
static bool get_twint_flag(void);
static void twi_busy_wait(void);
static void busy_wait_stop_cond(void);
static bool check_repeated_start(void);
static bool init_master_mode(uint8_t slave_addr, bool rw);

/*Start condition to establish MCU as the master*/
static void send_start_cond(void)
{
	TWCR = ((TWCR & CLEAR_TWCR) | ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN)));
}

/*Stop condition to reliquinsh control of bus*/
static void send_stop_cond(void)
{
	TWCR = (TWCR & CLEAR_TWCR) | ((1 << TWINT) | (1 << TWSTO) | (1 << TWEN));
}

/*Wrapper that re-issues the start condition*/
static void send_repeated_start(void)
{
	send_start_cond();
}

/*Transmit byte by clearing TWINT*/
static void transmit_byte(void)
{
	TWCR = ((TWCR & CLEAR_TWCR) | ((1 << TWINT) | (1 << TWEN)));
}

/*Acknowledgement for use in Master Receiver Mode*/
static void send_ack(void)
{
	TWCR = ((TWCR & CLEAR_TWCR) | ((1 << TWINT) | (1 << TWEA) | (1 << TWEN)));
}

/*Non-acknowledgement for use in Master Receiver Mode*/
static void send_not_ack(void)
{
	TWCR = ((TWCR & CLEAR_TWCR) | ((1 << TWINT) | (1 << TWEN)));
}

/*Return the value of the TWINT flag*/
static bool get_twint_flag(void)
{
	return (TWCR & (1 << TWINT));
}

/*Polls the TWI Control Register interrupt flag to see if busy*/
static void twi_busy_wait(void)
{
	while(!(TWCR & (1 << TWINT)));
}

/*Polls the TWSTO bit to confirm stop condition was issued*/
static void busy_wait_stop_cond(void)
{
	while(!(TWCR & (1 << TWSTO)));
}

/*Used to determine if Repeated Start was previously sent*/
static bool check_repeated_start(void)
{
	return ((TWSR & STATUS_MSK) == REPEATED_START_ACK) ? REPEATED_START : NO_REPEATED_START;
}

/*Initializes either Master Transmitter or Master Receiver Mode*/
static bool init_master_mode(uint8_t slave_addr, bool rw)
{
	/*Determine if MCU already has control of bus or not*/
	if(check_repeated_start() == NO_REPEATED_START)
	{
		send_start_cond();	
		twi_busy_wait();

		if((TWSR & STATUS_MSK) != START_ACK)
		{
			return ENTRY_ERR;
		}
	}
	
	/*Load TWDR with SLA+W or SLA+R*/
	TWDR = (rw) ? ((slave_addr << 1) & WRITE) : ((slave_addr << 1) | READ);
	transmit_byte();	
	twi_busy_wait();	

	uint8_t status = TWSR & STATUS_MSK;
	/*Verify SLA+R/W was transmitted successfully*/
	if((status != MT_SLA_ACK) && (status != MR_SLA_ACK))
	{
		return ENTRY_ERR;
	}
	
	return ENTRY_SUCCESS;
}

/* API Functions */

/*See i2c_lib.h for details*/
void init_i2c(void)
{
	/*No pre-scaler*/
	TWSR &= ~((1 << TWPS0) | (1 << TWPS1));	
	/*Set SCL frequency*/
	TWBR = BIT_RATE;
	/*Explicitly clear TWI Power Reduction Bit*/
	PRR0 &= ~(1 << PRTWI);
	/*Enable TWI operation*/
	TWCR |= (1 << TWEN);
}

/*See i2c_lib.h for details*/
bool i2c_init_mt_mode(uint8_t slave_addr)
{
	return init_master_mode(slave_addr,W);
}

/*See i2c_lib.h for details*/
bool i2c_init_mr_mode(uint8_t slave_addr)
{
	return init_master_mode(slave_addr,R);
}

/*See i2c_lib.h for details*/
uint8_t i2c_mt_write(uint8_t *data, uint8_t bytes, bool repeated_start)
{
	for(uint8_t i = 0; i < bytes; i++)
	{
		/*Write data byte into TWI Data Register*/
		TWDR = data[i];
		transmit_byte();
		twi_busy_wait();	
	
		if((TWSR & STATUS_MSK) != MT_DATA_ACK)
		{
			return MT_WRITE_ERR;
		}
	}
	/*Maintain control of bus*/
	if(repeated_start)
	{
		send_repeated_start();
		twi_busy_wait();	

		if((TWSR & STATUS_MSK) != REPEATED_START_ACK)
		{
			return MT_REENTRY_ERR;
		}
	}
	/*Relinquish control of bus*/
	else
	{
		send_stop_cond();
		/*Poll TWSTO to confirm stop condition*/
		busy_wait_stop_cond();
	}
	
	return MT_WRITE_SUCCESS;
}

/*See i2c_lib.h for details*/
uint8_t i2c_mr_read(uint8_t *data, uint8_t bytes, bool repeated_start)
{
	uint8_t read_status = 0;
	
	if(data == NULL) return MR_READ_ERR;

	while(bytes > 0)
	{
		/*Wait for a new byte to become available*/
		if(get_twint_flag())
		{	
			/*Issue Non-Acknowledgement after last byte is read*/
			if(bytes == 1)
			{
				send_not_ack();
			}

			/*Acknowledge the received byte*/
			else
			{
				send_ack();
			}
			
			twi_busy_wait(); //Wait for acknowledge or not acknowledge to be sent
			*data++ = TWDR;
			bytes--;

			read_status = TWSR & STATUS_MSK;
			/*Check that data has been received properly*/
			if((read_status != MR_DATA_ACK) && (read_status != MR_DATA_NACK))
			{
				return MR_READ_ERR;
			}
		}
	}
	/*Maintain control of bus*/
	if(repeated_start)
	{
		send_repeated_start();
		twi_busy_wait();	
	
		if((TWSR & STATUS_MSK) != REPEATED_START_ACK)
		{
			return MR_REENTRY_ERR;
		}	
	}
	/*Relinquish control of bus*/
	else
	{
		send_stop_cond();
		/*Poll TWSTO to confirm stop condition*/
		busy_wait_stop_cond();
	}
	
	return MR_READ_SUCCESS;
}
/* End of i2c_lib.c */
