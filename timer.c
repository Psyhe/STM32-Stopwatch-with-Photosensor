#include <stm32.h>
#include <gpio.h>
#include <delay.h>
#include <string.h>

#include "timer.h"

#define PIN_OFFSET 16

#define ANODE_1_GPIO GPIOB
#define ANODE_2_GPIO GPIOA
#define ANODE_3_GPIO GPIOA
#define ANODE_4_GPIO GPIOA

#define ANODE_1_PIN 0
#define ANODE_2_PIN 4
#define ANODE_3_PIN 1
#define ANODE_4_PIN 0

#define CATHODE_A_GPIO GPIOB
#define CATHODE_B_GPIO GPIOC
#define CATHODE_C_GPIO GPIOA
#define CATHODE_D_GPIO GPIOA
#define CATHODE_E_GPIO GPIOB
#define CATHODE_F_GPIO GPIOB
#define CATHODE_G_GPIO GPIOB
#define CATHODE_DP_GPIO GPIOB

#define CATHODE_A_PIN 6
#define CATHODE_B_PIN 7
#define CATHODE_C_PIN 9
#define CATHODE_D_PIN 8
#define CATHODE_E_PIN 10
#define CATHODE_F_PIN 4
#define CATHODE_G_PIN 5
#define CATHODE_DP_PIN 3

#define ANODE_1_ON() ANODE_1_GPIO->BSRR = 1 << (ANODE_1_PIN + PIN_OFFSET)
#define ANODE_2_ON() ANODE_2_GPIO->BSRR = 1 << (ANODE_2_PIN + PIN_OFFSET)
#define ANODE_3_ON() ANODE_3_GPIO->BSRR = 1 << (ANODE_3_PIN + PIN_OFFSET)
#define ANODE_4_ON() ANODE_4_GPIO->BSRR = 1 << (ANODE_4_PIN + PIN_OFFSET)

#define CATHODE_A_ON() CATHODE_A_GPIO->BSRR = 1 << (CATHODE_A_PIN + PIN_OFFSET)
#define CATHODE_B_ON() CATHODE_B_GPIO->BSRR = 1 << (CATHODE_B_PIN + PIN_OFFSET)
#define CATHODE_C_ON() CATHODE_C_GPIO->BSRR = 1 << (CATHODE_C_PIN + PIN_OFFSET)
#define CATHODE_D_ON() CATHODE_D_GPIO->BSRR = 1 << (CATHODE_D_PIN + PIN_OFFSET)
#define CATHODE_E_ON() CATHODE_E_GPIO->BSRR = 1 << (CATHODE_E_PIN + PIN_OFFSET)
#define CATHODE_F_ON() CATHODE_F_GPIO->BSRR = 1 << (CATHODE_F_PIN + PIN_OFFSET)
#define CATHODE_G_ON() CATHODE_G_GPIO->BSRR = 1 << (CATHODE_G_PIN + PIN_OFFSET)
#define CATHODE_DP_ON() CATHODE_DP_GPIO->BSRR = 1 << (CATHODE_DP_PIN + PIN_OFFSET)

#define ANODE_1_OFF() ANODE_1_GPIO->BSRR = 1 << ANODE_1_PIN
#define ANODE_2_OFF() ANODE_2_GPIO->BSRR = 1 << ANODE_2_PIN
#define ANODE_3_OFF() ANODE_3_GPIO->BSRR = 1 << ANODE_3_PIN
#define ANODE_4_OFF() ANODE_4_GPIO->BSRR = 1 << ANODE_4_PIN

#define CATHODE_A_OFF() CATHODE_A_GPIO->BSRR = 1 << CATHODE_A_PIN
#define CATHODE_B_OFF() CATHODE_B_GPIO->BSRR = 1 << CATHODE_B_PIN
#define CATHODE_C_OFF() CATHODE_C_GPIO->BSRR = 1 << CATHODE_C_PIN
#define CATHODE_D_OFF() CATHODE_D_GPIO->BSRR = 1 << CATHODE_D_PIN
#define CATHODE_E_OFF() CATHODE_E_GPIO->BSRR = 1 << CATHODE_E_PIN
#define CATHODE_F_OFF() CATHODE_F_GPIO->BSRR = 1 << CATHODE_F_PIN
#define CATHODE_G_OFF() CATHODE_G_GPIO->BSRR = 1 << CATHODE_G_PIN
#define CATHODE_DP_OFF() CATHODE_DP_GPIO->BSRR = 1 << CATHODE_DP_PIN

int current_position = 0; // Indicates which digit is currently on.

static void one_on() {
    CATHODE_B_ON();
    CATHODE_C_ON();    
}

static void two_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_D_ON();
    CATHODE_E_ON();
    CATHODE_G_ON();
}

static void three_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_G_ON();
}

