#ifndef PID_H
#define PID_H

#include <Arduino.h>

// #include <EEPROM.h>

#ifndef DEFAULT_CONSTANTS_ADDRESS
#define DEFAULT_CONSTANTS_ADDRESS 0
#endif

#ifndef MIN_CONSTANTS_VALUE
#define MIN_CONSTANTS_VALUE 0.0f
#endif

#ifndef MAX_CONSTANTS_VALUE
#define MAX_CONSTANTS_VALUE 1000.0f
#endif

extern bool flag_PID_running;

struct PID_Constants
{
    double PID_k;
    double PID_i;
    double PID_d;
    // output relative to threshold will dictate if error is big enough to warrant the heater to turn on
    double threshold;
};

#ifndef DEFAULT_PROFILES_ADDRESS
#define DEFAULT_PROFILES_ADDRESS (sizeof(PID_Constants))
#endif

// struct to represent PID stuff
struct PID
{
    // PID constants
    PID_Constants constants;

    // time that has passed
    double dt;

    // inputs
    double error;
    double prev_error;
    double input;
    double target;

    // calculated filter components
    double integral;
    double derivative;

    // output
    double output;
    // output relative to threshold will dictate if error is big enough to warrant the heater to turn on

    double temperatureHistory[5]; // Array to store the last 5 temperature readings
    int historyIndex;             // Index for the current position in the temperature history array
};

/*!
    @brief  Verifies if the PID constants are proper


    @return boolean; whether the supplied constants struct Passes
*/
bool is_PID_constants_valid(PID_Constants *ptr_constants_to_check);

/*!
    @brief  Writes the supplied PID constants to EEPROM

    @param index an int which is the address to write the data to.

    @param ptr_constants a pointer to the PID_Constants you want to write to EEPROM

    @return boolean: success = true. fail = false
*/
bool save_PID_constants_to_EEPROM(int index, PID_Constants *ptr_constants);

/*!
    @brief  Writes the supplied PID constants to EEPROM

    @param index an int which is the address to write the data to.

    @param ptr_constants a pointer to the PID_Constants you want to write to EEPROM

    @return boolean: success = true. fail = false
*/
bool get_PID_constants_from_EEPROM(int index, PID_Constants *ptr_constants);

/**
 * @brief calculate PID
 *
 * @param pid pointer to PID struct that has the data for PID
 * @return true run heater
 * @return false don't run heater
 */
bool calculatePID(PID *pid);

/**
 * @brief Prints a csv of the states of a PID object
 *
 * @param pid pointer to the pid object
 */
void csvPID(PID *pid);

#endif