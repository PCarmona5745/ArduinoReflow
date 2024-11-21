#include "PID.h"
#include <EEPROM.h>

bool flag_PID_running = false;

bool is_PID_constants_valid(PID_Constants *ptr_constants_to_check)
{
    if (ptr_constants_to_check == NULL)
    {
        return false;
    }

    if (ptr_constants_to_check->PID_d < MIN_CONSTANTS_VALUE || ptr_constants_to_check->PID_i < MIN_CONSTANTS_VALUE || ptr_constants_to_check->PID_k < MIN_CONSTANTS_VALUE)
    {
        return false;
    }

    if (ptr_constants_to_check->PID_d > MAX_CONSTANTS_VALUE || ptr_constants_to_check->PID_i > MAX_CONSTANTS_VALUE || ptr_constants_to_check->PID_k > MAX_CONSTANTS_VALUE)
    {
        return false;
    }

    if (isnan(ptr_constants_to_check->PID_d) || isnan(ptr_constants_to_check->PID_i) || isnan(ptr_constants_to_check->PID_k) || isnan(ptr_constants_to_check->threshold))
    {
        return false;
    }

    return true;
}

bool save_PID_constants_to_EEPROM(int index, PID_Constants *ptr_constants)
{
    if ((index + sizeof(PID_Constants)) > EEPROM.length())
    {
        return false;
    }

    EEPROM.put(index, *ptr_constants);

    return true;
}

bool get_PID_constants_from_EEPROM(int index, PID_Constants *ptr_constants)
{
    if ((index + sizeof(PID_Constants)) > EEPROM.length())
    {
        return false;
    }

    EEPROM.get(index, *ptr_constants);

    return true;
}

bool calculatePID(PID *pid)
{

    // Update the temperature history
    pid->temperatureHistory[pid->historyIndex] = pid->input;
    pid->historyIndex = (pid->historyIndex + 1) % 5; // Update the index, wrapping around

    pid->error = pid->target - pid->input;

    if (abs(pid->error) <= 0.5f)
    {
        pid->integral = 0.0f;
    }
    else
    {

        pid->integral += pid->error * pid->dt;
    }

    if (pid->integral >= 1000.0f)
    {
        pid->integral = 1000.0f;
    }
    if (pid->integral <= -1000.0f)
    {
        pid->integral = -1000.0f;
    }

    // pid->derivative = (pid->error - pid->prev_error) / pid->dt;

    // Calculate the derivative using the current and 5th previous reading
    int previousIndex = (pid->historyIndex + 4) % 5;
    double previousTemperature = pid->temperatureHistory[previousIndex];
    pid->derivative = (pid->input - previousTemperature) / (pid->dt * 5);

    pid->output = pid->constants.PID_k * pid->error +
                  pid->constants.PID_i * pid->integral +
                  pid->constants.PID_d * pid->derivative;

    pid->prev_error = pid->error;

    // csvPID(pid);

    return pid->output > pid->constants.threshold;
}

void csvPID(PID *pid)
{
    // Time,dt,input,target,error,P,I,D,output,threshold
    Serial.print(millis() / 1000.0f);
    Serial.print(",");
    Serial.print(pid->dt);
    Serial.print(",");
    Serial.print(pid->input);
    Serial.print(",");
    Serial.print(pid->target);
    Serial.print(",");
    Serial.print(pid->error);
    Serial.print(",");
    Serial.print(pid->constants.PID_k * pid->error);
    Serial.print(",");
    Serial.print(pid->integral * pid->constants.PID_i);
    Serial.print(",");
    Serial.print(pid->derivative * pid->constants.PID_d);
    Serial.print(",");
    Serial.print(pid->output);
    Serial.print(",");
    Serial.println(pid->constants.threshold);
}
