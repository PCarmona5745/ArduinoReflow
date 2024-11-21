#include "reflow.h"
#include <EEPROM.h>

ReflowProfile default_reflow_profile;

bool reflow_profile_running = false;

void initialize_default_profile(void)
{
    // setup the default reflow profile:
    default_reflow_profile.fan_on = DEFAULT_FAN_ON;
    default_reflow_profile.preheat_temp_c = DEFAULT_PREHEAT_TEMP_C;
    default_reflow_profile.preheat_time_s = DEFAULT_PREHEAT_TIME_S;
    default_reflow_profile.soak_temp_c = DEFAULT_SOAK_TEMP_C;
    default_reflow_profile.soak_time_s = DEFAULT_SOAK_TIME_S;
    default_reflow_profile.reflow_hold_time_s = DEFAULT_REFLOW_HOLD_TIME_S;
    default_reflow_profile.reflow_temp = DEFAULT_REFLOW_TEMP;
}

bool isProfileValid(ReflowProfile *ptr_profile_to_check)
{

    if (ptr_profile_to_check == NULL)
    {
        return false;
    }

    if (ptr_profile_to_check->preheat_temp_c < MIN_TEMP_C || ptr_profile_to_check->preheat_temp_c > MAX_TEMP_C)
    {
        return false;
    }

    if (ptr_profile_to_check->soak_temp_c < MIN_TEMP_C || ptr_profile_to_check->soak_temp_c > MAX_TEMP_C)
    {
        return false;
    }

    if (ptr_profile_to_check->reflow_temp < MIN_TEMP_C || ptr_profile_to_check->reflow_temp > MAX_TEMP_C)
    {
        return false;
    }

    if (ptr_profile_to_check->preheat_time_s < MIN_TIME_S || ptr_profile_to_check->preheat_time_s > MAX_TIME_S)
    {
        return false;
    }

    if (ptr_profile_to_check->soak_time_s < MIN_TIME_S || ptr_profile_to_check->soak_time_s > MAX_TIME_S)
    {
        return false;
    }

    if (ptr_profile_to_check->reflow_hold_time_s < MIN_TIME_S || ptr_profile_to_check->reflow_hold_time_s > MAX_TIME_S)
    {
        return false;
    }

    if (ptr_profile_to_check->preheat_temp_c > ptr_profile_to_check->soak_temp_c)
    {
        return false;
    }

    if (ptr_profile_to_check->soak_temp_c > ptr_profile_to_check->reflow_temp)
    {
        return false;
    }

    if (ptr_profile_to_check->preheat_temp_c > ptr_profile_to_check->reflow_temp)
    {
        return false;
    }

    if ((ptr_profile_to_check->soak_time_s + ptr_profile_to_check->preheat_time_s + ptr_profile_to_check->reflow_hold_time_s) < MIN_TOTAL_RUNTIME_S)
    {
        return false;
    }

    return true;
}

bool getProfilesFromEEPROM(ReflowProfile profiles_array[], int num_of_profiles, int address)
{
    int size = num_of_profiles * sizeof(ReflowProfile);
    int eepromSize = EEPROM.length();

    // Check if the read operation goes beyond the EEPROM size
    if ((address + size) > eepromSize)
    {
        return false; // Read operation exceeds EEPROM size
    }

    EEPROM.get(address, profiles_array);

    return true;
}

bool saveProfilesToEEPROM(ReflowProfile profiles_array[], int num_of_profiles, int address)
{
    int size = num_of_profiles * sizeof(ReflowProfile);
    int eepromSize = EEPROM.length();

    // Check if the write operation goes beyond the EEPROM size
    if ((address + size) > eepromSize)
    {
        return false; // Write operation exceeds EEPROM size
    }

    EEPROM.put(address, profiles_array);

    return true;
}
