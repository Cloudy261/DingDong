/**
 * @Author: Claudius Laves <claudi>
 * @Date:   11-11-2020  17:39:53
 * @Email:  claudiuslaves@gmx.de
 * @Filename: DingDong.cpp
 * @Last modified by:   claudi
 * @Last modified time: 12-11-2020  14:56:42
 */
#include "DingDong.h"


void DingDong::routine()
{
  int difficulty = getDifficulty();
  game(difficulty);
  sleep();
}

void DingDong::setup()
{
  red = 0;
  yellow = 2;
  green = 1;
  button = 3;
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(button, INPUT);

  EEPROM.get(highscore_address, highscore);
}

void DingDong::wait_on_button_Release()
{
  delay(10);
  while(digitalRead(button) == HIGH){}
}

boolean DingDong::button_is_pressed()
{
  if(digitalRead(button) == HIGH)
  {
    return true;
  }else{
    return false;
  }
}

int DingDong::getDifficulty()
{
  delay(10);
  wait_on_button_Release();

  int difficulty = 1;
  uint32_t timestamp = millis();
  set_green();
  while(millis() - timestamp < 6000)
  {
    if(button_is_pressed())
    {
      timestamp = millis();
      difficulty++;
      if(difficulty > 4)
      {
        difficulty = 1;
      }
      leds_off();
      switch (difficulty) {
        case 1: set_green(); break;
        case 2: set_yellow(); break;
        case 3: set_red(); break;
        case 4: set_green(); set_yellow(); set_red(); break;
      }
      wait_on_button_Release();
    }
  }

  return difficulty;
}

void DingDong::game(int difficulty)
{
  uint32_t general_timestamp = millis();
  unsigned int score = 0;

  if(difficulty == 4)
  {
    show_highscore();
    difficulty = 3;
  }

  set_onoff_times(difficulty);

  randomSeed(millis());
  while(millis() - general_timestamp < 45000) // main game loop
  {
    int led_id = get_random_led();

    leds_off();

    switch (led_id) {
      case 1: set_green(); break;
      case 2: set_yellow(); break;
      case 3: set_red(); break;
    }
    uint32_t timestamp = millis();
    while(millis() - timestamp < on_time)
    {
      if(button_is_pressed())
      {
        if(led_id == 2)       // got the yellow one !
        {
          score ++;
          if(score < 10)
          {
            on_time -= 10;
            off_time -= 3;
          }
          leds_off();
          set_green();
          wait_on_button_Release();
          delay(1500);
        }
        else{
          leds_off();
          set_red();
          wait_on_button_Release();
          delay(3000);
          show_score(score, difficulty);
          score = 0;
          set_onoff_times(difficulty);
        }
        general_timestamp = millis();
      }
    }
    leds_off();
    timestamp = millis();
    while(millis() - timestamp < off_time)
    {
      if(button_is_pressed())
      {
        leds_off();
        set_red();
        wait_on_button_Release();
        delay(3000);
        show_score(score, difficulty);
        score = 0;
        general_timestamp = millis();
        set_onoff_times(difficulty);
      }
    }


  }
}

void DingDong::set_onoff_times(unsigned int difficulty)
{
  switch (difficulty) {
    case 1: on_time = 550; off_time = 166; break;
    case 2: on_time = 450; off_time = 133; break;
    case 3: on_time = 350; off_time = 100; break;
    default: on_time = 350; off_time = 100; difficulty = 3; break;
  }
}

void DingDong::show_score(unsigned int score, unsigned int difficulty)
{
  if(difficulty == 3)
  {
    if(score > highscore)
    {
      EEPROM.put(highscore_address, score);
      highscore = score;
    }
  }
  for(unsigned int i = 0; i < score; i ++)
  {
    set_green();
    set_yellow();
    set_red();
    delay(400);
    leds_off();
    delay(300);
  }
  delay(1500);
}

void DingDong::show_highscore()
{
  leds_off();
  delay(400);
  show_score(highscore, 0);
}

int DingDong::get_random_led()
{
  int rnd = random(1,200);
  if(rnd <= 85)
  {
    return 1;
  }
  else if (rnd > 85 && rnd < 115)
  {
    return 2;
  }
  else
  {
    return 3;
  }
}

void DingDong::leds_off()
{
  digitalWrite(red, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);
}

void DingDong::set_green()
{
  digitalWrite(green, HIGH);
}

void DingDong::set_yellow()
{
  digitalWrite(yellow, HIGH);
}

void DingDong::set_red()
{
  digitalWrite(red, HIGH);
}

void DingDong::sleep()
{

    leds_off();
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT3);                   // Use PB0 (was PB3) as interrupt pin
    // Turn off unnecessary peripherals
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    ACSR |= _BV(ACD); // Disable analog comparator

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement
    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep ... Zzzz

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB0 (was PB3) as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);
    sei();                                  // Enable interrupts

}
