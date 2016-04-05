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

#include <stdint.h>

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
 *  - Sets the sampling frequency by dictating the rate at which
 *    the timer module generates interrupts. The maximum value of 
 *	  the input "rate" is 255. This value should be sufficiently
 *    large enough to avoid missing input changes.
 *
 **************************************************************/
void set_sampling_rate(uint8_t rate);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Returns the value of OCR0A (set by the function above).
 *    Indicates how often the timer module generates an interrupt
 *    and samples the each of the encoder inputs.
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
 *  - Disables encoders by turning off Timer/Counter0 and timer
 *    generated interrupts.
 *
 **************************************************************/
void stop_encoders(void);

#endif
/* End of encoder.h */
