/**
 * @Author: Claudius Laves <claudi>
 * @Date:   11-11-2020  17:40:01
 * @Email:  claudiuslaves@gmx.de
 * @Filename: DingDong.h
 * @Last modified by:   claudi
 * @Last modified time: 17-11-2020  13:35:34
 */

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <inttypes.h>
#include <EEPROM.h>

#define BUTTON_TURN_OFF_TIME 4000 //ms

struct reaction_time {
        float time;
        uint32_t counter;
        uint32_t reaction_address{200};
        uint32_t counter_address{300};
};

class DingDong
{
private:

int red, green, yellow, button;

int highscore_address{100};
unsigned int highscore{0};



reaction_time reaction;

uint32_t on_time, off_time;

boolean keep_running{true};

void leds_off();
boolean wait_on_button_Release();

void set_red();
void set_yellow();
void set_green();

boolean button_is_pressed();
void show_highscore();
int getDifficulty();
void game(int diff);
int get_random_led();

void show_score(unsigned int score, unsigned int difficulty);
void set_onoff_times(unsigned int diff);

void show_on_screen();
void show_off_screen();

void update_average_reaction_time(uint32_t time, unsigned int score);
void show_average_reaction_time();
void sleep();

public:

void setup();
void routine();
void resetEEPROM();

};
