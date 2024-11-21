#include <Arduino.h>
#include "buttons.h"

// Time constants
const unsigned long BUTTON_RATE_LIMIT = 200; // 200 milliseconds for rate limit

// Last press time for each button
unsigned long lastPressTimeLeft = 0;
unsigned long lastPressTimeRight = 0;
unsigned long lastPressTimeUp = 0;
unsigned long lastPressTimeDown = 0;
unsigned long lastPressTimeSelect = 0;

// flags
bool left_button_pressed;
bool right_button_pressed;
bool up_button_pressed;
bool down_button_pressed;
bool select_button_pressed;

void checkButtonStates(bool *left_button_pressed, bool *right_button_pressed, bool *down_button_pressed, bool *up_button_pressed, bool *select_button_pressed)
{
    unsigned long currentTime = millis();

    // Check left button with rate limit
    if (currentTime - lastPressTimeLeft > BUTTON_RATE_LIMIT && !digitalRead(LB))
    {
        *left_button_pressed = true;
        lastPressTimeLeft = currentTime;
    }
    else
    {
        *left_button_pressed = false;
    }

    if (currentTime - lastPressTimeRight > BUTTON_RATE_LIMIT && !digitalRead(RB))
    {
        *right_button_pressed = true;
        lastPressTimeRight = currentTime;
    }
    else
    {
        *right_button_pressed = false;
    }
    if (currentTime - lastPressTimeDown > BUTTON_RATE_LIMIT && !digitalRead(DB))
    {
        *down_button_pressed = true;
        lastPressTimeDown = currentTime;
    }
    else
    {
        *down_button_pressed = false;
    }
    if (currentTime - lastPressTimeUp > BUTTON_RATE_LIMIT && !digitalRead(UB))
    {
        *up_button_pressed = true;
        lastPressTimeUp = currentTime;
    }
    else
    {
        *up_button_pressed = false;
    }
    if (currentTime - lastPressTimeSelect > BUTTON_RATE_LIMIT && !digitalRead(SB))
    {
        *select_button_pressed = true;

        lastPressTimeSelect = currentTime;
    }
    else
    {
        *select_button_pressed = false;
    }

    return;
}

void resetButtonStates(bool *left_button_pressed, bool *right_button_pressed, bool *down_button_pressed, bool *up_button_pressed, bool *select_button_pressed)
{
    *left_button_pressed = false;
    *right_button_pressed = false;
    *down_button_pressed = false;
    *up_button_pressed = false;
    *select_button_pressed = false;

    return;
}
