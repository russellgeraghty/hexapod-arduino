#include <Arduino.h>
#include <Modbus.h>

#ifndef Walker_h
#define Walker_h

#define MAX_BUFFER 100

#define NUM_LEGS 1

/**
 * Function code errors.
 */
#define FUNCTION_NOT_SUPPORTED 1
#define REGISTER_OUT_OF_RANGE  2
#define REGISTER_NOT_AVAILABLE 3
#define WRITE_SINGLE_REGISTER_FAIL 4

/**
 * Motion commands
 */
#define MOTION_HOME 1
#define MOTION_FORWARD 2
#define MOTION_BACKWARD 3
#define MOTION_LEFT 4
#define MOTION_RIGHT 5
#define MOTION_CROUCH 6

/**
 * Supported function codes.
 */
#define WRITE_SINGLE_REGISTER 6

/**
 * Registers
 */
#define MOTION_REGISTER 1




#endif