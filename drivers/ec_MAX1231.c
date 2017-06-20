#include "ec_MAX1231.h"

/*
 * ADC is configured to read temp and AIN0...AIN14 = total of 16 channels.
 * Reading initialized in ec_MAX1231_init() and re-inited in EOC interrupt
 * Results are stored in cfg_table->data
 */

ec_MAX1231_cfg_t * cfg_t_ptr[3];
static uint8_t zero_variable = 0x00;
//static uint8_t DMA_buffer[2] = { 0 }; // for storing both ADC MSBs and LSBs
static uint8_t current_ADC_index = 0; // for DMA interrupt to know, which ADC is sending data
//static uint8_t result_index = 0; // for DMA interrupt to know, where data in adc result array should be written
static uint8_t DMA_already_inited = 0;

uint16_t all_ADC_readings[ADC0_N_channels + ADC1_N_channels + ADC2_N_channels] = { 0 };

ec_MAX1231_cfg_t MAX1231_0 = {
                               .spi_conf = &hal_spi_channel_cfg_B0,
                               .data = &all_ADC_readings[0],
                               .index = ADC0,
                               .N_channels = ADC0_N_channels,
                               .ready = ADC_not_ready };

ec_MAX1231_cfg_t MAX1231_1 = {
                               .spi_conf = &hal_spi_channel_cfg_B0,
                               .data = &all_ADC_readings[ADC0_N_channels],
                               .index = ADC1,
                               .N_channels = ADC1_N_channels,
                               .ready = ADC_not_ready };

ec_MAX1231_cfg_t MAX1231_2 = {
                               .spi_conf = &hal_spi_channel_cfg_B0,
                               .data = &all_ADC_readings[ADC0_N_channels
                                               + ADC1_N_channels],
                               .index = ADC2,
                               .N_channels = ADC2_N_channels,
                               .ready = ADC_not_ready };

void ec_MAX1231_GPIO_init() {
    //ADC0
    //Configure nCNVST_pin
    P8DIR |= BIT2; // output
    P8OUT |= BIT2; // pull high

    //configure nCS_pin
    P2DIR |= BIT2; // output
    P2OUT |= BIT2; // pull high

    //Configure nEOC_pin with interrupt
    P2DIR &= ~(BIT3); // input
    P2IES |= BIT3; // Interrupt edge High->Low
    P2IFG &= ~BIT3; // interrupt flag cleared
    P2IE |= BIT3; // interrupt enabled

    //ADC1
    //Configure nCNVST_pin
    P8DIR |= BIT3; // output
    P8OUT |= BIT3; // pull high

    //configure nCS_pin
    P8DIR |= BIT0; // output
    P8OUT |= BIT0; // pull high

    //Configure nEOC_pin with interrupt
    P3DIR &= ~(BIT7); // input
    P3IES |= BIT7; // Interrupt edge High->Low
    P3IFG &= ~BIT7; // interrupt flag cleared
    P3IE |= BIT7; // interrupt enabled

    //ADC2
    //Configure nCNVST_pin
    P8DIR |= BIT1; // output
    P8OUT |= BIT1; // pull high

    //configure nCS_pin
    P3DIR |= BIT6; // output
    P3OUT |= BIT6; // pull high

    //Configure nEOC_pin with interrupt
    P3DIR &= ~(BIT5); // input
    P3IES |= BIT5; // Interrupt edge High->Low
    P3IFG &= ~BIT5; // interrupt flag cleared
    P3IE |= BIT5; // interrupt enabled
}

