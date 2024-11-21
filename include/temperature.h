#ifndef TEMPERATURE_H
#define TEMPERATURE_H

// #include "max6675.h"
#include "config.h"
#include <SPI.h>
#include "Adafruit_MAX31856.h"

#define MIN_TIME_BETWEEN_THERMO_READ 400

// extern MAX6675 thermocouple;
extern Adafruit_MAX31856 thermocouple;
extern unsigned long last_time_ran;
extern double last_temp;

/*!
    @brief  Reads the current temperature from the thermocouple and updates the provided pointer.
    @param  temp Pointer to a double where the current temperature will be stored.
    @return boolean. success or failure.
*/
bool getTemperature(double *temp);

bool initializeTemperature();

#endif