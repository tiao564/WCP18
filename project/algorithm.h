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

#define INIT_FAILED 0
#define INIT_SUCCESS 1
#define OBSTACLE_SENSOR_FAIL 0
#define OBSTACLE_SENSOR_SUCCESS 1
#define ERROR 0
#define COMPLETE 1
#define MOTOR_OFF 0
#define MOTOR_ON 1
#define DOWN 0 
#define UP 1
#define DUTY_CYCLE 128
#define SIX_INCHES 200
#define LOWER_us_LIMIT 10
#define UPPER_us_LIMIT 15
#define x_lower 10
#define x_upper 15
#define y_lower 10
#define y_upper 15
#define z_lower 10
#define z_upper 15

//Initializes accelerometer, motors,vibration,ultrasonic sesnors. Sets motor speeds. 
//Returns a logic 1 if everything initialized correctly, 0 if it fails
bool initialize(void);

//Fetches the data from the two ultrasonic sensors and compares the values against predetermined limits
//Returns a logic 1 if the data is within limits, 0 if it fails
bool check_obstacle_sensors(void);

//Busy waits for enable signal, doesn't exit until it receives it 
void check_enable(void);

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
bool check_sensors(void);

//Checks acclerometer values against limits to return to above function.
//1 if in range, 0 if out or read error 
bool check_accel(void);

//Stops the encoders and clear their respective counts
void stop_clear_encoders(void);
#endif
