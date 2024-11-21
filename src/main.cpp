#include <Arduino.h>
#include "config.h"
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include "reflow.h"
#include "PID.h"
#include "temperature.h"
#include "time.h"
#include "buttons.h"
#include "menu.h"
#include "screens.h"

// global variables

/**
 * @brief Array to hold all the reflow profiles acquired from EEPROM
 */
ReflowProfile reflow_profiles[NUM_REFLOW_PROFILES];

/**
 * @brief object to hold all the PID settings and constants
 */
PID pid;

/**
 * @brief Buffer to hold temporary string values. Mostly just for the ScreenItem
 * and MenuItem initialization functions. Use sprintf(reusableBuffer, "string %d", data) to fill this.
 */
char reusableBuffer[10];

/**
 * @brief int representing the current mode. Used for the switch statement to check which menu we should be displaying
 */
int current_mode = MODE_STATUS;

/**
 * @brief double container to hold the temperature in celsius gotten from the SPI thermocouple amplifier
 */
double current_temp;

/**
 * @brief container to hold currently selected profile
 */
ReflowProfile currentlySelectedProfile;

/**
 * @brief container to hold in-edit profile
 */
ReflowProfile editingProfile;

uint32_t hold_reflow_time;
bool hold_reflow_time_started = false;
double incrementor = 1.0f;
uint8_t profile_index = 0;

// Main code----------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);

  // Set pin direcitons
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(HEAT_RELAY_PIN, OUTPUT);
  // buttons Normally open. when pressed, they will read LOW
  pinMode(LB, INPUT_PULLUP);
  pinMode(UB, INPUT_PULLUP);
  pinMode(DB, INPUT_PULLUP);
  pinMode(RB, INPUT_PULLUP);
  pinMode(SB, INPUT_PULLUP);

  // try to initialize the RTC.
  if (!initializeRTC())
  {
    Serial.println(F("Failed to start program: no RTC found"));
    while (1)
    {
      delay(100);
    }
  }

  // check eeprom for PID constants
  get_PID_constants_from_EEPROM(DEFAULT_CONSTANTS_ADDRESS, &(pid.constants));
  // verify PID constants
  if (is_PID_constants_valid(&(pid.constants)))
  {
    // assign result to pid variable (already done by using get above)
  }
  else
  {
    // if the PID constants are invalid, write the default constants to memory and set current volatile variables to defaults
    pid.constants.PID_d = DEFAULT_PID_D;
    pid.constants.PID_i = DEFAULT_PID_I;
    pid.constants.PID_k = DEFAULT_PID_K;
    pid.constants.threshold = DEFAULT_PID_THRESHOLD;

    save_PID_constants_to_EEPROM(DEFAULT_CONSTANTS_ADDRESS, &(pid.constants));
  }

  pid.target = 0.0f;
  pid.integral = 0.0f;

  // Initializes the default reflow profile object
  initialize_default_profile();

  // initialize adafruit temp sensor
  if (!initializeTemperature())
  {
    Serial.println("ERROR, failed to initialize Adafruit_MAX31855 sensor.");
  }

  // get current temperature
  getTemperature(&current_temp);

  // get the currnet reflow profiles from EEPROM
  getProfilesFromEEPROM(reflow_profiles, NUM_REFLOW_PROFILES, DEFAULT_PROFILES_ADDRESS);

  // verify the current reflow profiles
  bool invalidProfilePresent = false;
  for (int i = 0; i < NUM_REFLOW_PROFILES; i++)
  {
    if (!isProfileValid(&(reflow_profiles[i])))
    {
      invalidProfilePresent = true;
      reflow_profiles[i] = default_reflow_profile;
    }
  }
  if (invalidProfilePresent)
  {
    saveProfilesToEEPROM(reflow_profiles, NUM_REFLOW_PROFILES, DEFAULT_PROFILES_ADDRESS);
  }
  if (!initializeScreen())
  {
    Serial.println(F("Failed to start program: couldn't allocate SSD1306 Buffer. Not enough memory."));
    while (1)
    {
      delay(100);
    }
  }

  initializeAllScreens();

  getTimeNow(&time_s);

  setPreviousTime();
  setPreviousMillis();

  // initialize PID object temperature history
  pid.historyIndex = 0;
  for (int i = 0; i < 5; i++)
  {
    pid.temperatureHistory[i] = current_temp;
  }

  // display status screen
  current_mode = MODE_STATUS;
}

