#ifndef __TIMER_H__
#define __TIMER_H__

#include <stm32.h>
#include <gpio.h>
#include <delay.h>
#include <string.h>
#define NUMBER_OF_DIGITS 10
#define NUMBER_OF_POSITIONS 4

void configure_counter_miliseconds();

void configure_timer();

void reset_all();

void update_time(int position);

void refresh();

void set_zeroes();

#endif /* __TIMER_H__ */