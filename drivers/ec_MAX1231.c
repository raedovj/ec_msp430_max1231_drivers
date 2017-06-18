#include "ec_MAX1231.h"
#include <stdint.h>

/*
 * ADC is configured to read temp and AIN0...AIN14 = total of 16 channels.
 * Reading initialized in ec_MAX1231_init() and re-inited in EOC interrupt
 * Results are stored in cfg_table->data
 */


ec_MAX1231_cfg_t * cfg_t_ptr[3];

void ec_MAX1231_init(ec_MAX1231_cfg_t *cfg_table)
{

    if (cfg_table->index == ADC0)
    {
        //Configure nCNVST_pin
        P8DIR |= BIT2; // output
        P8OUT |= BIT2; // pull high

        //configure nCS_pin
        P2DIR |= BIT2; // output
        P2OUT |= BIT2; // pull high

        //Configure nEOC_pin with interrupt
        P2DIR &= ~(BIT3); // input
        P2IE |= BIT3; // interrupt enabled
        P2IFG &= ~BIT3; // interrupt flag cleared

        cfg_t_ptr[0] = cfg_table;
    }
    else if (cfg_table->index == ADC1)
    {
        //Configure nCNVST_pin
        P8DIR |= BIT3; // output
        P8OUT |= BIT3; // pull high

         //configure nCS_pin
        P8DIR |= BIT0; // output
        P8OUT |= BIT0; // pull high

        //Configure nEOC_pin with interrupt
        P3DIR &= ~(BIT7); // input
        P3IE |= BIT7; // interrupt enabled
        P3IFG &= ~BIT7; // interrupt flag cleared

        cfg_t_ptr[1] = cfg_table;
    }
    else if (cfg_table->index == ADC2)
    {
        //Configure nCNVST_pin
        P8DIR |= BIT1; // output
        P8OUT |= BIT1; // pull high

        //configure nCS_pin
        P3DIR |= BIT6; // output
        P3OUT |= BIT6; // pull high

        //Configure nEOC_pin with interrupt
        P3DIR &= ~(BIT5); // input
        P3IE |= BIT5; // interrupt enabled
        P3IFG &= ~BIT5; // interrupt flag cleared

        cfg_t_ptr[1] = cfg_table;
    }
    //configure SPI

    //SPI writes to init MAX1231
    // Write to conversion register
    uint8_t CHSEL_bits = cfg_table->N_channels;  // 4 bits // Config how many channels are to be scanned
    uint8_t SCAN_bits = 00;     // 2 bits // Scans channels 0 through N - selected by CHSEL3–CHSEL0.
    uint8_t TEMP_bits = 1;      // 1 bit  // reads temperature
    spi_write( (uint8_t) ( (1 << 7) | (CHSEL_bits << 3) | (SCAN_bits << 1) | (TEMP_bits) );

    // Write to setup register
    uint8_t CKSEL_bits = 01;    // 2 bits // Acquisition externally timed through CNVST
    uint8_t REFSEL_bits = 10;   // 2 bits // Reference always on; no wake-up delay.
    uint8_t DIFFSEL_bits = 10;  // 2 bits // One byte of data follows the setup byte and is written to the unipolar mode register.
    spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );

    // Write to unipolar / bipolar config register
    spi_write( (uint8_t) (0b11111111) ); // AIN0...AIN15 are set to unipolar mode
    spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );
    uint8_t DIFFSEL_bits = 11;  // 2 bits // One byte of data follows the setup byte and is written to the bipolar mode register
    spi_write( (uint8_t) (0b00000000) ); // none of AIN0...AIN15 are set to bipolar mode

    /*
    // Write to averaging register
    uint8_t AVGON_bits = 0;     // 1 bit  // Set to 1 to turn averaging on. Set to zero to turn averaging off.
    uint8_t NAVG_bits = 00;     // 2 bits // Configures the number of conversions for single-channel scans.
    uint8_t NSCAN_bits = 00;    // 2 bits // Single-channel scan count. (Scan mode 10 only.)
    spi_write( (uint8_t) ( (0b001 << 5) | (AVGON_bits << 4) | (NAVG_bits << 2) | (NSCAN_bits) );
    */


}

void ec_MAX1231_reset(ec_MAX1231_cfg_t *cfg_table)
{
    //reset pins

    //reset pin interrupts

    // Write reset to ADC
    spi_write(0b00010000);
    // Write reset FIFO to ADC
    spi_write(0b00011000);

    //reset SPI

}
void ec_MAX1231_clear_FIFO(ec_MAX1231_cfg_t *cfg_table)
{
    // Write reset FIFO to ADC
    spi_write(0b00011000);
}


void ec_MAX1231_read_results(ec_MAX1231_cfg_t *cfg_table, uint16_t *result)
{
    // atomic? or return pointer of cfg_table->data
    for (uint8_t i = 0; i < cfg_table->N_channels; i++)
    {
        result[i] = cfg_table->data[i];
    }
}

// Interrupt driven ADC reading (w DMA??)
#pragma vector=PORT2_VECTOR //adc0 p2.3
__interrupt void Port_2(void)
{
    if (P2IFG && BIT3)
    {
        P2OUT &= ~BIT2; // nCS

        spi_read(cfg_t_ptr[0]->data, nr_of_bytes);

        P2IFG &= ~BIT3; // interrupt flag cleared

        P2OUT |= BIT2; //reset nCS

        // toggle nCNSTV
        P8OUT &= ~BIT2;
        P8OUT |= BIT2;
    }
}

// Interrupt driven ADC reading (w DMA)
#pragma vector=PORT3_VECTOR //adc1 p3.7     adc2 p3.6
__interrupt void Port_3(void)
{
    if (P3IFG && BIT7)
    {
        P8OUT &= 0BIT?; // nCS

        spi_read(cfg_t_ptr[1]->data, nr_of_bytes);

        P3IFG &= ~BIT7; // interrupt flag cleared

        P8OUT |= BIT0; //reset nCS

        // toggle nCNSTV
        P8OUT &= ~BIT3;
        P8OUT |= BIT3;
    }
    if (P3IFG && BIT6)
    {
        P3OUT &= ~BIT6; // nCS

        spi_read(cfg_t_ptr[2]->data, nr_of_bytes);

        P3IFG &= ~BIT6; // interrupt flag cleared

        P3OUT |= BIT6; //reset nCS

        // toggle nCNSTV
        P8OUT &= ~BIT1;
        P8OUT |= BIT1;
    }
}
