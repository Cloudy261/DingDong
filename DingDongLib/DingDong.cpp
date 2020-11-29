/**
 * @Author: Claudius Laves <claudi>
 * @Date:   11-11-2020  17:39:53
 * @Email:  claudiuslaves@gmx.de
 * @Filename: DingDong.cpp
 * @Last modified by:   claudi
 * @Last modified time: 29-11-2020  18:14:22
 */
#include "DingDong.h"

void DingDong::routine() //main fuction
{
        keep_running = true;
        setup();
        show_on_screen();
        int difficulty = getDifficulty();
        game(difficulty);
        show_off_screen();
        sleep();
}

void DingDong::setup()
{
        /*
           Setup only gets called if the Attiny somehow loses its power source.
           It then sets the GPIOs as IN-/Outputs and reads the saved
           Highscore from the EEPROM.
           Note: This only has to be done once, even though it nearl powers
           everything down in the SLEEP_MODE_PWR_DOWN mode. Attinys hold onto
           their RAM when they are just slightly slightly powered.
         */
        red = 0;
        green = 1;
        yellow = 2;
        button = 3;
        pinMode(red, OUTPUT);
        pinMode(yellow, OUTPUT);
        pinMode(green, OUTPUT);

        pinMode(button, INPUT);

        EEPROM.get(stats_address, stats);

}

boolean DingDong::wait_on_button_Release()
{
        delay(10);                        //Debounce time
        uint32_t timestamp = millis();
        boolean running = true;
        while(digitalRead(button) == HIGH)
        {
                if(millis() - timestamp > BUTTON_TURN_OFF_TIME)
                {
                        running = false;
                        break;
                }
        }
        delay(10);
        return running;

}

boolean DingDong::button_is_pressed()
{
        //returns true if its pressed, false if it isnt. Nothing speial to see here.
        if(digitalRead(button) == HIGH)
        {
                return true;
        }else{
                return false;
        }
}

int DingDong::getDifficulty()
{
        /*
           Interrupt happens here, and the Attiny wakes up. Since The Pin
           Change Interrupt Button is the main Button, we have to wait for
           it to be released before we can start with this functions.
         */
        wait_on_button_Release();
        int difficulty = 1;                   // init difficulty
        uint32_t timestamp = millis();        // set a timestamp for the wait 6s functionality
        set_green();                          // set green in the beginning (difficulty 1)
        while(keep_running && millis() - timestamp < 4000)    // while loop for 6s, timestamp gets reset to millis() on every button press
        {
                if(button_is_pressed())
                {
                        difficulty++;
                        if(difficulty > 6)    //"rolls over" at 7
                        {
                                difficulty = 1;
                        }
                        leds_off(); // reset leds, then look for difficulty
                        switch (difficulty) {
                        case 1: set_green(); break;
                        case 2: set_yellow(); break;
                        case 3: set_red(); break;
                        case 4: set_green();  break;
                        case 5: set_green(); set_yellow(); break;
                        case 6: set_green(); set_yellow(); set_red(); break;
                        }
                        keep_running = wait_on_button_Release();
                        timestamp = millis();
                }
        }

        return difficulty;
}

void DingDong::game(int difficulty)
{
        uint32_t general_timestamp = millis(); // get the timestamp for the 45s general routine
        uint32_t score = 0;

        if(difficulty >= 4) // difficulty 4 = show the Highscore and set difficulty to 3
        {
                difficulty -= 3;
                show_highscore(difficulty);
        }
        if(keep_running)
        {
                set_onoff_times(difficulty); // set the on/off times

                randomSeed(millis()); // random Seed just to add a unique random experience
                uint32_t reactiontime = 0;
                while(keep_running && millis() - general_timestamp < 45000) // main game loop
                {
                        int led_id = get_random_led(); // get led_id to determine which LED should be on

                        leds_off();
                        //all leds off and turn on the matching LED
                        switch (led_id) {
                        case 1: set_green(); break;
                        case 2: set_yellow(); break;
                        case 3: set_red(); break;
                        }
                        uint32_t timestamp = millis();    // 'On/Off TIme timestamp'
                        while(millis() - timestamp < on_time) // chek for buttonpress as long as the On/Off time
                        {
                                if(button_is_pressed())
                                {
                                        if(led_id == 2) // got the yellow one !
                                        {
                                                score++;
                                                reactiontime += (millis() - timestamp);
                                                if(score < 10)
                                                {
                                                        //Times get shorter / Game becomes faster
                                                        on_time -= 10;
                                                        off_time -= 3;
                                                }
                                                leds_off();
                                                set_green();
                                                keep_running = wait_on_button_Release();
                                                if(keep_running)
                                                {
                                                        keep_running = clean_delay(2000);
                                                }
                                                else
                                                {
                                                        break;
                                                }
                                        }
                                        else{       // pressed at wrong led
                                                leds_off();
                                                set_red();
                                                keep_running = wait_on_button_Release();
                                                if(keep_running)
                                                {
                                                        keep_running = clean_delay(3000);
                                                        update_average_reaction_time(reactiontime, score);
                                                        show_score(score, difficulty);
                                                        score = 0;
                                                        reactiontime = 0;
                                                        set_onoff_times(difficulty);
                                                }
                                                else
                                                {
                                                        break;
                                                }
                                        }
                                        general_timestamp = millis(); // renew general_timestamp
                                }
                        }
                        leds_off();
                        timestamp = millis();
                        if(keep_running)
                        {
                                while(millis() - timestamp < off_time)
                                {
                                        if(button_is_pressed())
                                        {
                                                leds_off();
                                                set_red();
                                                keep_running = wait_on_button_Release();
                                                if(keep_running)
                                                {
                                                        keep_running = clean_delay(3000);
                                                        update_average_reaction_time(reactiontime, score);
                                                        show_score(score, difficulty);
                                                        score = 0;
                                                        reactiontime = 0;
                                                        general_timestamp = millis();
                                                        set_onoff_times(difficulty);
                                                }
                                                else
                                                {
                                                        break;
                                                }
                                        }
                                }
                        }

                }
        }
        save_to_EEPROM(score, difficulty);
}

