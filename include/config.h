#ifndef CONFIG_H
#define CONFIG_H

#ifndef MAX_TRIES
#define MAX_TRIES 10
#endif
#ifndef EEPROM_INITIAL_ADDRESS
#define EEPROM_INITIAL_ADDRESS 0
#endif

/*
Pin definitions
*/
#ifndef FAN_RELAY_PIN
#define FAN_RELAY_PIN 8
#endif
#ifndef HEAT_RELAY_PIN
#define HEAT_RELAY_PIN 9
#endif
#ifndef MS_BETWEEN_PID
#define MS_BETWEEN_PID 500
#endif
#ifndef LB
#define LB 3
#endif
#ifndef UB
#define UB 4
#endif
#ifndef RB
#define RB 6
#endif
#ifndef DB
#define DB 5
#endif
#ifndef SB
#define SB 2
#endif

/*
SPI pins for thermocouple board
*/
#ifndef THERMO_DO
#define THERMO_DO 35
#endif
#ifndef THERMO_CS
#define THERMO_CS 33
#endif
#ifndef THERMO_CLK
#define THERMO_CLK 31
#endif

/*
Reflow profiles Config
*/
#ifndef NUM_REFLOW_PROFILES
#define NUM_REFLOW_PROFILES 10
#endif

#ifndef MAX_TEMP_C
#define MAX_TEMP_C 300
#endif

#ifndef MIN_TEMP_C
#define MIN_TEMP_C 80
#endif

#ifndef MIN_TIME_S
#define MIN_TIME_S 0
#endif

#ifndef MAX_TIME_S
#define MAX_TIME_S (60 * 10)
#endif

#ifndef MIN_TOTAL_RUNTIME_S
#define MIN_TOTAL_RUNTIME_S 30
#endif

/*
Default reflow profile:
*/

#ifndef DEFAULT_FAN_ON
#define DEFAULT_FAN_ON true
#endif

#ifndef DEFAULT_PREHEAT_TEMP_C
#define DEFAULT_PREHEAT_TEMP_C 125
#endif

#ifndef DEFAULT_PREHEAT_TIME_S
#define DEFAULT_PREHEAT_TIME_S 90
#endif

#ifndef DEFAULT_SOAK_TEMP_C
#define DEFAULT_SOAK_TEMP_C 190
#endif

#ifndef DEFAULT_SOAK_TIME_S
#define DEFAULT_SOAK_TIME_S (270 - 90)
#endif

#ifndef DEFAULT_REFLOW_HOLD_TIME_S
#define DEFAULT_REFLOW_HOLD_TIME_S 0
#endif

#ifndef DEFAULT_REFLOW_TEMP
#define DEFAULT_REFLOW_TEMP 225
#endif

/*
Default PID constants
*/
#ifndef DEFAULT_PID_K
#define DEFAULT_PID_K 1.0f
#endif
#ifndef DEFAULT_PID_I
#define DEFAULT_PID_I 0.0f
#endif
#ifndef DEFAULT_PID_D
#define DEFAULT_PID_D 22.0f
#endif
#ifndef DEFAULT_PID_THRESHOLD
#define DEFAULT_PID_THRESHOLD 0.1f
#endif

/*
PID config
*/
#ifndef DEFAULT_CONSTANTS_ADDRESS
#define DEFAULT_CONSTANTS_ADDRESS 0
#endif

#ifndef MIN_CONSTANTS_VALUE
#define MIN_CONSTANTS_VALUE 0.0f
#endif

#ifndef MAX_CONSTANTS_VALUE
#define MAX_CONSTANTS_VALUE 1000.0f
#endif

/*
Modes
*/
#ifndef MODE_HOME
#define MODE_HOME 0 // home menu. up and down to select items. after timeout, goes to -1
#endif

#ifndef MODE_SELECT_PROFILE_TO_RUN
#define MODE_SELECT_PROFILE_TO_RUN 1 // choose a profile to run. back takes you to 0
#endif
#ifndef MODE_PROFILE_SELECTED_TO_RUN
#define MODE_PROFILE_SELECTED_TO_RUN 2 // display graph; go and back button present. go takes you to 3, back takes you to 1
#endif
#ifndef MODE_PROFILE_SELECTED_RUNNING
#define MODE_PROFILE_SELECTED_RUNNING 3 // execute the selected PID profile. cancel button should be present, takes you back to 2. once it's done, it takes you to -1.
#endif

#ifndef MODE_SELECT_PROFILE_TO_EDIT
#define MODE_SELECT_PROFILE_TO_EDIT 4 // up and down to select loaded profiles from eeprom. One option takes you to 5 to make new. selecting a profile takes you to 6
#endif
#ifndef MODE_CREATE_NEW_PROFILE
#define MODE_CREATE_NEW_PROFILE 5 // left and right to select target temps, durations. save button takes you back to 4. cancel button takes you back to 4.
#endif
#ifndef MODE_EDIT_SELECTED_PROFILE
#define MODE_EDIT_SELECTED_PROFILE 6 // left and right to select target temps, durations. save button takes you back to 4. cancel button takes you back to 4.
#endif

#ifndef MODE_HEAT_TO_TARGET
#define MODE_HEAT_TO_TARGET 10 // up and down to select a target temp. shows current temp as well. maybe a graph. no timeout. cancel takes you back to -1.
#endif

#ifndef MODE_EDIT_PID_PRAMS
#define MODE_EDIT_PID_PRAMS 7 // left and right to select param, up and down to mod. save or cancel brings you back to -1
#endif

#ifndef MODE_STATUS
#define MODE_STATUS -1 // hitting select takes you to 0. otherwise it displays the temp inside the oven.
#endif

/*
Screen
*/
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 64
#endif

#ifndef SHIMMY_PERIOD
#define SHIMMY_PERIOD 5000 // the amount of time that passes in between cycling information.
#endif

#endif