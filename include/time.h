#ifndef TIME_H
#define TIME_H

#include "RTClib.h"

extern RTC_DS3231 rtc;
extern uint32_t time_s;
extern unsigned long previousMillis;
extern uint32_t previous_time;

/*!
    @brief  Return current time in seconds


    @return None; sets the pointer you supply.
*/
void getTimeNow(uint32_t *time);

/*!
    @brief  sets the previousMillis variable to whatever millis() is right now


    @return None; sets the external variable
*/
void setPreviousMillis(void);

/*!
    @brief  sets the previous_time variable to whatever it is right now


    @return None; sets the external variable
*/
void setPreviousTime(void);

bool initializeRTC(void);

#endif