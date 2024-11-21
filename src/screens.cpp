#include "screens.h"
#include <Arduino.h>
#include "config.h"

// currently selected index for the menu item
int index_to_highlight = 0;

// status screen
ScreenItem *status_screen_items = new ScreenItem[4];
MenuScreen status_screen;

// home screen
MenuItem *home_screen_menu_items = new MenuItem[5];
ScreenItem *home_screen_items = new ScreenItem[4];
MenuScreen home_screen;

// run reflow screen
MenuItem *run_reflow_screen_menu_items = new MenuItem[11];
MenuScreen run_reflow_screen;
ScreenItem *run_reflow_screen_items = new ScreenItem[8];

// profile selected to run Screen
MenuItem *selected_to_run_menu_items = new MenuItem[3];
MenuScreen selected_to_run_screen;
ScreenItem *selected_to_run_screen_items = new ScreenItem[8];

// edit PID screen
MenuItem *edit_pid_menu_items = new MenuItem[7];
MenuScreen edit_pid_screen;
ScreenItem *edit_pid_screen_items = new ScreenItem[1];

// edit reflow profile screen
MenuItem *edit_reflow_menu_items = new MenuItem[9];
MenuScreen edit_reflow_screen;
ScreenItem *edit_reflow_screen_items = new ScreenItem[1];

// select profile to edit screen
MenuItem *select_profile_to_edit_menu_items = new MenuItem[11];
MenuScreen select_profile_to_edit_screen;
ScreenItem *select_profile_to_edit_screen_items = new ScreenItem[8];

void initializeStatusScreen()
{
    initializeScreenItem(&(status_screen_items[0]), "TEMP", 4);
    initializeScreenItem(&(status_screen_items[1]), "0", 1);
    initializeScreenItem(&(status_screen_items[2]), "TARGET", 6);
    initializeScreenItem(&(status_screen_items[3]), "0", 1);

    initializeMenuScreen(&status_screen, 0, NULL, -1, 4, status_screen_items);
}

void initializeHomeScreen()
{
    initializeScreenItem(&(home_screen_items[0]), "TEMP", 4);
    initializeScreenItem(&(home_screen_items[1]), "0", 1);
    initializeScreenItem(&(home_screen_items[2]), "TARGET", 6);
    initializeScreenItem(&(home_screen_items[3]), "0", 1);

    initializeMenuItem(&(home_screen_menu_items[0]), "Cancel", 6, MODE_STATUS, false);
    initializeMenuItem(&(home_screen_menu_items[1]), "Run", 3, MODE_SELECT_PROFILE_TO_RUN, false);
    initializeMenuItem(&(home_screen_menu_items[2]), "Edit Prof", 9, MODE_SELECT_PROFILE_TO_EDIT, false);
    initializeMenuItem(&(home_screen_menu_items[3]), "Edit PID", 8, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(home_screen_menu_items[4]), "Just Heat", 9, MODE_HEAT_TO_TARGET, false);

    initializeMenuScreen(&home_screen, 5, home_screen_menu_items, 0, 4, home_screen_items);
}

void initializeRunReflowScreen()
{
    // this is MODE_SELECT_PROFILE_TO_RUN; back returns you to MODE_HOME

    // initialize blank screen items; to be populated with the reflow params as user scrolls
    for (int i = 0; i < 8; i++)
    {
        initializeScreenItem(&(run_reflow_screen_items[i]), " ", 1);
    }

    initializeMenuItem(&(run_reflow_screen_menu_items[0]), "Cancel", 6, MODE_HOME, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[1]), "1", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[2]), "2", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[3]), "3", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[4]), "4", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[5]), "5", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[6]), "6", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[7]), "7", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[8]), "8", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[9]), "9", 1, MODE_PROFILE_SELECTED_TO_RUN, false);
    initializeMenuItem(&(run_reflow_screen_menu_items[10]), "10", 2, MODE_PROFILE_SELECTED_TO_RUN, false);

    initializeMenuScreen(&run_reflow_screen, 11, run_reflow_screen_menu_items, 0, 8, run_reflow_screen_items);
}

