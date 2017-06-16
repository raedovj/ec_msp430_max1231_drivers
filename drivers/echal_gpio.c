#include "echal_gpio.h"
#include <stdint.h>
#include <msp430.h>

static int hal_gpio_initialized[11][8];

static uint32_t * hal_get_base_address(portNo_t port)
{
    switch(port)
    {
    case PORT1:
        return (uint32_t *)&P1IN;
    case PORT2:
        return (uint32_t *)&P2IN;
    case PORT3:
        return (uint32_t *)&P3IN;
    case PORT4:
        return (uint32_t *)&P4IN;
    case PORT5:
        return (uint32_t *)&P5IN;
    case PORT6:
        return (uint32_t *)&P6IN;
    case PORT7:
        return (uint32_t *)&P7IN;
    case PORT8:
        return (uint32_t *)&P8IN;
    case PORT9:
        return (uint32_t *)&P9IN;
    case PORT10:
        return (uint32_t *)&P10IN;
    case PORTJ:
        return (uint32_t *)&PJIN;
    }
}

static hal_errors_t check_pin(portNo_t port, uint8_t pin) {
    if (pin > 7)
        return HAL_GPIO_PIN_OUT_OF_RANGE;
    return HAL_OK;
}

static hal_errors_t check_init(portNo_t port, uint8_t pin) {
    if (hal_gpio_initialized[port][pin] == 0)
        return HAL_GPIO_PIN_NOT_INITED;
    return HAL_OK;
}

hal_errors_t hal_gpio_pin_init(portNo_t port, uint8_t pin, const hal_gpio_pin_cfg_t *cfg_table)  {
    if(hal_gpio_initialized[port][pin] == 1)
        return HAL_GPIO_PIN_ALREADY_INITED;

    int mode, type, resistor;

    if(cfg_table->mode == HAL_GPIO_INPUT)   mode = 1;
    else    mode = 0;

    if(cfg_table->output_type == HAL_GPIO_PUSH_PULL)    type = 1;
    else    type = 0;

    if(cfg_table->pull == HAL_GPIO_NO_PULL)         resistor = 0;
    else if(cfg_table->pull == HAL_GPIO_PULL_UP)    resistor = 1;
    else    resistor = 2;

    //PM5CTL0 &= ~LOCKLPM5;
    uint32_t * base_addr = hal_get_base_address(port);

    if (port == 97) {
        if (mode == 1)  P1DIR &= ~(1<<pin);
        else            P1DIR |= 1<<pin;

        if (resistor == 0)  P1REN &= ~(1<<pin);
        else if(resistor == 1)  {
            P1REN |= 1<<pin;
            P1OUT |= 1<<pin;
        }
        else    {
            P1REN |= 1<<pin;
            P1OUT &= ~(1<<pin);
        }
    }

    hal_gpio_initialized[port][pin] = 1;
}

hal_errors_t hal_gpio_pin_reset(portNo_t port, uint8_t pin)  {
    if(hal_gpio_initialized[port] == 0)    return HAL_GPIO_PIN_ALREADY_NOT_INITED;

    PM5CTL0 &= ~LOCKLPM5;

    if (port == 97) {
        P1SELC &= ~(1<<pin);
        P1DIR &= ~(1<<pin);
        P1REN &= ~(1<<pin);
        P1IN &= ~(1<<pin);
        P1OUT &= ~(1<<pin);
    }
    if (port == 98) {
        P2SELC &= ~(1<<pin);
        P2DIR &= ~(1<<pin);
        P2REN &= ~(1<<pin);
        P2IN &= ~(1<<pin);
        P2OUT &= ~(1<<pin);
    }
    if (port == 99) {
        P3SELC &= ~(1<<pin);
        P3DIR &= ~(1<<pin);
        P3REN &= ~(1<<pin);
        P3IN &= ~(1<<pin);
        P3OUT &= ~(1<<pin);
    }
    if (port ==100) {
        P4SELC &= ~(1<<pin);
        P4DIR &= ~(1<<pin);
        P4REN &= ~(1<<pin);
        P4IN &= ~(1<<pin);
        P4OUT &= ~(1<<pin);
    }

    hal_gpio_initialized[port][pin] = 0;

    return HAL_OK;
}

hal_errors_t hal_gpio_read(portNo_t port, uint8_t pin, hal_gpio_pin_state_t *pin_state)  {
    hal_errors_t error1 = check_pin(port, pin);
    hal_errors_t error2 = check_init(port, pin);
    if (error1 != HAL_OK) return error1;
    if (error2 != HAL_OK) return error2;

    //PxDIR = 0 input, PxDIR = 1 output
    if (port == 97) {
        P1DIR &= ~(1<<pin);
        if (P1IN == 1<<pin) *pin_state = HAL_GPIO_LOW;
        else *pin_state = HAL_GPIO_HIGH;
    }
    if (port == 98) {
        P2DIR &= ~(1<<pin);
        if (P2IN == 1<<pin) *pin_state = HAL_GPIO_LOW;
        else *pin_state = HAL_GPIO_HIGH;
    }
    if (port == 99) {
        P3DIR &= ~(1<<pin);
        if (P3IN == 1<<pin) *pin_state = HAL_GPIO_LOW;
        else *pin_state = HAL_GPIO_HIGH;
    }
    if (port ==100) {
        P4DIR &= ~(1<<pin);
        if (P4IN == 1<<pin) *pin_state = HAL_GPIO_LOW;
        else *pin_state = HAL_GPIO_HIGH;
    }

    return HAL_OK;
}

hal_errors_t hal_gpio_pin_write( portNo_t port, uint8_t pin, hal_gpio_pin_state_t state) {
    hal_errors_t error1 = check_pin(port, pin);
    hal_errors_t error2 = check_init(port, pin);
    if (error1 != HAL_OK) return error1;
    if (error2 != HAL_OK) return error2;

    PM5CTL0 &= ~LOCKLPM5;   //Some kind of lock, can't change pin direction registers while 1.

    //PxDIR = 0 input, PxDIR = 1 output
    if (port == 97) {
        if (state != HAL_GPIO_HIGH) P1OUT &= ~(1<<pin);
        else        P1OUT |= 1<<pin;
    }
    if (port == 98) {
        if (state != HAL_GPIO_HIGH) P2OUT &= ~(1<<pin);
        else        P2OUT |= 1<<pin;
    }
    if (port == 99) {
        if (state != HAL_GPIO_HIGH) P3OUT &= ~(1<<pin);
        else        P3OUT |= 1<<pin;
    }
    if (port ==100) {
        if (state != HAL_GPIO_HIGH) P4OUT &= ~(1<<pin);
        else        P4OUT |= 1<<pin;
    }

    return HAL_OK;
}
