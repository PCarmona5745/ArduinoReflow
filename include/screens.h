#ifndef SCREENS_H
#define SCREENS_H

#include "menu.h"

// currently selected index
extern int index_to_highlight;

// status screen
extern ScreenItem *status_screen_items;
extern MenuScreen status_screen;

// home screen
extern MenuItem *home_screen_menu_items;
extern MenuScreen home_screen;
extern ScreenItem *home_screen_items;

// run reflow screen
extern MenuItem *run_reflow_screen_menu_items;
extern MenuScreen run_reflow_screen;
extern ScreenItem *run_reflow_screen_items;

// profile selected to run Screen
extern MenuItem *selected_to_run_menu_items;
extern MenuScreen selected_to_run_screen;
extern ScreenItem *selected_to_run_screen_items;

// edit PID screen
extern MenuItem *edit_pid_menu_items;
extern MenuScreen edit_pid_screen;
extern ScreenItem *edit_pid_screen_items;

// edit reflow profile screen
extern MenuItem *edit_reflow_menu_items;
extern MenuScreen edit_reflow_screen;
extern ScreenItem *edit_reflow_screen_items;

// select profile to edit screen
extern MenuItem *select_profile_to_edit_menu_items;
extern MenuScreen select_profile_to_edit_screen;
extern ScreenItem *select_profile_to_edit_screen_items;

void initializeStatusScreen();
void initializeHomeScreen();
void initializeRunReflowScreen();
void initializeSelectedToRunScreen();
void initializeEditPIDScreen();
void initializeEditReflowScreen();
void initializeSelectProfileToEditScreen();

/**
 * @brief run all the initialization methods to create all the screens
 *
 */
void initializeAllScreens();

void set_item_to_highlight(MenuScreen *screen, int index);
void increment_highlight(MenuScreen *screen);
void decrement_highlight(MenuScreen *screen);

#endif