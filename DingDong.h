/**
 * @Author: Claudius Laves <claudi>
 * @Date:   11-11-2020  17:40:01
 * @Email:  claudiuslaves@gmx.de
 * @Filename: DingDong.h
 * @Last modified by:   claudi
 * @Last modified time: 13-11-2020  10:09:04
 */

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <inttypes.h>
#include <EEPROM.h>


class DingDong
{
private:

int red, green, yellow, button;

int highscore_address{100};
unsigned int highscore{0};

uint32_t on_time, off_time;

void leds_off();
void wait_on_button_Release();
void set_red();
void set_yellow();
void set_green();
boolean button_is_pressed();
void show_highscore();
int getDifficulty();
void game(int diff);
void sleep();
int get_random_led();
void show_score(unsigned int score, unsigned int difficulty);
void set_onoff_times(unsigned int diff);

public:

void setup();
void routine();
void resetEEPROM();

};
