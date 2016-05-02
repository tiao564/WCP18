/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver to interface DC motor 2 channel encoders. Intended
 *    for an AVR microprocessor. Utilizes 8bit Timer/Counter0 
 *	  interrupt capabilities to sample the encoder values at fixed
 *    intervals defined by the user. The API has been design to
 *    specifically interface to independent encoders.
 *
 **************************************************************/
 
#ifndef ENCODER_H_
#define ENCODER_H_

/********************************************
 * 		          Includes                  *
 ********************************************/ 
#include <stdint.h>

/********************************************
 * 		           Macros                   *
 ********************************************/
/*Translation Motor Encoder Ports*/
#define TRANS_ENCODER_A_PORT D
#define TRANS_ENCODER_B_PORT D
#define TRANS_ENCODER_A_POS  4
#define TRANS_ENCODER_B_POS  5

/*Rotational Motor Encoder Ports*/
#define ROTAT_ENCODER_A_PORT D
#define ROTAT_ENCODER_B_PORT D
#define ROTAT_ENCODER_A_POS  0
#define ROTAT_ENCODER_B_POS  1

/********************************************
 * 		      Function Prototypes           *
 ********************************************/
 
/***************************************************************
 *
 * DESCRIPTION:
 *  - Configures the translational motor encoder and rotational
 *    motor encoder inputs. Configures Timer/Counter0 to 
 *    Clear Timer on Compare (CTC) mode with a divide by 8 prescaler.
 *    Additionally, enables global interrupts and timer generated
 *    interrupts.
 *
 **************************************************************/
void init_encoders(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of OCR0A which indicates how often the timer 
 *    module generates an interrupt and samples the each of the encoder 
 *    inputs.The SAMPLING_RATE macro is a fixed value and is defined
 *    within encoder.c.
 *
 **************************************************************/
uint8_t get_sampling_rate(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the translational motor encoder count, which
 *    corresponds to the number of revolutions the motor has turned.
 *
 **************************************************************/
uint16_t get_trans_encoder_cnt(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the rotational motor encoder count, which corresponds
 *	  to the number of revolutions the motor has turned.
 *
 **************************************************************/
uint16_t get_rotat_encoder_cnt(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Resets the translational motor encoder counter to 0.
 *
 **************************************************************/
void clear_trans_encoder_cnt(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Resets the rotational motor encoder counter to 0.
 *
 **************************************************************/
void clear_rotat_encoder_cnt(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Enables encoders by turning on timer generated interrupts
 *    and starting Timer/Counter0.
 *
 **************************************************************/
void start_encoders(void);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Disables encoders by turning off Timer/Counter0 and timer
 *    generated interrupts.
 *
 **************************************************************/
void stop_encoders(void);

#endif
/* End of encoder.h */
