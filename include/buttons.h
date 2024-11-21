#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"

#ifndef LB
#define LB 7
#endif
#ifndef UB
#define UB 8
#endif
#ifndef RB
#define RB 9
#endif
#ifndef DB
#define DB 10
#endif
#ifndef SB
#define SB 3
#endif

// flags
extern bool left_button_pressed;
extern bool right_button_pressed;
extern bool up_button_pressed;
extern bool down_button_pressed;
extern bool select_button_pressed;

/*!
    @brief  Check the states of various buttons and assign their states to corresponding flags.

    This function checks if any of the specified buttons are pressed and updates the respective flags.
    It's used to handle user inputs from the buttons in the system.

    @param left_button_pressed A pointer to a boolean flag that will be set to true if the left button is pressed.
    @param right_button_pressed A pointer to a boolean flag that will be set to true if the right button is pressed.
    @param down_button_pressed A pointer to a boolean flag that will be set to true if the down button is pressed.
    @param up_button_pressed A pointer to a boolean flag that will be set to true if the up button is pressed.
    @param select_button_pressed A pointer to a boolean flag that will be set to true if the select button is pressed.

    @return None; this function updates the values of the pointers provided to reflect the current button states.
*/
void checkButtonStates(bool *left_button_pressed, bool *right_button_pressed, bool *down_button_pressed, bool *up_button_pressed, bool *select_button_pressed);

/*!
    @brief  Reset all button states to 0.



    @param left_button_pressed A pointer to left button boolean flag
    @param right_button_pressed A pointer to right button boolean flag
    @param down_button_pressed A pointer to down button boolean flag
    @param up_button_pressed A pointer to up button boolean flag that
    @param select_button_pressed A pointer to select button boolean flag

    @return None; this function updates the values of the pointers provided to reflect the current button states.
*/
void resetButtonStates(bool *left_button_pressed, bool *right_button_pressed, bool *down_button_pressed, bool *up_button_pressed, bool *select_button_pressed);

#endif