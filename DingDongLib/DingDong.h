/**
 * @Author: Claudius Laves <claudi>
 * @Date:   11-11-2020  17:40:01
 * @Email:  claudiuslaves@gmx.de
 * @Filename: DingDong.h
 * @Last modified by:   claudi
 * @Last modified time: 03-12-2020  12:50:30
 */

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <inttypes.h>
#include <EEPROM.h>

#define BUTTON_TURN_OFF_TIME 3000 //ms
#define UP 1
#define DOWN 0

struct stats_struct {
        float average_reaction_time;
        uint32_t counter;
        unsigned int highscore_easy;
        unsigned int highscore_medium;
        unsigned int highscore_hard;
};

class DingDong
{
private:

int red, green, yellow, button;

stats_struct stats;
unsigned int stats_address{0};

uint32_t on_time, off_time;

boolean keep_running{true};

void leds_off();
boolean wait_on_button_Release();

void set_red();
void set_yellow();
void set_green();
void dim_led(int led, int dly, boolean up);
boolean button_is_pressed();
void show_highscore(int difficulty);
int getDifficulty();
void game(int diff);
int get_random_led();

void show_score(unsigned int score, unsigned int difficulty);
void set_onoff_times(unsigned int diff);
void save_to_EEPROM(unsigned int score, unsigned int difficulty);

void show_on_screen();
void show_off_screen();

void update_average_reaction_time(uint32_t time, uint32_t score);
void show_average_reaction_time();
void sleep();
boolean clean_delay(uint32_t ms);

public:

void setup();
void routine();
void resetEEPROM();

};
