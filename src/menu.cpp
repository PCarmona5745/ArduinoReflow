#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "menu.h"

/**
 * @brief Global adafruit SSD1306 handle for drawing things on screen
 */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

/**
 * @brief initialize and allocate the SSD1306 screen buffer.
 *
 * Will try to initialize for MAX_TRIES.
 *
 * @return true: successfully allocated buffer
 * @return false: failed to allocate. Probably not enough memory
 */
bool initializeScreen(void)
{
    int tries = 0;
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C) && tries < MAX_TRIES)
    {
        delay(500); // Wait for 500 ms before retrying
        tries++;
    }

    if (tries >= MAX_TRIES)
    {
        Serial.println(F("SSD1306 allocation failed after multiple tries"));
        return false;
    }

    return true;
}

/**
 * @brief Draw an individual MenuItem object at a specified location.
 * Has logic for drawing the object with a border if it is highlighted.
 *
 * @param ptr_menu_item Pointer to the menu item object.
 * @param pos_x X pixel position that the object will be drawn at
 * @param pos_y Y pixel position that the object will be drawn at
 */
void drawMenuItem(MenuItem *ptr_menu_item, uint8_t pos_x, uint8_t pos_y)
{
    if (ptr_menu_item->highlighted)
    {
        display.fillRect(pos_x, pos_y, 63, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    }
    else
    {
        display.setTextColor(SSD1306_WHITE);
    }
    display.setCursor(pos_x, pos_y);
    display.println(ptr_menu_item->text);
}

/**
 * @brief Draws an individual ScreenItem object at a specified location.
 *
 * @param ptr_screen_item pointer to the ScreenItem object
 * @param pos_x X pixel position that the object will be drawn at
 * @param pos_y Y pixel position that the object will be drawn at
 */
void drawScreenItem(ScreenItem *ptr_screen_item, uint8_t pos_x, uint8_t pos_y)
{
    display.setCursor(pos_x, pos_y);
    display.setTextColor(SSD1306_WHITE);
    display.println(ptr_screen_item->text);
}

/**
 * @brief Draw a whole MenuScreen object, including its child ScreenItems and MenuItems
 *
 * @param ptr_menu_screen pointer to the MenuScreen object
 */
void drawMenuScreen(MenuScreen *ptr_menu_screen)
{
    display.clearDisplay();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setFont(NULL);               // Default font

    // Calculate the starting index based on the highlighted menu item
    int startIndex = max(0, ptr_menu_screen->selectedMenuItemIndex - WINDOW_SIZE / 2);
    startIndex = min(startIndex, max(0, ptr_menu_screen->numMenuItems - WINDOW_SIZE));

    // Draw menu items on left side. Shifts based on which item is highlighted.
    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        if (startIndex + i < ptr_menu_screen->numMenuItems)
        {
            drawMenuItem(&(ptr_menu_screen->menuItems[startIndex + i]), 0, i * 8);
        }
    }

    // Draw screen items on the right side. Not selectable.
    for (int i = 0; i < ptr_menu_screen->numScreenItems; i++)
    {

        drawScreenItem(&(ptr_menu_screen->screenItems[i]), 64, i * 8); // Assuming half the screen width for menu items
    }

    display.display();
}

/**
 * @brief Changes or sets the data inside a MenuItem object.
 *
 * @param ptr_menu_item pointer to the menu item
 * @param text character array representing the label of the menu item
 * @param text_length length of the character array. Should not be longer than STR_LEN - 1 (leaving room for the end of string character)
 * @param mode When selected, the MenuItem should activate this mode (int)
 * @param highlighted boolean. True if highlighted. False if not.
 */
void initializeMenuItem(MenuItem *ptr_menu_item, char *text, int text_length, int mode, bool highlighted)
{
    int length;
    char err_str[70];
    if (text_length < 0 || text_length > STR_LEN - 1)
    {
        sprintf(err_str, "Initialize Menu Item Error: text length %d is out of bounds.", text_length);
        Serial.println(err_str);
        length = STR_LEN - 1;
    }
    else
    {
        length = text_length;
    }

    strncpy(ptr_menu_item->text, text, length);
    ptr_menu_item->text[length] = '\0'; // Ensure null termination
    ptr_menu_item->mode = mode;
    ptr_menu_item->highlighted = highlighted;
}

/**
 * @brief Changes or sets the data inside a ScreenItem object.
 *
 * @param ptr_screen_item pointer to the ScreenItem
 * @param text character array representing the label of the menu item
 * @param text_length length of the character array. Should not be longer than STR_LEN - 1 (leaving room for the end of string character)
 */
void initializeScreenItem(ScreenItem *ptr_screen_item, char *text, int text_length)
{

    int length;
    char err_str[70];

    if (text_length < 0 || text_length > STR_LEN - 1)
    {
        sprintf(err_str, "Initialize Menu Item Error: text length %d is out of bounds.", text_length);
        Serial.println(err_str);
        length = STR_LEN - 1;
    }
    else
    {
        length = text_length;
    }
    strncpy(ptr_screen_item->text, text, length);
    ptr_screen_item->text[length] = '\0'; // Ensure null termination
}

/**
 * @brief Changes or sets the data inside a MenuScreen object. Including its nested Menu and Screen Items.
 *
 * @param ptr_menu_screen pointer to the menu screen item
 * @param numMenuItems number of menu items
 * @param menuItems pointer to the list of MenuItems
 * @param selectedMenuItemIndex int representing the index for which menuItem is selected
 * @param numScreenItems number of screen items (maximum is WINDOW_SIZE)
 * @param screenItems pointer to the list of ScreenItems
 */
void initializeMenuScreen(MenuScreen *ptr_menu_screen, int numMenuItems, MenuItem *menuItems, int selectedMenuItemIndex, int numScreenItems, ScreenItem screenItems[])
{
    // max size of screen items is WINDOW_SIZE
    int number_of_screen_items = numScreenItems;

    if (numScreenItems > WINDOW_SIZE)
    {
        number_of_screen_items = WINDOW_SIZE;
    }

    ptr_menu_screen->numMenuItems = numMenuItems;
    ptr_menu_screen->menuItems = menuItems;
    ptr_menu_screen->selectedMenuItemIndex = selectedMenuItemIndex;
    ptr_menu_screen->numScreenItems = number_of_screen_items;

    if (number_of_screen_items > 0)
    {

        for (int i = 0; i < number_of_screen_items && i < WINDOW_SIZE; i++)
        {
            ptr_menu_screen->screenItems[i] = screenItems[i];
        }
    }

    if (numMenuItems > 0)
    {

        for (int i = 0; i < numMenuItems && i < WINDOW_SIZE; i++)
        {
            ptr_menu_screen->menuItems[i] = menuItems[i];
        }
    }
}