void ec_MAX1231_init(ec_MAX1231_cfg_t *cfg_table)
{
    __delay_cycles(100);
    if (cfg_table->index == ADC0)
    {
        cfg_t_ptr[0] = cfg_table;
        P2OUT &= ~BIT2; // pull CS low
    }
    else if (cfg_table->index == ADC1)
    {
        cfg_t_ptr[1] = cfg_table;
        P8OUT &= ~BIT0; // pull CS low
    }
    else
    {
        cfg_t_ptr[2] = cfg_table;
        P3OUT &= ~BIT6; // pull CS low
    }
    __delay_cycles(100);
    //SPI writes to init MAX1231
    // Write to setup register
    static const uint8_t CKSEL_bits = 0b00; // 2 bits // CNSTV triggers N conversions - stored in FIFO
    static const uint8_t REFSEL_bits = 0b01; // 2 bits // External reference
    static uint8_t DIFFSEL_bits = 0b10; // 2 bits // One byte of data follows the setup byte and is written to the unipolar mode register.
    hal_spi_write(cfg_table->spi_conf,(uint8_t) ((0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits)));

    // Write to unipolar / bipolar config register
    hal_spi_write(cfg_table->spi_conf, (uint8_t) (0b00000000)); // AIN0...AIN15 are set to unipolar mode
    DIFFSEL_bits = 11; // 2 bits // One byte of data follows the setup byte and is written to the bipolar mode register
    hal_spi_write(cfg_table->spi_conf, (uint8_t) ((0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits)));
    hal_spi_write(cfg_table->spi_conf, (uint8_t) (0b00000000)); // none of AIN0...AIN15 are set to bipolar mode

    // Write to conversion register
    uint8_t CHSEL_bits = cfg_table->N_channels;// 4 bits // Config how many channels are to be scanned
    static const uint8_t SCAN_bits = 0b00;     // 2 bits // Scans channels 0 through N - selected by CHSEL3–CHSEL0.
    static const uint8_t TEMP_bits = 0b0;      // 1 bit  // reads temperature
    hal_spi_write(cfg_table->spi_conf,(uint8_t) ((1 << 7) | (CHSEL_bits << 3) | (SCAN_bits << 1) | (TEMP_bits)));

    // Write to averaging register
    static const uint8_t AVGON_bits = 0;     // 1 bit  // Set to 1 to turn averaging on. Set to zero to turn averaging off.
    static const uint8_t NAVG_bits = 00;     // 2 bits // Configures the number of conversions for single-channel scans.
    static const uint8_t NSCAN_bits = 00;    // 2 bits // Single-channel scan count. (Scan mode 10 only.)
    hal_spi_write(cfg_table->spi_conf, (uint8_t) ( (0b001 << 5) | (AVGON_bits << 4) | (NAVG_bits << 2) | (NSCAN_bits) ));

    __delay_cycles(100);
    //Release all CS
    P2OUT |= BIT2;
    P8OUT |= BIT0;
    P3OUT |= BIT6;
    __delay_cycles(100);

    if (DMA_already_inited == 0)
    {
        //Init DMA0 - used for receiving and moving data to right array
        DMACTL0 |= DMA0TSEL__UCB0RXIFG0; //DMA Control 0 Register

        DMA0SA = &UCB0RXBUF;

        //disable DMA and it's interrupts
        DMA0CTL &= ~(DMAEN | DMAIE);

        //DMA Channel 0 Control Register
        // Single transfer + dest addr+=1 + dest,source = 1 byte
        DMA0CTL |= DMADT_4 + DMADSTINCR_2 + DMASRCBYTE_L + DMADSTBYTE_L;



        //Init DMA1 - used to write to TXBUF, so clock is generated
        DMACTL0 |= DMA1TSEL__UCB0RXIFG0; //DMA Control 0 Register

        DMA1SA = &zero_variable;
        DMA1DA = &UCB0TXBUF;

        //disable DMA and it's interrupts
        DMA1CTL &= ~(DMAEN | DMAIE);
        DMA1SZ = 32; // DMA interrupt should disable it before

        //DMA Channel 1 Control Register
        // Single transfer + dest,source = 1 byte
        DMA1CTL |= DMADT_4 + DMASRCBYTE_L + DMADSTBYTE_L;

        DMA_already_inited = 1;
    }
}

void ec_MAX1231_start_all_ADC_conversions() // how to check interrupt flag?
{
    cfg_t_ptr[0]->ready = ADC_not_ready;
    cfg_t_ptr[1]->ready = ADC_not_ready;
    cfg_t_ptr[2]->ready = ADC_not_ready;

    __disable_interrupt();

    //Toggle CNSTV pins
    P8OUT &= ~(BIT1 | BIT2 | BIT3);
    //__delay_cycles(100);
    P8OUT |= (BIT1 | BIT2 | BIT3);

    __enable_interrupt();
}

ADC_data_ready ec_MAX1231_check_results_available()
{
    if ((cfg_t_ptr[0]->ready == ADC_ready) && (cfg_t_ptr[1]->ready == ADC_ready)
            && (cfg_t_ptr[2]->ready == ADC_ready))
    {
        return ADC_ready;
    }
    return ADC_not_ready;
}