void DingDong::set_onoff_times(unsigned int difficulty)
{
        /*
           Sets on_time and off_time in the beginning of a game.
           This is necessary since the times get changed during the game.
         */
        switch (difficulty) {
        case 1: on_time = 500; off_time = 166; break;
        case 2: on_time = 400; off_time = 133; break;
        case 3: on_time = 350; off_time = 100; break;
        default: on_time = 350; off_time = 100; difficulty = 3; break;
        }
}

void DingDong::save_to_EEPROM(unsigned int score, unsigned int difficulty)
{
        boolean highscore = false;
        switch(difficulty)
        {
        case 1: if(score > stats.highscore_easy) {stats.highscore_easy = score; highscore = true;} break;
        case 2: if(score > stats.highscore_medium) {stats.highscore_medium = score; highscore = true;} break;
        case 3: if(score > stats.highscore_hard) {stats.highscore_hard = score; highscore = true;} break;
        }
        if(highscore)
        {
                EEPROM.put(stats_address, stats);
        }
}

void DingDong::show_score(unsigned int score, unsigned int difficulty)
{
        save_to_EEPROM(score, difficulty);
        int button_is_pressed_counter = 0;
        int score_on_delay = 350;
        int score_off_delay = 300;
        for(unsigned int i = 0; i < score; i++) // blink all leds score-times
        {
                set_green();
                set_yellow();
                set_red();
                delay(score_on_delay);
                leds_off();
                delay(score_off_delay);
                if(button_is_pressed())
                {
                        button_is_pressed_counter++;
                        if(button_is_pressed_counter * (score_on_delay + score_off_delay) > BUTTON_TURN_OFF_TIME)
                        {
                                keep_running = false;
                                break;
                        }
                }
        }
        if(keep_running)
        {
                keep_running = clean_delay(1500);
        }
}

void DingDong::show_highscore(int difficulty)
{
        // i don't think comments are necessary here.
        leds_off();
        delay(400);
        switch(difficulty)
        {
        case 1: show_score(stats.highscore_easy, 1); break;
        case 2: show_score(stats.highscore_medium, 2); break;
        case 3: show_score(stats.highscore_hard, 3); break;
        }
}

int DingDong::get_random_led() // returns 1, 2 or 3 as led_id
{
        int rnd = random(1,200);
        if(rnd <= 80)
        {
                return 1;
        }
        else if (rnd > 80 && rnd < 120)
        {
                return 2;
        }
        else
        {
                return 3;
        }
}

void DingDong::leds_off() // all LEDs off
{
        digitalWrite(red, LOW);
        digitalWrite(yellow, LOW);
        digitalWrite(green, LOW);
}

void DingDong::set_green() // green on
{
        digitalWrite(green, HIGH);
}

void DingDong::set_yellow() // yellow on
{
        digitalWrite(yellow, HIGH);
}

void DingDong::set_red()    // red on
{
        digitalWrite(red, HIGH);
}

void DingDong::dim_led(int led, int dly, boolean up)
{
        if(up)
        {
                for(int i = 0; i < 255; i++)
                {
                        analogWrite(led, i);
                        delay(dly);
                }
                digitalWrite(led, HIGH);
        }else{
                for(int i = 255; i > 0; i--)
                {
                        analogWrite(led, i);
                        delay(dly);
                }
                digitalWrite(led, LOW);
        }
}

