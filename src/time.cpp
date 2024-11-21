#include "time.h"
#include "RTClib.h"
#include "config.h"

RTC_DS3231 rtc;
uint32_t time_s = 0;
unsigned long previousMillis = 0L;
uint32_t previous_time = 0;

/**
 * @brief Get the current time in unix time (seconds)
 *
 * @param time pointer to the container that you want to store the resulting time in.
 */
void getTimeNow(uint32_t *time)
{
    // seconds since unix time
    *time = rtc.now().unixtime();
}

/**
 * @brief Set the PreviousMillis container to now
 *
 */
void setPreviousMillis(void)
{
    previousMillis = millis();
}

/**
 * @brief Set the Previous Time container to now
 *
 */
void setPreviousTime(void)
{
    previous_time = rtc.now().unixtime();
}

/**
 * @brief Initializes the RTC communication. Will try to look for an RTC for MAX_TRIES
 *
 * @return true. Initialized and found RTC.
 * @return false. Could not find RTC after repeated failures.
 */
bool initializeRTC(void)
{
    // if no rtc is found, try again until MAX_TRIES is reached
    int tries = 0;
    while (!rtc.begin() && tries < MAX_TRIES)
    {
        Serial.println("Couldn't find RTC, trying again");
        Serial.flush();
        tries++;
        delay(1000);
    }

    if (tries < MAX_TRIES)
    {
        // set time to when program was compiled (actual time is not really important)
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        return true;
    }
    else
    {
        return false;
    }
}