void initializeSelectedToRunScreen()
{
    // this is MODE_PROFILE_SELECTED_TO_RUN; run brings you to MODE_PROFILE_SELECTED_RUNNING; back brings you to MODE_SELECT_PROFILE_TO_RUN;
    // edit brings you to MODE_EDIT_SELECTED_PROFILE
    // initialize blank screen items; to be populated with the reflow params as user scrolls
    for (int i = 0; i < 8; i++)
    {
        initializeScreenItem(&(selected_to_run_screen_items[8]), " ", 1);
    }

    initializeMenuItem(&(selected_to_run_menu_items[0]), "Cancel", 6, MODE_SELECT_PROFILE_TO_RUN, false);
    initializeMenuItem(&(selected_to_run_menu_items[1]), "Run", 3, MODE_PROFILE_SELECTED_RUNNING, false);
    initializeMenuItem(&(selected_to_run_menu_items[2]), "Edit", 4, MODE_EDIT_SELECTED_PROFILE, false);

    initializeMenuScreen(&selected_to_run_screen, 3, selected_to_run_menu_items, 0, 8, selected_to_run_screen_items);
}

void initializeEditPIDScreen()
{

    initializeScreenItem(&(edit_pid_screen_items[0]), " ", 1);

    initializeMenuItem(&(edit_pid_menu_items[0]), "Cancel", 6, MODE_HOME, false);
    initializeMenuItem(&(edit_pid_menu_items[1]), "K", 6, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(edit_pid_menu_items[2]), "I", 6, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(edit_pid_menu_items[3]), "D", 6, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(edit_pid_menu_items[4]), "Thresh.", 7, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(edit_pid_menu_items[5]), "Inc.", 4, MODE_EDIT_PID_PRAMS, false);
    initializeMenuItem(&(edit_pid_menu_items[6]), "Save", 4, MODE_EDIT_PID_PRAMS, false);

    initializeMenuScreen(&edit_pid_screen, 7, edit_pid_menu_items, 0, 1, edit_pid_screen_items);
}

void initializeEditReflowScreen()
{
    // just one screen item to display currently-hovered reflow parameter
    initializeScreenItem(&(edit_reflow_screen_items[0]), " ", 1);

    initializeMenuItem(&(edit_reflow_menu_items[0]), "Cancel", 6, MODE_HOME, false);
    initializeMenuItem(&(edit_reflow_menu_items[1]), "Pre Temp", 8, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[2]), "Pre Time", 8, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[3]), "Soak Temp", 9, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[4]), "Soak Time", 9, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[5]), "Refl Temp", 6, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[6]), "Hold Temp", 9, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[7]), "Fan", 3, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(edit_reflow_menu_items[8]), "Save", 4, MODE_EDIT_SELECTED_PROFILE, false);

    initializeMenuScreen(&edit_reflow_screen, 9, edit_reflow_menu_items, 0, 1, edit_reflow_screen_items);
}

void initializeSelectProfileToEditScreen()
{
    // all blank screen items
    for (int i = 0; i < 8; i++)
    {
        initializeScreenItem(&(select_profile_to_edit_screen_items[i]), " ", 1);
    }
    initializeMenuItem(&(select_profile_to_edit_menu_items[0]), "Cancel", 6, MODE_HOME, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[1]), "1", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[2]), "2", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[3]), "3", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[4]), "4", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[5]), "5", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[6]), "6", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[7]), "7", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[8]), "8", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[9]), "9", 1, MODE_EDIT_SELECTED_PROFILE, false);
    initializeMenuItem(&(select_profile_to_edit_menu_items[10]), "10", 2, MODE_EDIT_SELECTED_PROFILE, false);

    initializeMenuScreen(&select_profile_to_edit_screen, 11, select_profile_to_edit_menu_items, 0, 8, select_profile_to_edit_screen_items);
}

void initializeAllScreens()
{
    initializeStatusScreen();
    initializeHomeScreen();
    initializeRunReflowScreen();
    initializeSelectedToRunScreen();
    initializeEditPIDScreen();
    initializeEditReflowScreen();
    initializeSelectProfileToEditScreen();
}

void set_item_to_highlight(MenuScreen *screen, int index)
{
    int num_items = screen->numMenuItems;
    for (int i = 0; i < num_items; i++)
    {
        screen->menuItems[i].highlighted = false;
    }

    screen->menuItems[index].highlighted = true;
    screen->selectedMenuItemIndex = index;
    index_to_highlight = index;
}

void increment_highlight(MenuScreen *screen)
{
    int num_items = screen->numMenuItems;
    if (index_to_highlight + 1 < num_items)
    {
        index_to_highlight++;
        set_item_to_highlight(screen, index_to_highlight);
    }
}

void decrement_highlight(MenuScreen *screen)
{
    if (index_to_highlight > 0)
    {
        index_to_highlight--;
        set_item_to_highlight(screen, index_to_highlight);
    }
}