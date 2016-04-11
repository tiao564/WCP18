//main file
#define F_CPU 8000000UL

/* Accel, for tilt and movement */
#include "accelerometer.h"

/* Gyro for tilt */
#include "gyroscope.h"

/* i2c for gyro and accel coms to ATMEGA */
#include "i2c_lib.h"

/* Ultrasonic for object detection, progress tracking and tilt detection */
#include "ultrasonic.h"

/* Motor driver to operate motors */
#include "motor.h"

/* Encoder for progress tracking */
#include "encoder.h"

/* Vibration for danger detection */
#include "vibration.h"

/* Standard Avr library */
#include <avr/io.h>

/* Delay fucntions for timing */
#include <util/delay.h>

/* Interrupts for AVR */
#include <avr/interrupt.h>


int main()
{
  /*******************
   * Initializations *
   ******************/
  /* Set interrupt handler */
  sei();

  /* Abortion flag */
  bool abort = false;

  /**************
   * Accel Init *
   *************/
  /* Accelerometer data struct */
  accel_data a_data;
  a_data.x = 0;
  a_data.y = 0;
  a_data.z = 0;
  
  /*Init accel*/
  bool accel_init = init_accel();
  if ( accel_init != ACCEL_INIT_PASS )
  {
    //Failed to init
    abort = true;
  }
  
  /*************
   * Gyro Init *
   ************/
  /* Gyro data struct */
  gyro_data g_data;
  g_data.x = 0;
  g_data.y = 0;
  g_data.z = 0;

  /*Full-scale range value*/
  gyro_range rng = RANGE_245_DPS;

  /*Init gyro*/
  bool gyro_init = init_gyro(rng);
  if(gyro_init != GYRO_INIT_PASS)
  {
    abort = true;
  }
  enable_autorange();

  /*******************
   * Ultrasonic Init *
   ******************/
  init_ultrasonic_sensors();

  /**************
   * Motor Init *
   *************/
  #define DUTY_CYCLE 128
  init_motor_drivers();
  brake_rotational_motor();
  brake_translational_motor();
  set_rotational_motor_speed(DUTY_CYCLE);
  set_translational_motor_speed(DUTY_CYCLE);

  /****************
   * Encoder Init *
   ***************/
  #define SAMPLE_RATE 200
  init_encoders();
  clear_rotat_encoder_cnt();
  clear_trans_encoder_cnt();

  /*******************
   * Vibrations Init *
   ******************/
  init_vibration_sensors();

  /***********************
   * End Initialiaztions *
   **********************/



  /***************************
   * Varialbes for Algorithm *
   **************************/
  /* Algorithm Definitions */
  //  MOST OF THESE ARE UNKNOWN //
  #define ULTRA_OFFSET_LIMIT  50
  #define DISTANCE_TO_GROUND  400
  #define GYRO_LIMIT          200
  #define ACCEL_LIMIT         200
  #define MIN_MOVE_DISTANCE   200
  #define ENCODE_DONE         2000

  /* Variables */
  
  //Abortion flag
  abort = false;

  //Accel variables
  bool accel_status = read_accel(&a_data);

  //Gyro variables
  //???

  //Ultrasonic variables
  accum distanceA = get_obstacle_distance_cm(A);
  accum distanceB = get_obstacle_distance_cm(B);
  uint8_t AmB_dist;
  uint8_t BmA_dist;

  //Encoder variables 
  uint16_t trans_cnt = 0;
  uint16_t old_r_cnt = 0;
  uint16_t old_t_cnt = 0;
 
  /*****************
   * End Variables *
   ****************/


  /*******************
   * Begin Algorithm *
   ******************/

  /*******************
   * Start up checks *
   ******************/

  /*******************************************
   * If not correct distance to ground abort *
   ******************************************/
  if( get_obstacle_distance_cm(A) < DISTANCE_TO_GROUND || get_obstacle_distance_cm(B) < DISTANCE_TO_GROUND ) 
  {
    abort = true;
  } 

  /****************************
   * If tilted too much abort *
   ***************************/
  if( accel_status != ACCEL_READ_FAIL )
    {
    if( a_data.x > ACCEL_LIMIT || a_data.y > ACCEL_LIMIT)
    {
      abort = true; 
    }
  }
  ////////////////
  //Keep this???//
  ////////////////
  if( accel_status == ACCEL_READ_FAIL )
  {
    abort = true;
  }

  /****************************
   * If tilted too much abort *
   ***************************/
  if( g_data.x > GYRO_LIMIT || g_data.y > GYRO_LIMIT )
  {
    abort = true;
  }

  /*************
   * Motors on *
   ************/
  if(abort == false)
  {
    rotational_motor_right();   //MIGHT BE LEFT
    translational_motor_down();
  }

  /*****************************************
   * While not abort and not done sampling *
   ****************************************/
  while( (get_trans_encoder_cnt() < ENCODE_DONE) && (abort == false) )
  {
    /*********************
     * Utlrasonic checks *
     ********************/
   
    //Use distance sensors to read tilt
    distanceA = get_obstacle_distance_cm(A);
    distanceB = get_obstacle_distance_cm(B);

    AmB_dist = distanceA - distanceB;
    BmA_dist = distanceB - distanceA;

    if(AmB_dist > ULTRA_OFFSET_LIMIT && AmB_dist < ULTRA_OFFSET_LIMIT)
    {
      abort = true;
    }

    if( BmA_dist > ULTRA_OFFSET_LIMIT && BmA_dist < ULTRA_OFFSET_LIMIT)
    {
      abort = true;
    }

    /****************
     * Accel Checks *
     ***************/
    /****************************
    * If tilted too much abort *
    ***************************/
    if( a_data.x > ACCEL_LIMIT || a_data.y > ACCEL_LIMIT)
    {
      abort = true; 
    }

    /***************
     * Gyro Checks *
     **************/
    /****************************
     * If tilted too much abort *
     ***************************/
    if( g_data.x > GYRO_LIMIT || g_data.y > GYRO_LIMIT )
    {
      abort = true;
    }

    /******************
     * Encoder Checks *
     *****************/
    //We're not spinning
    if( get_rotat_encoder_cnt() == old_r_cnt || get_trans_encoder_cnt() == old_t_cnt )
    {
      abort = true; 
    }
    old_r_cnt = get_rotat_encoder_cnt();
    old_t_cnt = get_trans_encoder_cnt();

    /********************
     * Vibration Checks *
     *******************/
    if( check_vs_med_a() && check_vs_med_b() )
    {
      abort = true; 
    }

  }

  /****************
   * Brake motors *
   ***************/
  brake_rotational_motor();
  brake_translational_motor();
 
  /***********************
   * Get encode position *
   **********************/
  trans_cnt = get_trans_encoder_cnt();
  clear_trans_encoder_cnt();

  /************************
   * Turn signal light on *
   ***********************/
  if(abort == true)
  {
    //turn led on?? PORTX |= (1 << led_abort_port)
  }

  if(get_trans_encoder_cnt() >= ENCODE_DONE)
  {
    //turn led on?? PORTX |= (1 << led_done_port) 
  }
  /***********************
   * bring drill back up *
   **********************/
  while( get_trans_encoder_cnt() != trans_cnt )
  {
    translational_motor_up();     
  }

  brake_translational_motor();

  return 0;
} 