static void four_on() {
    CATHODE_B_ON();
    CATHODE_C_ON();
    CATHODE_F_ON();
    CATHODE_G_ON();
}

static void five_on() {
    CATHODE_A_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_F_ON();
    CATHODE_G_ON();
}

static void six_on() {
    CATHODE_A_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_E_ON();
    CATHODE_F_ON();
    CATHODE_G_ON();
}

static void seven_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_C_ON();
}

static void eight_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_E_ON();
    CATHODE_F_ON();
    CATHODE_G_ON();
}

static void nine_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_F_ON();
    CATHODE_G_ON();
}

static void zero_on() {
    CATHODE_A_ON();
    CATHODE_B_ON();
    CATHODE_C_ON();
    CATHODE_D_ON();
    CATHODE_E_ON();
    CATHODE_F_ON();
}

static void digit_off() {
    CATHODE_A_OFF();
    CATHODE_B_OFF();
    CATHODE_C_OFF();
    CATHODE_D_OFF();
    CATHODE_E_OFF();
    CATHODE_F_OFF();
    CATHODE_G_OFF();
}

static void (*light_digit[NUMBER_OF_DIGITS])() = {
    zero_on, 
    one_on, 
    two_on, 
    three_on, 
    four_on, 
    five_on, 
    six_on, 
    seven_on, 
    eight_on, 
    nine_on
};

static void dot_on() {
    CATHODE_DP_ON();
}

static void dot_off() {
    CATHODE_DP_OFF();
}

static void display_position_on(int position) {
    switch (position) {
        case 0:
            ANODE_1_ON();
            break;
        case 1:
            ANODE_2_ON();
            break;
        case 2:
            ANODE_3_ON();
            break;
        case 3:
            ANODE_4_ON();
            break;
    }
}

static void display_position_off(int position) {
    switch (position) {
        case 0:
            ANODE_1_OFF();
            break;
        case 1:
            ANODE_2_OFF();
            break;
        case 2:
            ANODE_3_OFF();
            break;
        case 3:
            ANODE_4_OFF();
            break;
    }
}

static int positions[NUMBER_OF_POSITIONS] = {
    0,
    0,
    0,
    0
};

static void set_timers() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
}

static void configure_anodes() {
    GPIOoutConfigure(ANODE_1_GPIO,
        ANODE_1_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(ANODE_2_GPIO,
        ANODE_2_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(ANODE_3_GPIO,
        ANODE_3_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(ANODE_4_GPIO,
        ANODE_4_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);
}

static void configure_cathodes() {
   GPIOoutConfigure(CATHODE_A_GPIO,
        CATHODE_A_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_B_GPIO,
        CATHODE_B_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_C_GPIO,
        CATHODE_C_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_D_GPIO,
        CATHODE_D_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_E_GPIO,
        CATHODE_E_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_F_GPIO,
        CATHODE_F_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_G_GPIO,
        CATHODE_G_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL);

    GPIOoutConfigure(CATHODE_DP_GPIO,
        CATHODE_DP_PIN,
        GPIO_OType_PP,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL); 
}

void configure_counter_miliseconds() {
    TIM3->CR1 = 0;
    TIM3->PSC = 16000 - 1; // Change into milisecond
    TIM3->ARR = 10; // 1/100 of second
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = ~(TIM_SR_UIF);
    TIM3->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM2->CR1 = 0;
    TIM2->PSC = 16 - 1;
    TIM2->ARR = 100;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = ~(TIM_SR_UIF);
    TIM2->DIER = TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void reset_all() {
    ANODE_1_OFF();
    ANODE_2_OFF();
    ANODE_3_OFF();
    ANODE_4_OFF();

    CATHODE_A_OFF();
    CATHODE_B_OFF();
    CATHODE_C_OFF();
    CATHODE_D_OFF();
    CATHODE_E_OFF();
    CATHODE_F_OFF();
    CATHODE_G_OFF();
    CATHODE_DP_OFF();
}

void configure_timer() {
    set_timers();

    configure_cathodes();
    configure_anodes();

    reset_all();
}

void update_time(int position) {
    if (position != 0) {
        if (positions[position] == 9) {
            update_time(position - 1);
        }
    }
    positions[position] = (positions[position] + 1) % 10;
}

void refresh() {
    // We turn off the previous digit.
    dot_off();
    digit_off();
    display_position_off(current_position);

    current_position = (current_position + 1) % NUMBER_OF_POSITIONS;
    // We turn on the next digit.
    display_position_on(current_position);
    light_digit[positions[current_position]]();
    // We want to have seconds.miliseconds timer,
    // so we light only central dot.
    if (current_position == 1) {
        dot_on();
    }
}

void set_zeroes() {
    for (int i = 0; i < NUMBER_OF_POSITIONS; i++) {
        positions[i] = 0;
    }
}

void debug(int position, int number) {
    positions[position] = number;
}
