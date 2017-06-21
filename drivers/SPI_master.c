/*
 * SPI_master.c
 *
 *  Created on: 03.01.2015
 *      Author: Martin Põder <martin.poder@estcube.eu>
 *      Author: Erik Ilbis <erik.ilbis@estcube.eu>
 */

#include "drivers/SPI_master.h"
#include <msp430.h>
#include <stdint.h>

#pragma SET_DATA_SECTION(".TI.persistent")

static uint8_t SPI_master_buffer;	//stores master SPI received data byte

#pragma SET_DATA_SECTION()

// ADC bus
void SPI_B0_initialize() {
// Put the EUSCI into reset state
    UCB0CTLW0 = UCSWRST;
// Configure MISO, MOSI and CLK alternative function
    P1SEL0 |= BIT4 | BIT6 | BIT7;
    P1SEL1 &= ~(BIT4 | BIT6 | BIT7);
// Configure port
    UCB0CTLW0 |=
            (UCCKPH)    |   // clock phase REVERSED when compared to usual practice!!!
                                // 0 - data sampled at second edge, 1 - first edge
            (UCCKPL & 0)    |   // clock polarity
            (UCMSB)     |   // 0-lsb 1-msb
            (UC7BIT & 0)    |   // 0-8bit data, 1-7bit
            (UCMST)         |   //0-slave, 1-master
            (UCMODE0 & 0)   |   //00-3w spi, 01-4w spi (slave active ste=1)
            (UCMODE1 & 0)   |   //10-4w spi (slave active ste=0)
            (UCSYNC)        |   // 0-async, 1-sync
            (UCSSEL0)       |   //01-aclk, 10&11-smclk
            (UCSSEL1)       |
            (UCSTEM & 0);       //ignored in 3w mode, 0-prevent conflicts, 1-generate cs for slave

    //UCB0CTLW0 = 0xA9C1;
    UCB0BRW = 16;                //f_brclk/(UCBRx+1)
// Remove reset
   UCB0CTLW0 &= ~UCSWRST;
// Enable interrupts

}

// MISC bus
void SPI_B1_initialize() {
// Put the EUSCI into reset state
    UCB1CTLW0 = UCSWRST;
// Configure MISO, MOSI and CLK alternative function
    P4SEL0 &= ~(BIT5 | BIT6 | BIT7);
    P4SEL1 |= BIT5 | BIT6 | BIT7;
// Configure port
    UCB1CTLW0 |=
            (UCCKPH)    |   // clock phase REVERSED when compared to usual practice!!!
                                // 0 - data sampled at second edge, 1 - first edge
            (UCCKPL & 0)    |   // clock polarity
            (UCMSB)     |   // 0-lsb 1-msb
            (UC7BIT & 0)    |   // 0-8bit data, 1-7bit
            (UCMST)         |   //0-slave, 1-master
            (UCMODE0 & 0)   |   //00-3w spi, 01-4w spi (slave active ste=1)
            (UCMODE1 & 0)   |   //10-4w spi (slave active ste=0)
            (UCSYNC)        |   // 0-async, 1-sync
            (UCSSEL0)       |   //01-aclk, 10&11-smclk
            (UCSSEL1)       |
            (UCSTEM & 0);       //ignored in 3w mode, 0-prevent conflicts, 1-generate cs for slave
    //UCB1CTLW0 = 0xA9C1;
    UCB1BRW = 16;                //f_brclk/(UCBRx+1)
// Remove reset
   UCB1CTLW0 &= ~UCSWRST;
// Enable interrupts

}

uint8_t SPI_master_swapData(uint8_t port, uint8_t data) {
    if(port == SPI_PORT_ADC) {
        while (!(UCB0IFG & UCTXIFG));   //Buffer empty?
        UCB0TXBUF = data;
        while (!(UCB0IFG & UCRXIFG));   //Byte received?
        SPI_master_buffer = UCB0RXBUF;
        return SPI_master_buffer;
    }
    else if(port == SPI_PORT_MISC) {
        while (!(UCB1IFG & UCTXIFG));   //Buffer empty?
        UCB1TXBUF = data;
        while (!(UCB1IFG & UCRXIFG));   //Byte received?
        SPI_master_buffer = UCB1RXBUF;
        return SPI_master_buffer;
    }
    else
        return 0;
}

uint8_t SPI_checkBusy(uint8_t port) {
    if(port == SPI_PORT_ADC)
        return UCB0STATW & UCBUSY;
    else if(port == SPI_PORT_MISC)
        return UCB1STATW & UCBUSY;
    else
        return 2;
}

void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR(void) {
    __no_operation();
}

void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR(void) {
    __no_operation();
}