void DingDong::resetEEPROM()
{
        /*
           This step is obligatory ONCE. This sets the Highscores to 0
           as i had problems with my highscore being some random number in the
           EEPROM. I want every DingDong to have the Highscore 0 in the beginning.
         */
        stats_struct tmp_stats;
        tmp_stats.counter = 1;
        tmp_stats.average_reaction_time = 200.0f;
        tmp_stats.highscore_easy = 0;
        tmp_stats.highscore_medium = 0;
        tmp_stats.highscore_hard = 0;
        EEPROM.put(stats_address, tmp_stats);


        show_average_reaction_time();
        while(1)
        {
                set_yellow();
                delay(100);
                leds_off();
                delay(100);
                set_yellow();
                delay(100);
                leds_off();
                delay(1000);
        }
}

void DingDong::show_on_screen()
{

/*
   Phil
              dim_led(green, 1, UP);
              delay(250);
              set_yellow();
              delay(500);
              set_red();
              delay(500);
              digitalWrite(red,LOW);
              delay(500);
              digitalWrite(yellow, LOW);
              delay(500);
              dim_led(green, 1, DOWN);
              delay(250);
   JAYB

               set_green();
               delay(100);
               leds_off();
               delay(100);
               set_green();
               delay(100);
               leds_off();
               delay(100);
               set_green();
               delay(100);
               leds_off();
               delay(300);

               set_yellow();
               delay(300);
               leds_off();
               delay(300);
               set_yellow();
               delay(300);
               leds_off();
               delay(300);
               set_yellow();
               delay(300);
               leds_off();
               delay(300);

               set_green();
               delay(100);
               leds_off();
               delay(100);
               set_green();
               delay(100);
               leds_off();
               delay(100);
               set_green();
               delay(100);
               leds_off();
               delay(300);
 */

        //RETRO

        dim_led(green, 1, UP);


}

void DingDong::show_off_screen()
{
        leds_off();
        set_red();
        int easteregg_counter = 0;
        int state = digitalRead(button);
        int old_state = state;
        for(int i = 255; i > 0; i--)
        {
                state = digitalRead(button);
                if(state != old_state)
                {
                        easteregg_counter++;
                }
                old_state = state;
                analogWrite(red, i);
                delay(5);
        }
        leds_off();
        if(easteregg_counter == 7)
        {
                show_average_reaction_time();
        }
        wait_on_button_Release();
}

void DingDong::update_average_reaction_time(uint32_t time, uint32_t score)
{
        float new_time = ((float)(stats.average_reaction_time * stats.counter) + (float)time)/(stats.counter + score);
        stats.average_reaction_time = new_time;
        stats.counter += score;
        EEPROM.put(stats_address, stats);
}

void DingDong::show_average_reaction_time()
{
        uint32_t time = (uint32_t)stats.average_reaction_time;
        uint32_t hundreds = (time - (time % 100));
        uint32_t tens = (time - ((time % 10) + hundreds));
        uint32_t ones = time % 10;
        hundreds /= 100;
        tens /= 10;
        leds_off();
        delay(250);
        for(uint32_t i = 0; i < hundreds; i++)
        {
                set_green();
                delay(300);
                leds_off();
                delay(250);
        }
        delay(1000);
        for(uint32_t i = 0; i < tens; i++)
        {
                set_yellow();
                delay(300);
                leds_off();
                delay(250);
        }
        delay(1000);
        for(uint32_t i = 0; i < ones; i++)
        {
                set_red();
                delay(300);
                leds_off();
                delay(250);
        }
        delay(1000);

}

boolean DingDong::clean_delay(uint32_t ms)
{
        uint32_t timestamp = millis();
        boolean keep_alive = true;
        while(millis() - timestamp < ms)
        {
                if(button_is_pressed())
                {
                        keep_alive = wait_on_button_Release();
                        if(!keep_alive)
                        {
                                break;
                        }
                }
                delay(1);
        }
        return keep_alive;
}

void DingDong::sleep()       // when sleeping, i measured a current of 100nA. This is pretty decent.
{
        leds_off();                         // all LEDs offs
        GIMSK |= _BV(PCIE);                 // Enable Pin Change Interrupts
        PCMSK |= _BV(PCINT3);               // Use PB0 (was PB3) as interrupt pin
        // Turn off unnecessary peripherals
        ADCSRA &= ~_BV(ADEN);               // ADC off
        ACSR |= _BV(ACD); // Disable analog comparator

        set_sleep_mode(SLEEP_MODE_PWR_DOWN); // replaces above statement
        sleep_enable();                     // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
        sei();                              // Enable interrupts
        sleep_cpu();                        // sleep ... Zzzz

        cli();                              // Disable interrupts
        PCMSK &= ~_BV(PCINT3);              // Turn off PB0 (was PB3) as interrupt pin
        sleep_disable();                    // Clear SE bit
        ADCSRA |= _BV(ADEN);
        sei();                              // Enable interrupts

}
