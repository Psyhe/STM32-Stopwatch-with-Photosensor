#include <stm32.h>
#include <gpio.h>
#include <delay.h>
#include <string.h>

#include "timer.h"

#define GPIO1_PIN 6
#define GPIO_SCL_PIN 8
#define GPIO_SDA_PIN 9

#define GPIO_SCL GPIOB
#define GPIO_SDA GPIOB
#define GPIO1 GPIOA

#define I2C_SPEED_HZ 100000
#define PCLK1_MHZ 16
#define IGNORE_PERIOD_MS 20

#define I2C_SLAVE__DEVICE_ADDRESS_ADDRESS 0x212
#define I2C_SLAVE__DEVICE_ADDRESS_VALUE 0x29 

#define SYSRANGE_START_ADDRESS 0x18
#define SYSRANGE_START_VALUE 0x03 // We start continuous mode.
#define SYSTRANGE_START_RESET 0x00

#define SYSTEM__INTERRUPT_CLEAR_ADDRESS 0x015
#define SYSTEM__INTERRUPT_CLEAR_VALUE 0x7 // We clean all interrupts.

#define SYSTEM__INTERRUPT_CONFIG_GPIO_ADDRESS 0x014
#define SYSTEM__INTERRUPT_CONFIG_GPIO_VALUE 0x1 // We set interrupt on GPIO1.

#define SYSRANGE__THRESH_LOW_ADDRESS 0x01A
#define SYSRANGE__THRESH_LOW_VALUE 0x14

#define SYSRANGE__INTERMEASUREMENT_PERIOD_ADDRESS 0x01B
#define SYSRANGE__INTERMEASUREMENT_PERIOD_VALUE 0x0A

#define SYSTEM__MODE_GPIO1_ADDRESS 0x011
#define SYSTEM__MODE_GPIO1_VALUE 0x30

/********************************** GLOBAL VARIABLES **********************************/

int started = 0;
int temporary_disabled = 0;
int temporary_disabled_counter = 0;
int register_address;
int value;
int which_to_be_configured = -1;
int configured = 0;
int counter = 0;
int start_timer = 1;
int exti_nr = 1;

int step = 0;

static void configure_I2C() {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C1->CR1 = 0;
    I2C1->CCR = (PCLK1_MHZ * 1000000) /
                (I2C_SPEED_HZ << 1);
    I2C1->CR2 = PCLK1_MHZ;
    I2C1->TRISE = PCLK1_MHZ + 1;

    I2C1->CR2 |= I2C_CR2_ITEVTEN;

    NVIC_EnableIRQ(I2C1_EV_IRQn);

    I2C1->CR1 |= I2C_CR1_PE;

    GPIOafConfigure(GPIO_SCL, 
        GPIO_SCL_PIN, 
        GPIO_OType_OD,
        GPIO_Low_Speed, 
        GPIO_PuPd_NOPULL,
        GPIO_AF_I2C1);

    GPIOafConfigure(GPIO_SDA, 
        GPIO_SDA_PIN,
        GPIO_OType_OD,
        GPIO_Low_Speed,
        GPIO_PuPd_NOPULL,
        GPIO_AF_I2C1);
}

