#ifndef REFLOW_H
#define REFLOW_H

#include <Arduino.h>

// #include <EEPROM.h>
#include "PID.h"

#ifndef NUM_REFLOW_PROFILES
#define NUM_REFLOW_PROFILES 10
#endif

#ifndef MAX_TEMP_C
#define MAX_TEMP_C 300
#endif

#ifndef MIN_TEMP_C
#define MIN_TEMP_C 0
#endif

#ifndef MIN_TIME_S
#define MIN_TIME_S 0
#endif

#ifndef MAX_TIME_S
#define MAX_TIME_S (60 * 10)
#endif

#ifndef MIN_TOTAL_RUNTIME_S
#define MIN_TOTAL_RUNTIME_S 30
#endif

#ifndef DEFAULT_PROFILES_ADDRESS
#define DEFAULT_PROFILES_ADDRESS (sizeof(PID_Constants))
#endif

#ifndef DEFAULT_FAN_ON
#define DEFAULT_FAN_ON true
#endif

#ifndef DEFAULT_PREHEAT_TEMP_C
#define DEFAULT_PREHEAT_TEMP_C 125
#endif

#ifndef DEFAULT_PREHEAT_TIME_S
#define DEFAULT_PREHEAT_TIME_S 90
#endif

#ifndef DEFAULT_SOAK_TEMP_C
#define DEFAULT_SOAK_TEMP_C 190
#endif

#ifndef DEFAULT_SOAK_TIME_S
#define DEFAULT_SOAK_TIME_S (270 - 90)
#endif

#ifndef DEFAULT_REFLOW_HOLD_TIME_S
#define DEFAULT_REFLOW_HOLD_TIME_S 0
#endif

#ifndef DEFAULT_REFLOW_TEMP
#define DEFAULT_REFLOW_TEMP 225
#endif

extern bool reflow_profile_running;

struct ReflowProfile
{
    bool fan_on;
    int preheat_temp_c;
    int preheat_time_s;
    int soak_temp_c;
    int soak_time_s;
    int reflow_temp;
    int reflow_hold_time_s;
};

extern ReflowProfile default_reflow_profile;

/*!
    @brief  Verifies if the data in a Reflow profile is proper

    @param ptr_profile_to_check pointer to the profile to verify.


    @return boolean; whether the supplied ReflowProfile Passes
*/
bool isProfileValid(ReflowProfile *ptr_profile_to_check);

/*!
    @brief  Saves the supplied array of profiles to specified address


    @return boolean; success/fail
*/
bool saveProfilesToEEPROM(ReflowProfile profiles_array[], int num_of_profiles, int address);

/**
 * @brief Get the reflow Profiles From EEPROM. stores them into the provided array
 *
 * @param profiles_array array to store the profiles retrieved from EEPROM
 * @param num_of_profiles the number of profiles that should exist
 * @param address the address on EEPROM that the profiles exist at
 * @return true. success
 * @return false. failure
 */
bool getProfilesFromEEPROM(ReflowProfile profiles_array[], int num_of_profiles, int address);

/*!
    @brief attaches all the default reflow profile values to the default profile

    @return void
*/
void initialize_default_profile(void);

#endif