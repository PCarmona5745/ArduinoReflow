#include "temperature.h"
#include <Arduino.h>

// MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
// Adafruit_MAX31856 thermocouple(THERMO_CS, 31, THERMO_DO, THERMO_CLK);
Adafruit_MAX31856 thermocouple(THERMO_CS);
unsigned long last_time_ran = 0;
double last_temp = 0.0f;

bool getTemperature(double *temp)
{
    Serial.println("Going to read temperature");
    Serial.println("Checking MIN_TIME_BETWEEN_THERMO_READ");

    if (millis() - last_time_ran < MIN_TIME_BETWEEN_THERMO_READ)
    {
        Serial.println("Too soon between temp readings, passing.");
        *temp = last_temp;
        return true;
    }

    double temp_temp;
    // double temp_temp = 0.0f;
    Serial.print("Taking temp reading now.");

    temp_temp = thermocouple.readThermocoupleTemperature();
    Serial.print("Temp read is: ");
    Serial.println(temp_temp);
    if (isnan(temp_temp) || temp_temp <= 1.0 || temp_temp > 1000)
    {
        Serial.println("PROBABLY failed to read temp.");

        uint8_t fault = thermocouple.readFault();
        if (fault)
        {
            Serial.println("Thermocouple fault(s) detected!");

            if (fault & MAX31856_FAULT_CJRANGE)
                Serial.println("Cold Junction Range Fault");
            if (fault & MAX31856_FAULT_TCRANGE)
                Serial.println("Thermocouple Range Fault");
            if (fault & MAX31856_FAULT_CJHIGH)
                Serial.println("Cold Junction High Fault");
            if (fault & MAX31856_FAULT_CJLOW)
                Serial.println("Cold Junction Low Fault");
            if (fault & MAX31856_FAULT_TCHIGH)
                Serial.println("Thermocouple High Fault");
            if (fault & MAX31856_FAULT_TCLOW)
                Serial.println("Thermocouple Low Fault");
            if (fault & MAX31856_FAULT_OVUV)
                Serial.println("Over/Under Voltage Fault");
            if (fault & MAX31856_FAULT_OPEN)
                Serial.println("Thermocouple Open Fault");
        }

        last_time_ran = millis();
        return false;
    }
    Serial.println("Supposedly successful temp read.");

    *temp = temp_temp;
    last_temp = temp_temp;
    last_time_ran = millis();

    return true;
}

bool initializeTemperature()
{
    if (!thermocouple.begin())
    {
        Serial.println("THERMOCOUPLE ERROR.");
        return false;
    }

    thermocouple.setThermocoupleType(MAX31856_TCTYPE_K);

    Serial.print("Thermocouple type: ");
    switch (thermocouple.getThermocoupleType())
    {
    case MAX31856_TCTYPE_B:
        Serial.println("B Type");
        break;
    case MAX31856_TCTYPE_E:
        Serial.println("E Type");
        break;
    case MAX31856_TCTYPE_J:
        Serial.println("J Type");
        break;
    case MAX31856_TCTYPE_K:
        Serial.println("K Type");
        break;
    case MAX31856_TCTYPE_N:
        Serial.println("N Type");
        break;
    case MAX31856_TCTYPE_R:
        Serial.println("R Type");
        break;
    case MAX31856_TCTYPE_S:
        Serial.println("S Type");
        break;
    case MAX31856_TCTYPE_T:
        Serial.println("T Type");
        break;
    case MAX31856_VMODE_G8:
        Serial.println("Voltage x8 Gain mode");
        break;
    case MAX31856_VMODE_G32:
        Serial.println("Voltage x8 Gain mode");
        break;
    default:
        Serial.println("Unknown");
        break;
    }
    return true;
}