void loop()
{
  // check buttons, assign states
  checkButtonStates(&left_button_pressed, &right_button_pressed, &down_button_pressed, &up_button_pressed, &select_button_pressed);

  // check what mode we're in. run the case for that mode.
  switch (current_mode)
  {
  case MODE_STATUS:
    flag_PID_running = false;
    pid.target = 0.0f;

    // show status screen, if select is pressed, sets current_mode to home
    if (select_button_pressed)
    {
      // Serial.println(F("Select button pressed!"));
      current_mode = MODE_HOME;
      index_to_highlight = 0;
      break;
    }

    // draw status screen
    dtostrf(current_temp, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");                 // Append the "C" for Celsius
    initializeScreenItem(&(status_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer));
    dtostrf(pid.target, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");               // Append the "C" for Celsius
    initializeScreenItem(&(status_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

    drawMenuScreen(&status_screen);

    break;
  case MODE_HOME:
    flag_PID_running = false;
    pid.target = 0.0f;

    set_item_to_highlight(&home_screen, index_to_highlight);
    // show home screen. up and down to select what mode to go to next
    // check what mode is highlighted. if select is pressed, change current_mode to that
    if (select_button_pressed)
    {
      current_mode = home_screen.menuItems[index_to_highlight].mode;
      index_to_highlight = 0;
      break;
    }
    else if (down_button_pressed)
    {
      increment_highlight(&home_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&home_screen);
    }

    // draw status screen
    dtostrf(current_temp, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");                 // Append the "C" for Celsius
    initializeScreenItem(&(home_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer));
    dtostrf(pid.target, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");               // Append the "C" for Celsius
    initializeScreenItem(&(home_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

    drawMenuScreen(&home_screen);

    break;
  case MODE_SELECT_PROFILE_TO_RUN:
    flag_PID_running = false;
    pid.target = 0.0f;

    set_item_to_highlight(&run_reflow_screen, index_to_highlight);

    if (select_button_pressed)
    {
      current_mode = run_reflow_screen.menuItems[index_to_highlight].mode;
      if (index_to_highlight > 0)
      {
        // put selected profile into selected profile variable
        currentlySelectedProfile = reflow_profiles[index_to_highlight - 1];
        profile_index = index_to_highlight - 1;
      }

      index_to_highlight = 0;

      break;
    }
    else if (down_button_pressed)
    {
      increment_highlight(&run_reflow_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&run_reflow_screen);
    }

    if (index_to_highlight > 0)
    {
      // populate the respective profile data into screen items. get from reflow_profiles[].

      // only room for 8 screen items, but there's 14 items to show, so just kinda shimmy that thang back and forth
      if ((millis() / SHIMMY_PERIOD) % 2 == 0)
      {
        // first 8 items
        initializeScreenItem(&(run_reflow_screen.screenItems[0]), "Pre Temp", 8);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].preheat_temp_c);
        initializeScreenItem(&(run_reflow_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer)); // from reflow_profiles[index_to_highlight-1].preheat_temp_c

        initializeScreenItem(&(run_reflow_screen.screenItems[2]), "Pre Time", 8);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].preheat_time_s);
        initializeScreenItem(&(run_reflow_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(run_reflow_screen.screenItems[4]), "Soak Temp", 9);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].soak_temp_c);
        initializeScreenItem(&(run_reflow_screen.screenItems[5]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(run_reflow_screen.screenItems[6]), "Soak Time", 9);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].soak_time_s);
        initializeScreenItem(&(run_reflow_screen.screenItems[7]), reusableBuffer, strlen(reusableBuffer));
      }
      else
      {
        // last 5 items
        initializeScreenItem(&(run_reflow_screen.screenItems[0]), "Rfl Temp", 8);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].reflow_temp);
        initializeScreenItem(&(run_reflow_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(run_reflow_screen.screenItems[2]), "Rfl Time", 8);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].reflow_hold_time_s);
        initializeScreenItem(&(run_reflow_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

        if (reflow_profiles[index_to_highlight - 1].fan_on)
        {
          initializeScreenItem(&(run_reflow_screen.screenItems[4]), "Fan: On", 7);
        }
        else
        {
          initializeScreenItem(&(run_reflow_screen.screenItems[4]), "Fan: Off", 8);
        }

        initializeScreenItem(&(run_reflow_screen.screenItems[5]), " ", 1);
        initializeScreenItem(&(run_reflow_screen.screenItems[6]), " ", 1);
        initializeScreenItem(&(run_reflow_screen.screenItems[7]), " ", 1);
      }
    }
    else
    {
      // all screen items are blank
      for (int i = 0; i < run_reflow_screen.numScreenItems; i++)
      {
        initializeScreenItem(&(run_reflow_screen.screenItems[i]), " ", 1);
      }
    }

    // draw the screen after all the rigamarole that happened above
    drawMenuScreen(&run_reflow_screen);
    break;
  case MODE_PROFILE_SELECTED_TO_RUN:
    flag_PID_running = false;
    pid.target = 0.0f;

    set_item_to_highlight(&selected_to_run_screen, index_to_highlight);

    if (select_button_pressed)
    {
      current_mode = selected_to_run_screen.menuItems[index_to_highlight].mode;
      if (current_mode == MODE_PROFILE_SELECTED_RUNNING)
      {
        setPreviousTime();
      }

      index_to_highlight = 0;
      break;
    }
    else if (down_button_pressed)
    {
      increment_highlight(&selected_to_run_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&selected_to_run_screen);
    }

    if ((millis() / SHIMMY_PERIOD) % 2 == 0)
    {
      // first 8 items
      initializeScreenItem(&(selected_to_run_screen.screenItems[0]), "Pre Temp", 8);

      sprintf(reusableBuffer, "%dC", currentlySelectedProfile.preheat_temp_c);
      initializeScreenItem(&(selected_to_run_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer)); // from reflow_profiles[index_to_highlight-1].preheat_temp_c

      initializeScreenItem(&(selected_to_run_screen.screenItems[2]), "Pre Time", 8);

      sprintf(reusableBuffer, "%ds", currentlySelectedProfile.preheat_time_s);
      initializeScreenItem(&(selected_to_run_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

      initializeScreenItem(&(selected_to_run_screen.screenItems[4]), "Soak Temp", 9);

      sprintf(reusableBuffer, "%dC", currentlySelectedProfile.soak_temp_c);
      initializeScreenItem(&(selected_to_run_screen.screenItems[5]), reusableBuffer, strlen(reusableBuffer));

      initializeScreenItem(&(selected_to_run_screen.screenItems[6]), "Soak Time", 9);

      sprintf(reusableBuffer, "%ds", currentlySelectedProfile.soak_time_s);
      initializeScreenItem(&(selected_to_run_screen.screenItems[7]), reusableBuffer, strlen(reusableBuffer));
    }
    else
    {
      // last 5 items
      initializeScreenItem(&(selected_to_run_screen.screenItems[0]), "Rfl Temp", 8);

      sprintf(reusableBuffer, "%dC", currentlySelectedProfile.reflow_temp);
      initializeScreenItem(&(selected_to_run_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer));

      initializeScreenItem(&(selected_to_run_screen.screenItems[2]), "Rfl Time", 8);

      sprintf(reusableBuffer, "%ds", currentlySelectedProfile.reflow_hold_time_s);
      initializeScreenItem(&(selected_to_run_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

      if (currentlySelectedProfile.fan_on)
      {
        initializeScreenItem(&(selected_to_run_screen.screenItems[4]), "Fan: On", 7);
      }
      else
      {
        initializeScreenItem(&(selected_to_run_screen.screenItems[4]), "Fan: Off", 8);
      }

      initializeScreenItem(&(selected_to_run_screen.screenItems[5]), " ", 1);
      initializeScreenItem(&(selected_to_run_screen.screenItems[6]), " ", 1);
      initializeScreenItem(&(selected_to_run_screen.screenItems[7]), " ", 1);
    }
    drawMenuScreen(&selected_to_run_screen);
    break;
  case MODE_PROFILE_SELECTED_RUNNING:

    if (select_button_pressed)
    {
      current_mode = MODE_PROFILE_SELECTED_TO_RUN;
      index_to_highlight = 0;
      flag_PID_running = false;
      pid.target = 0;
      digitalWrite(FAN_RELAY_PIN, LOW);
      digitalWrite(HEAT_RELAY_PIN, LOW);
      hold_reflow_time_started = false;
      break;
    }

    flag_PID_running = true;

    // profile logic
    getTimeNow(&time_s);
    // fail or refuse to run this if the get temperature method returns false.
    if (current_temp < 5)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK); // Draw white text
      display.setFont(NULL);
      display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
      display.setCursor(22, 16);
      display.println("NO TEMP");
      display.setCursor(22, 32);
      display.println("READING");
      pid.target = 0.0f;
      flag_PID_running = false;
      digitalWrite(FAN_RELAY_PIN, LOW);
      digitalWrite(HEAT_RELAY_PIN, LOW);
      display.display();

      break;
    }

    if (time_s - previous_time <= currentlySelectedProfile.preheat_time_s)
    {
      // preheat logic

      pid.target = currentlySelectedProfile.preheat_temp_c;
    }
    else if (time_s - previous_time > currentlySelectedProfile.preheat_time_s && time_s - previous_time <= currentlySelectedProfile.soak_time_s + currentlySelectedProfile.preheat_time_s)
    {
      // soak logic

      pid.target = currentlySelectedProfile.soak_temp_c;
    }
    else if (time_s - previous_time > currentlySelectedProfile.soak_time_s + currentlySelectedProfile.preheat_time_s)
    {

      pid.target = currentlySelectedProfile.reflow_temp;
      // reflow logic (+ hold time)
      // wait for current temp to reach reflow. then start counting reflow hold time
      if (current_temp < currentlySelectedProfile.reflow_temp)
      {
        // do nothing; wait when temp reaches goal
      }
      else
      {
        // once temp reaches goal. start counting hold time

        if (!hold_reflow_time_started)
        {
          hold_reflow_time = time_s + currentlySelectedProfile.reflow_hold_time_s;
          hold_reflow_time_started = true;
        }

        if (time_s > hold_reflow_time)
        {
          pid.target = 0.0f;
          flag_PID_running = false;

          // break, go to select profile
          current_mode = MODE_SELECT_PROFILE_TO_RUN;
          index_to_highlight = 0;
          break;
        }
      }
    }

    if (currentlySelectedProfile.fan_on && flag_PID_running == true)
    {
      digitalWrite(FAN_RELAY_PIN, HIGH);
    }
    else
    {
      digitalWrite(FAN_RELAY_PIN, LOW);
    }
    // drawing/screen stuff
    display.clearDisplay();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setFont(NULL);               // Default font
    if (flag_PID_running)
    {
      // display target temperature, current temperature, and time passed.
      display.setCursor(0, 0);
      display.println(F("Temp:"));
      display.setCursor(64, 0);
      display.println(F("Target:"));
      display.setCursor(0, 8);
      dtostrf(current_temp, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
      strcat(reusableBuffer, "C");                 // Append the "C" for Celsius
      display.println(reusableBuffer);
      display.setCursor(64, 8);
      dtostrf(pid.target, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
      strcat(reusableBuffer, "C");               // Append the "C" for Celsius
      display.println(reusableBuffer);
      display.setCursor(28, 24);
      display.println("Time Passed:");
      display.setCursor(62, 32);
      sprintf(reusableBuffer, "%ds", (int)(time_s - previous_time));
      display.println(reusableBuffer);
      display.display();
    }
    else
    {
      // if the PID loop is done, then just say done (probably won't show)
      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK); // Draw white text
      display.setFont(NULL);
      display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
      display.setCursor(34, 24);
      display.println("Done!");
      display.display();
    }

    break;
  case MODE_SELECT_PROFILE_TO_EDIT:
    flag_PID_running = false;
    pid.target = 0.0f;

    set_item_to_highlight(&select_profile_to_edit_screen, index_to_highlight);

    if (select_button_pressed)
    {
      current_mode = select_profile_to_edit_screen.menuItems[index_to_highlight].mode;
      if (index_to_highlight > 0)
      {
        // put selected profile into selected profile variable
        currentlySelectedProfile = reflow_profiles[index_to_highlight - 1];
        profile_index = index_to_highlight - 1;
      }

      index_to_highlight = 0;

      break;
    }
    else if (down_button_pressed)
    {
      increment_highlight(&select_profile_to_edit_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&select_profile_to_edit_screen);
    }

    if (index_to_highlight > 0)
    {
      // populate the respective profile data into screen items. get from reflow_profiles[].

      // only room for 8 screen items, but there's 14 items to show, so just kinda shimmy that thang back and forth
      if ((millis() / SHIMMY_PERIOD) % 2 == 0)
      {
        // first 8 items
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[0]), "Pre Temp", 8);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].preheat_temp_c);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer)); // from reflow_profiles[index_to_highlight-1].preheat_temp_c

        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[2]), "Pre Time", 8);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].preheat_time_s);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[4]), "Soak Temp", 9);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].soak_temp_c);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[5]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[6]), "Soak Time", 9);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].soak_time_s);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[7]), reusableBuffer, strlen(reusableBuffer));
      }
      else
      {
        // last 5 items
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[0]), "Rfl Temp", 8);

        sprintf(reusableBuffer, "%dC", reflow_profiles[index_to_highlight - 1].reflow_temp);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[1]), reusableBuffer, strlen(reusableBuffer));

        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[2]), "Rfl Time", 8);

        sprintf(reusableBuffer, "%ds", reflow_profiles[index_to_highlight - 1].reflow_hold_time_s);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[3]), reusableBuffer, strlen(reusableBuffer));

        if (reflow_profiles[index_to_highlight - 1].fan_on)
        {
          initializeScreenItem(&(select_profile_to_edit_screen.screenItems[4]), "Fan: On", 7);
        }
        else
        {
          initializeScreenItem(&(select_profile_to_edit_screen.screenItems[4]), "Fan: Off", 8);
        }

        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[5]), " ", 1);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[6]), " ", 1);
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[7]), " ", 1);
      }
    }
    else
    {
      // all screen items are blank
      for (int i = 0; i < select_profile_to_edit_screen.numScreenItems; i++)
      {
        initializeScreenItem(&(select_profile_to_edit_screen.screenItems[i]), " ", 1);
      }
    }

    // draw the screen after all the rigamarole that happened above
    drawMenuScreen(&select_profile_to_edit_screen);
    break;
  case MODE_EDIT_SELECTED_PROFILE:
    set_item_to_highlight(&edit_reflow_screen, index_to_highlight);
    if (select_button_pressed && index_to_highlight == 0)
    {
      current_mode = edit_reflow_screen.menuItems[index_to_highlight].mode;
      getProfilesFromEEPROM(reflow_profiles, NUM_REFLOW_PROFILES, DEFAULT_PROFILES_ADDRESS);
      currentlySelectedProfile = reflow_profiles[profile_index];
      index_to_highlight = 0;
      break;
    }
    else if (select_button_pressed && index_to_highlight == 8)
    {
      // save currently selected profile to EEPROM (keep track of which index it's in since EEPROM has X profiles)
      reflow_profiles[profile_index] = currentlySelectedProfile;
      saveProfilesToEEPROM(reflow_profiles, NUM_REFLOW_PROFILES, DEFAULT_PROFILES_ADDRESS);
    }
    else if (right_button_pressed)
    {
      switch (index_to_highlight)
      {
      case 1:
        currentlySelectedProfile.preheat_temp_c = currentlySelectedProfile.preheat_temp_c + 1;
        break;
      case 2:
        currentlySelectedProfile.preheat_time_s = currentlySelectedProfile.preheat_time_s + 1;
        break;
      case 3:
        currentlySelectedProfile.soak_temp_c = currentlySelectedProfile.soak_temp_c + 1;
        break;
      case 4:
        currentlySelectedProfile.soak_time_s = currentlySelectedProfile.soak_time_s + 1;
        break;
      case 5:
        currentlySelectedProfile.reflow_temp = currentlySelectedProfile.reflow_temp + 1;
        break;
      case 6:
        currentlySelectedProfile.reflow_hold_time_s = currentlySelectedProfile.reflow_hold_time_s + 1;
        break;
      case 7:
        currentlySelectedProfile.fan_on = !currentlySelectedProfile.fan_on;
        break;
      }
    }
    else if (left_button_pressed)
    {
      switch (index_to_highlight)
      {
      case 1:
        currentlySelectedProfile.preheat_temp_c = currentlySelectedProfile.preheat_temp_c - 1;
        break;
      case 2:
        currentlySelectedProfile.preheat_time_s = currentlySelectedProfile.preheat_time_s - 1;
        break;
      case 3:
        currentlySelectedProfile.soak_temp_c = currentlySelectedProfile.soak_temp_c - 1;
        break;
      case 4:
        currentlySelectedProfile.soak_time_s = currentlySelectedProfile.soak_time_s - 1;
        break;
      case 5:
        currentlySelectedProfile.reflow_temp = currentlySelectedProfile.reflow_temp - 1;
        break;
      case 6:
        currentlySelectedProfile.reflow_hold_time_s = currentlySelectedProfile.reflow_hold_time_s - 1;
        break;
      case 7:
        currentlySelectedProfile.fan_on = !currentlySelectedProfile.fan_on;
        break;
      }
    }
    else if (down_button_pressed)
    {
      increment_highlight(&edit_reflow_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&edit_reflow_screen);
    }

    switch (index_to_highlight)
    {
    case 0:
      // cancel / back
      initializeScreenItem(&(edit_reflow_screen.screenItems[0]), " ", 1);
      break;
    case 1:
      // preheat temp
      dtostrf(currentlySelectedProfile.preheat_temp_c, 1, 1, reusableBuffer);
      break;
    case 2:
      // preheat time
      dtostrf(currentlySelectedProfile.preheat_time_s, 1, 1, reusableBuffer);
      break;
    case 3:
      // soak temp
      dtostrf(currentlySelectedProfile.soak_temp_c, 1, 1, reusableBuffer);
      break;
    case 4:
      // soak time
      dtostrf(currentlySelectedProfile.soak_time_s, 1, 1, reusableBuffer);
      break;
    case 5:
      // reflow temp
      dtostrf(currentlySelectedProfile.reflow_temp, 1, 1, reusableBuffer);
      break;
    case 6:
      // reflow hold time
      dtostrf(currentlySelectedProfile.reflow_hold_time_s, 1, 1, reusableBuffer);
      break;
    case 7:
      // fan on/off
      if (currentlySelectedProfile.fan_on)
      {
        strcpy(reusableBuffer, "On");
      }
      else
      {
        strcpy(reusableBuffer, "Off");
      }
      break;
    case 8:
      // save to eeprom
      initializeScreenItem(&(edit_reflow_screen.screenItems[0]), " ", 1);
      break;
    }

    initializeScreenItem(&(edit_reflow_screen.screenItems[0]), reusableBuffer, strlen(reusableBuffer));
    drawMenuScreen(&edit_reflow_screen);

    break;
  case MODE_EDIT_PID_PRAMS:
    flag_PID_running = false;
    pid.target = 0.0f;

    set_item_to_highlight(&edit_pid_screen, index_to_highlight);

    if (select_button_pressed)
    {
      current_mode = edit_pid_screen.menuItems[index_to_highlight].mode;
      if (current_mode == MODE_EDIT_PID_PRAMS && index_to_highlight < 6)
      {
        // nothing
      }
      else if (current_mode == MODE_EDIT_PID_PRAMS && index_to_highlight == 6)
      {
        // save current constants to EEPROM
        save_PID_constants_to_EEPROM(DEFAULT_CONSTANTS_ADDRESS, &(pid.constants));
      }
      else
      {
        index_to_highlight = 0;
        break;
      }
    }
    else if (down_button_pressed)
    {
      increment_highlight(&edit_pid_screen);
    }
    else if (up_button_pressed)
    {
      decrement_highlight(&edit_pid_screen);
    }
    else if (right_button_pressed)
    {
      // increment currently highlighted PID constant if a PID constant is highlighted.
      switch (index_to_highlight)
      {
      case 1:
        pid.constants.PID_k += incrementor;
        break;
      case 2:
        pid.constants.PID_i += incrementor;
        break;
      case 3:
        pid.constants.PID_d += incrementor;
        break;
      case 4:
        pid.constants.threshold += incrementor;
        break;
      case 5:
        incrementor += 1.0f;
        break;
      }
    }
    else if (left_button_pressed)
    {
      // decrement currently highlighted PID constant if a PID constant is highlighted.
      switch (index_to_highlight)
      {
      case 1:
        pid.constants.PID_k -= incrementor;
        break;
      case 2:
        pid.constants.PID_i -= incrementor;
        break;
      case 3:
        pid.constants.PID_d -= incrementor;
        break;
      case 4:
        pid.constants.threshold -= incrementor;
        break;
      case 5:
        incrementor -= 0.1f;
        break;
      }
    }

    // depending on which menu item is selected, display the respective value
    switch (index_to_highlight)
    {
    case 1:
      dtostrf(pid.constants.PID_k, 1, 1, reusableBuffer);
      break;
    case 2:
      dtostrf(pid.constants.PID_i, 1, 1, reusableBuffer);
      break;
    case 3:
      dtostrf(pid.constants.PID_d, 1, 1, reusableBuffer);
      break;
    case 4:
      dtostrf(pid.constants.threshold, 1, 1, reusableBuffer);
      break;
    case 5:
      dtostrf(incrementor, 1, 1, reusableBuffer);
      break;
    default:
      strcpy(reusableBuffer, " ");
      break;
    }

    initializeScreenItem(&(edit_pid_screen.screenItems[0]), reusableBuffer, strlen(reusableBuffer));

    drawMenuScreen(&edit_pid_screen);

    break;
  case MODE_HEAT_TO_TARGET:
    flag_PID_running = true;

    if (select_button_pressed)
    {
      current_mode = MODE_HOME;
      index_to_highlight = 0;
      flag_PID_running = false;
      pid.target = 0;
      digitalWrite(FAN_RELAY_PIN, LOW);
      digitalWrite(HEAT_RELAY_PIN, LOW);

      break;
    }
    else if (up_button_pressed)
    {
      pid.target += 10.0f;
      if (pid.target > MAX_TEMP_C)
      {
        pid.target = MAX_TEMP_C;
      }
    }
    else if (down_button_pressed)
    {
      pid.target -= 10.0f;
      if (pid.target < MIN_TEMP_C)
      {
        pid.target = MIN_TEMP_C;
      }
    }
    else if (right_button_pressed)
    {
      digitalWrite(FAN_RELAY_PIN, HIGH);
    }
    else if (left_button_pressed)
    {
      digitalWrite(FAN_RELAY_PIN, LOW);
    }

    if (current_temp < 5)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK); // Draw white text
      display.setFont(NULL);
      display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
      display.setCursor(22, 16);
      display.println("NO TEMP");
      display.setCursor(22, 32);
      display.println("READING");
      pid.target = 0.0f;
      flag_PID_running = false;
      digitalWrite(FAN_RELAY_PIN, LOW);
      digitalWrite(HEAT_RELAY_PIN, LOW);
      display.display();

      break;
    }

    // display target temperature, current temperature.
    display.clearDisplay();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.println(F("Temp:"));
    display.setCursor(64, 0);
    display.println(F("Target:"));
    display.setCursor(0, 8);
    dtostrf(current_temp, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");                 // Append the "C" for Celsius
    display.println(reusableBuffer);
    display.setCursor(64, 8);
    dtostrf(pid.target, 1, 1, reusableBuffer); // 1 is min width, 1 is precision
    strcat(reusableBuffer, "C");               // Append the "C" for Celsius
    display.println(reusableBuffer);
    display.display();

    break;
  }

  // if PID is running, check if MS_BETWEEN_PIDms has passed since last check. if no, then just skip the PID calc
  // if  it has been MS_BETWEEN_PID, then evaluate the PID loop and set previous millis to now
  getTimeNow(&time_s);

  if (current_temp >= 5)
  {
    pid.input = current_temp;

    if (flag_PID_running && ((millis() - previousMillis) >= MS_BETWEEN_PID))
    {

      pid.dt = (double)((double)millis() - (double)previousMillis) / 1000.0f;
      if (calculatePID(&pid))
      {
        // turn heater on
        digitalWrite(HEAT_RELAY_PIN, HIGH);
      }
      else
      {
        // turn heater off
        digitalWrite(HEAT_RELAY_PIN, LOW);
      }
      setPreviousMillis();
    }
    else if (!flag_PID_running)
    {
      digitalWrite(HEAT_RELAY_PIN, LOW);
    }
  }
  else if (((time_s / 5) % 2 == 0) && flag_PID_running)
  {
    Serial.println("Can't run PID, no temperature reading!");
    digitalWrite(HEAT_RELAY_PIN, LOW);
  }
  else
  {
    digitalWrite(HEAT_RELAY_PIN, LOW);
  }

  getTemperature(&current_temp);
}
