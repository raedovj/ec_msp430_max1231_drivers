
#include "echal_spi.h"
#include <msp430.h>
#include <stdint.h>
#include "echal_common.h"

hal_spi_errors_t hal_spi_init(hal_spi_channel_cfg_t *table)
{
    table->extra_offset = 0;

    if(table->port == HAL_SPI_A0)
    {
        table->base_address = &UCA0CTLW0;

        if(table->io_port == HAL_SPI_A0_P2)
        {
            P2SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3);
            P2SEL0 |= (BIT0 | BIT1 | BIT2 | BIT3);
        }
        else if(table->io_port == HAL_SPI_A0_P4)
        {
            P4SEL1 &= ~(BIT2 | BIT3);
            P4SEL0 |= (BIT2 | BIT3);

            P1SEL0 &= ~(BIT4 | BIT5);
            P1SEL1 |= (BIT4 | BIT5);
        }
        else
        {
            return HAL_SPI_IO_PORT_INCOMPATIBLE;
        }
    }
    else if(table->port == HAL_SPI_A1)
    {
        table->base_address = &UCA1CTLW0;

        if(table->io_port == HAL_SPI_A1_P3)
        {
            P3SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7);
            P3SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);
        }
        else if(table->io_port == HAL_SPI_A1_P5)
        {
            P5SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7);
            P5SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);
        }
        else
        {
            return HAL_SPI_IO_PORT_INCOMPATIBLE;
        }
    }
    else if(table->port == HAL_SPI_B0)
    {
        table->base_address = &UCB0CTLW0;
        table->extra_offset = 0x10;

        if(table->io_port == HAL_SPI_B0_P1)
        {
            P1SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7);
            P1SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);
        }
        else
        {
            return HAL_SPI_IO_PORT_INCOMPATIBLE;
        }
    }
    else if(table->port == HAL_SPI_B1)
    {
        table->base_address = &UCB1CTLW0;
        table->extra_offset = 0x10;

        if(table->io_port == HAL_SPI_B1_P3)
        {
            P3SEL1 &= ~(BIT0 | BIT1 | BIT2);
            P3SEL0 |= (BIT0 | BIT1 | BIT2);

            P5SEL0 &= ~(BIT3);
            P5SEL1 |= (BIT3);
        }
        else if(table->io_port == HAL_SPI_B1_P4)
        {
            P4SEL0 &= ~(BIT0 | BIT1 | BIT6 | BIT7);
            P4SEL1 |= (BIT0 | BIT1 | BIT6 | BIT7);
        }
        else
        {
            return HAL_SPI_IO_PORT_INCOMPATIBLE;
        }
    }

    volatile unsigned int * current_address = table->base_address;

    *(table->base_address) = UCSWRST;


    if(table->phase == HAL_SPI_PHASE_SECOND_TRANSITION)
    {
        *(table->base_address) |= UCCKPH;
    }
    else
    {
        *(table->base_address) &= ~UCCKPH;
    }

    if(table->polarity == HAL_SPI_POLARITY_IDLE_1)
    {
        *(table->base_address) |= UCCKPL;
    }
    else
    {
        *(table->base_address) &= ~UCCKPL;
    }

    if(table->bit_order == HAL_SPI_MSB_FIRST)
    {
        *(table->base_address) |= UCMSB;
    }
    else
    {
        *(table->base_address) &= ~UCMSB;
    }

    if(table->character_length == HAL_SPI_CHARACTER_LENGTH_7_BIT)
    {
        *(table->base_address) |= UC7BIT;
    }
    else
    {
        *(table->base_address) &= ~UC7BIT;
    }

    if(table->master == HAL_SPI_MASTER_CONFIGURATION)
    {
        *(table->base_address) |= UCMST;

        if(table->clock_source == HAL_SPI_CLOCK_SOURCE_ALCK)
        {
            *(table->base_address) |= UCSSEL_1;
        }
        else
        {
            *(table->base_address) |= UCSSEL_2;
        }
    }
    else
    {
        *(table->base_address) &= ~UCMST;
        *(table->base_address) &= ~UCSSEL_3;
    }

    if(table->communication_mode == HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_1)
    {
        *(table->base_address) |= UCMODE_1;
    }
    else if(table->communication_mode == HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_0)
    {
        *(table->base_address) |= UCMODE_2;
    }
    else
    {
        *(table->base_address) &= ~UCMODE_1;
        *(table->base_address) &= ~UCMODE_2;
    }

    if(table->synchronus_mode == HAL_SPI_SYNCRONUS_MODE_ENBLED)
    {
        *(table->base_address) |= UCSYNC;
    }
    else
    {
        *(table->base_address) &= ~UCSYNC;
    }

    if(table->software_slave_management == HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_DISABLED)
    {
        *(table->base_address) |= UCSTEM;
    }
    else
    {
        *(table->base_address) &= ~UCSTEM;
    }


    current_address += 0x3;
    *current_address = table->prescaler;

    *(table->base_address) &= ~(UCSWRST);

    return HAL_SPI_OK;
}

hal_spi_errors_t hal_spi_write(hal_spi_channel_cfg_t *table, char c)
{
    //volatile unsigned int * base_address = table->base_address;
    uint16_t IFG_offset;

    if(table->port == HAL_SPI_A0 || table->port == HAL_SPI_A1)
    {
        IFG_offset = 0xE;
    }
    else if(table->port == HAL_SPI_B0 || table->port == HAL_SPI_B1)
    {
        IFG_offset = 0x16;
    }
    else
        return HAL_SPI_PORT_INVALID;

    while(!(*(table->base_address + IFG_offset) & UCTXIFG));
    *(table->base_address + 0x7) = c;

    return HAL_SPI_OK;
}

hal_spi_errors_t hal_spi_read(hal_spi_channel_cfg_t *table, uint8_t *arr, uint8_t bytes)
{
    uint8_t byte;
    for(byte = 0; byte < 8 * bytes; byte++)
    {
        arr[byte] = (table->base_address + 6)[byte];
    }
    return HAL_SPI_OK;
}
