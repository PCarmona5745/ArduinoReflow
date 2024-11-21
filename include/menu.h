#ifndef MENU_H
#define MENU_H
#include "Adafruit_SSD1306.h"
#ifndef STR_LEN
#define STR_LEN 10
#endif

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 8
#endif

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 64
#endif

extern Adafruit_SSD1306 display;

struct MenuItem
{
    char text[STR_LEN];
    int mode;
    bool highlighted;
};

struct ScreenItem
{
    char text[STR_LEN];
};

struct MenuScreen
{
    int numMenuItems;
    MenuItem *menuItems;
    int selectedMenuItemIndex;
    int numScreenItems;
    ScreenItem screenItems[WINDOW_SIZE];
};

bool initializeScreen(void);

void drawMenuItem(MenuItem *ptr_menu_item, uint8_t pos_x, uint8_t pos_y);

void drawScreenItem(ScreenItem *ptr_screen_item, uint8_t pos_x, uint8_t pos_y);

void drawMenuScreen(MenuScreen *ptr_menu_screen);

void initializeMenuItem(MenuItem *ptr_menu_item, char *text, int text_length, int mode, bool highlighted);

void initializeScreenItem(ScreenItem *ptr_screen_item, char *text, int text_length);

void initializeMenuScreen(MenuScreen *ptr_menu_screen, int numMenuItems, MenuItem *menuItems, int selectedMenuItemIndex, int numScreenItems, ScreenItem screenItems[]);

#endif