static void configure_exti() {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    GPIOinConfigure(GPIO1, 
        GPIO1_PIN, 
        GPIO_PuPd_UP,
        EXTI_Mode_Interrupt,
        EXTI_Trigger_Rising);

    EXTI->PR = EXTI_PR_PR6;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void send_via_i2c(int register_address_local, int value_local) {
    register_address = register_address_local;
    value = value_local;
    I2C1->CR1 |= I2C_CR1_START;
}

static void set_configuration() {
    if (!configured) {
        which_to_be_configured++;
        switch(which_to_be_configured) {
            case 0:
                send_via_i2c(SYSRANGE_START_ADDRESS, SYSTRANGE_START_RESET);
                break;
            case 1:
                send_via_i2c(SYSTEM__INTERRUPT_CLEAR_ADDRESS, SYSTEM__INTERRUPT_CLEAR_VALUE);
                break;
            case 2:
                send_via_i2c(SYSRANGE__THRESH_LOW_ADDRESS, SYSRANGE__THRESH_LOW_VALUE);
                break;
            case 3:
                send_via_i2c(SYSRANGE__INTERMEASUREMENT_PERIOD_ADDRESS, SYSRANGE__INTERMEASUREMENT_PERIOD_VALUE);
                break;
            case 4:
                send_via_i2c(SYSTEM__INTERRUPT_CONFIG_GPIO_ADDRESS, SYSTEM__INTERRUPT_CONFIG_GPIO_VALUE);
                break;
            case 5:
                send_via_i2c(SYSTEM__MODE_GPIO1_ADDRESS, SYSTEM__MODE_GPIO1_VALUE);
                break;
            case 6:
                send_via_i2c(SYSRANGE_START_ADDRESS, SYSRANGE_START_VALUE);
                break;
            case 7:
                send_via_i2c(SYSTEM__INTERRUPT_CLEAR_ADDRESS, SYSTEM__INTERRUPT_CLEAR_VALUE);
                break;
        }

        if (which_to_be_configured == 7) {
            configured = 1;
        }
    }
}

void I2C1_EV_IRQHandler(void) {
    if (I2C1->SR1 & I2C_SR1_SB && step == 0) {
        I2C1->DR = I2C_SLAVE__DEVICE_ADDRESS_VALUE << 1;
        step++;
    }
    if (I2C1->SR1 & I2C_SR1_ADDR && step == 1) {
        I2C1->SR2;
        I2C1->DR = 0x00;
        step++;
    }
    if (I2C1->SR1 & I2C_SR1_TXE && step == 2) {
        I2C1->DR = register_address;
        step++;
    }
    if (I2C1->SR1 & I2C_SR1_TXE && step == 3) {
        I2C1->DR = value;
        step++;
    }
    if (I2C1->SR1 & I2C_SR1_BTF && step == 4) {
        I2C1->CR1 |= I2C_CR1_STOP;
        set_configuration(); // Set next register.
        step = 0;
    }
}

// Updates the time on the display.
void TIM3_IRQHandler(void) {
    uint32_t it_status = TIM3->SR & TIM3->DIER;
    if (it_status & TIM_SR_UIF) {
        TIM3->SR = ~TIM_SR_UIF;
        if (started) {
            update_time(NUMBER_OF_POSITIONS - 1);
        }
    }
    if (it_status & TIM_SR_CC1IF) {
        TIM3->SR = ~TIM_SR_CC1IF;
    }
    temporary_disabled_counter++;

    // Prevents multiple interrupts from one event.
    if (temporary_disabled_counter == IGNORE_PERIOD_MS) {
        temporary_disabled = 0;
        temporary_disabled_counter = 0;
    }
}

// Refreshes the display.
void TIM2_IRQHandler(void) {
    uint32_t it_status = TIM2->SR & TIM2->DIER;
    if (it_status & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;
    }
    if (it_status & TIM_SR_CC1IF) {
        TIM2->SR = ~TIM_SR_CC1IF;
    }
    refresh();
}

// Reacts to the external interrupt from the sensor.
void EXTI9_5_IRQHandler(void) {
    EXTI->PR = EXTI_PR_PR6;
    send_via_i2c(SYSTEM__INTERRUPT_CLEAR_ADDRESS, SYSTEM__INTERRUPT_CLEAR_VALUE); // Clears interrupt.

    if (!temporary_disabled && start_timer == 1) {
        TIM3->CR1 |= TIM_CR1_CEN;
        started = 1;
        start_timer++;
        temporary_disabled = 1;
    }
    else if (!temporary_disabled && start_timer == 2) {
        TIM3->CR1 &= ~TIM_CR1_CEN;
        send_via_i2c(SYSRANGE_START_ADDRESS, SYSRANGE_START_VALUE); // Turns off continuous mode.
        started = 0;
        start_timer++;
    }
}

void start_display() {
    TIM2->CR1 |= TIM_CR1_CEN;
}

int main() {
    configure_timer();
    configure_exti();
    configure_I2C();

    configure_counter_miliseconds();
    start_display();

    set_configuration();

    for (;;){};
}