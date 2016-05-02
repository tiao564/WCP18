/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - API for communication using the I2C protocol. This API 
 *    directly interfaces the Two-Wire Interface (TWI) hardware
 *    of an Atmel AVR microntroller. Please note that this 
 *	  implementation is not interrupt driven. Presently, only
 *    Master Transmitter Mode and Master Receiver Mode have been
 *    implemented.
 *
 **************************************************************/
 
#ifndef I2C_LIB_H_
#define I2C_LIB_H_

/********************************************
 * 		          Includes                  *
 ********************************************/
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
/*TWI Status Codes*/
#define START_ACK   	   0x08
#define REPEATED_START_ACK 0x10
#define MT_SLA_ACK		   0x18
#define MT_DATA_ACK 	   0x28
#define MR_SLA_ACK  	   0x40
#define MR_DATA_ACK 	   0x50
#define MR_DATA_NACK	   0x58

/********************************************
 * 		         Typedefs                   *
 ********************************************/
/*I2C Error Codes*/
typedef enum {
	ENTRY_ERR,
	ENTRY_PASS,
	MT_WRITE_ERR,
	MT_WRITE_PASS,
	MT_REENTRY_ERR,
	MR_READ_ERR,
	MR_READ_PASS,
	MR_REENTRY_ERR,
}i2c_err;

/********************************************
 * 		      Function Prototypes           *
 ********************************************/
 
/***************************************************************
 *
 * DESCRIPTION:
 *  - Initialization function for TWI unit. This function must
 *    be called before reading or writing using the I2C protocol.
 *    By default, this function will establish the SCL frequency
 *    as 100KHz which is known as Standard Mode. 
 *
 **************************************************************/
void init_i2c(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Performs the necessary work to enter Master Transmitter
 *    Mode (MT). If successful, the function will return the 
 *    code MT_ENTRY_SUCCESS. This function must be called
 *    be called before calling i2c_mt_write(). The address of
 *    the slave receiver must be provided as an input.
 *
 **************************************************************/
i2c_err i2c_init_mt_mode(uint8_t slave_addr);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Allows an arbitrary number of bytes to be written to a
 *    slave receiver. The data to be written must be provided 
 *    as a pointer to a buffer of bytes. The number of bytes 
 *    you wish to write must also be provided. The user must
 *    inidcate whether the transmission should end with a 
 *    stop condition or a repeated start condition. This function
 *    cannot be called unless i2c_init_mt_mode() has been called.
 *
 **************************************************************/
i2c_err i2c_mt_write(uint8_t *data, uint8_t bytes, bool repeated_start);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Performs necessary work to enter Master Receiver (MR) Mode.
 *    If successful, the function will return the code 
 *    MR_ENTRY_SUCCESS. This function must be called before 
 *    calling i2c_mt_read(). The address of the slave transmitter
 *    must be provided as input.
 *
 **************************************************************/ 
i2c_err i2c_init_mr_mode(uint8_t slave_addr);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Allows an arbitrary number of bytes to be read from a slave
 *    transmitter. The user must provide a pointer to a buffer
 *    where the received data is to be stored. The number of bytes
 *    to be read must also be indicated. The "repreated_start"
 *    boolean value allows the user to decide whether a stop
 *    condition or repeated start condition will be sent when
 *    the transaction has completed. This function cannot be called
 *    unless i2c_init_mr_mode() has been called.
 *
 **************************************************************/ 
i2c_err i2c_mr_read(uint8_t *data, uint8_t bytes, bool repeated_start);

#endif
/* End i2c_lib.h */
