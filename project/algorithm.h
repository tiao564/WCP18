/*****************************
 *Logan Friedman(2016)
 *
 *DESCRIPTION:
 * -Algorithm for soil sampling mechanism. Uses Driver APIS written by Nicholas Shanahan
 *
 ******************************/




#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <stdint.h>
#include <stdbool.h>

//Pin locations for enable and leds
#define ENABLE_POS 4 
#define LED_MOTOR_POS 5
#define LED_COMPLETE_POS 6
#define LED_ERROR_POS 7 

//Masks to configure pins
#define ENABLE_MASK 1
#define LED_MOTOR_MASK 2
#define LED_COMPLETE_MASK 3
#define LED_ERROR_MASK 4
//Initialization status codes
#define INIT_FAILED 0
#define INIT_SUCCESS 1

//status code to indicate if drilling is completely finished
#define ON 1
#define OFF 0

//Status codes to indicate whether obstacle sensors readings are in range
#define OBSTACLE_SENSOR_FAIL 0
#define OBSTACLE_SENSOR_SUCCESS 1

//Status codes for exit code(completed or exited with errors)
#define ERROR 0
#define COMPLETE 1

//Status codes to indicate whether motors are on
#define MOTOR_OFF 0
#define MOTOR_ON 1

//Status codes for directions of motors
#define DOWN 0 
#define UP 1

//duty cycle for motors
#define DUTY_CYCLE 128

//Encoder variables (6" count and sampling rate)
#define SIX_INCHES 200
#define ENCODER_SAMPLING_RATE 200
//ultrasonic limits 
#define LOWER_us_LIMIT 10
#define UPPER_us_LIMIT 15

//accelerometer limits
#define ax_lower 10
#define ax_upper 15
#define ay_lower 10
#define ay_upper 15
#define az_lower 10
#define az_upper 15

//gyro limits
#define gx_lower 10
#define gx_upper 15
#define gy_lower 10
#define gy_upper 15
#define gz_lower 10
#define gz_upper 15

//timeout variable
#define TIMEOUT 0
//Initializes accelerometer, motors,vibration,ultrasonic sesnors. Sets motor speeds. 
//Returns a logic 1 if everything initialized correctly, 0 if it fails
bool initialize(void);

//Fetches the data from the two ultrasonic sensors and compares the values against predetermined limits
//Returns a logic 1 if the data is within limits, 0 if it fails
bool check_obstacle_sensors(void);

//Sets LED pins as outputs and Enable pin as input
//Busy waits for enable signal, doesn't exit until it receives it 
void init_pins_check_enable(void);

//Disables drivers and sends a signal to user based on exit code (error vs completion)
void send_signal_and_disable(bool code);

//Starts the translational motor going down, and the rotational motor to turn right
void start_motors_down(void);

//Starts motors, translational moves up, rotational turns left
void start_motors_up(void);

//Brakes the motors
void stop_motors(void);

//Checks accelerometer, gyroscope and vibration sensors to see if any has been triggered or in unsafe range
//Returns 1 if no errors, 0 if triggers
bool check_encoder_sensors(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans);

//Checks acclerometer values against limits to return to check_encoder_sensors().
//1 if in range, 0 if out or read error 
bool check_accel(void);

//Checks gyro values against limits to return to check_encoder_sensors()
//Returns a 1 if in range, 0 if out of range or read error
bool check_gyro(void);

//Checks previous encoder counts and new encoder counts to assure motors are moving. Returns 0 if counts are equal(motors not moving) 1 if counts are different
bool check_encoders(uint16_t prev_rotat,uint16_t rotat,uint16_t prev_trans,uint16_t trans);

//clears the two encoder counts
void clear_encoders(void);
#endif