void ec_MAX1231_reset(ec_MAX1231_cfg_t *cfg_table)
{
    //reset pins

    //reset pin interrupts

    // Write reset to ADC
    hal_spi_write(cfg_table->spi_conf, 0b00010000);
    // Write reset FIFO to ADC
    hal_spi_write(cfg_table->spi_conf, 0b00011000);

    //reset SPI

}
void ec_MAX1231_clear_FIFO(ec_MAX1231_cfg_t *cfg_table)
{
    // Write reset FIFO to ADC
    hal_spi_write(cfg_table->spi_conf, 0b00011000);
}

void ec_MAX1231_read_results(ec_MAX1231_cfg_t *cfg_table, uint16_t *result)
{
    *result = *(cfg_table->data);
}

// Interrupt driven ADC reading with DMA
// EOC triggers pin interrupt
// pin interrupt sets CS, disables other pin interrupts
#pragma vector=PORT2_VECTOR //adc0 p2.3
__interrupt void Port_2(void)
{
    if (P2IFG & BIT3)
    {
        current_ADC_index = 0;

        //set DMA dest address
        DMA0DA = ((uint8_t *) &all_ADC_readings[ADC0_N_channels - 1]) + 1; //high byte address

        //set nr of bytes to read for DMA
        DMA0SZ = (cfg_t_ptr[0]->N_channels) * 2;

        P2OUT &= ~BIT2; // nCS

        P2IFG &= ~BIT3; // interrupt flag cleared

        // disable all ADC eoc interrupts
        P3IE &= ~BIT5;
        P3IE &= ~BIT7;
        P2IE &= ~BIT3;

        //enable DMAs
        DMA0CTL |= DMAEN | DMAIE;
        DMA1CTL |= DMAEN;
        UCB0TXBUF = 0; // init CLK
    }
}
#pragma vector=PORT3_VECTOR //adc1 p3.7     adc2 p3.5
__interrupt void Port_3(void)
{
    if (P3IFG & BIT7)
    {
        current_ADC_index = 1;

        //set DMA dest address
        DMA0DA = ((uint8_t *) &all_ADC_readings[ADC0_N_channels
                + ADC1_N_channels - 1]) + 1; //high byte address

        //set nr of bytes to read for DMA
        DMA0SZ = (cfg_t_ptr[1]->N_channels) * 2;

        P8OUT &= ~BIT0; // nCS

        P3IFG &= ~BIT7; // interrupt flag cleared

        // disable all ADC eoc interrupts
        P3IE &= ~BIT5;
        P3IE &= ~BIT7;
        P2IE &= ~BIT3;

        //enable DMAs
        DMA0CTL |= DMAEN | DMAIE;
        DMA1CTL |= DMAEN;
        UCB0TXBUF = 0; // init CLK
    }
    else if (P3IFG & BIT5)
    {
        current_ADC_index = 2;

        //set DMA dest address
        DMA0DA = ((uint8_t *) &all_ADC_readings[ADC0_N_channels
                + ADC1_N_channels + ADC2_N_channels - 1]) + 1; //high byte address

        //set nr of bytes to read for DMA
        DMA0SZ = (cfg_t_ptr[2]->N_channels) * 2;

        P3OUT &= ~BIT6; // nCS

        P3IFG &= ~BIT5; // interrupt flag cleared

        // disable all ADC eoc interrupts
        P3IE &= ~BIT5;
        P3IE &= ~BIT7;
        P2IE &= ~BIT3;

        //enable DMAs
        DMA0CTL |= DMAEN | DMAIE;
        DMA1CTL |= DMAEN;
        UCB0TXBUF = 0; // init CLK
    }
}

// When DMA has moved all of one ADC results, this interrupt will be triggered
#pragma vector=DMA_VECTOR
__interrupt void DMA0_ISR(void)
{
    switch (__even_in_range(DMAIV, 16))
    {
    case 0:
        break; // No interrupt
    case 2:        // DMA0IFG
        cfg_t_ptr[current_ADC_index]->ready = ADC_ready;
        //disable DMAs
        DMA0CTL &= ~(DMAEN | DMAIE);
        DMA1CTL &= ~DMAEN;
        //result_index = 0;
        P8OUT |= BIT0; //reset nCS
        P3OUT |= BIT6; //reset nCS
        P2OUT |= BIT2; //reset nCS
        //enable all ADC eoc interrupts
        P3IE |= BIT5;
        P3IE |= BIT7;
        P2IE |= BIT3;

        UCB0IFG &= ~UCRXIFG; //clear RX interrupts, if flag remains set
        break;
    case 4:
        break; // DMA1IFG
    case 6:
        break; // DMA2IFG
    default:
        break;
    }
}

