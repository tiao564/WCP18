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

//Encoder count that equals 6"
#define SIX_INCHES 200

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

//Checks gyro values against limits to return to check_sensors()
//Returns a 1 if in range, 0 if out of range or read error
bool check_gyro(void);
//Stops the encoders and clear their respective counts
void stop_clear_encoders(void);
#endif
