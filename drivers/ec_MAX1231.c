#include "ec_MAX1231.h"

/*
 * ADC is configured to read temp and AIN0...AIN14 = total of 16 channels.
 * Reading initialized in ec_MAX1231_init() and re-inited in EOC interrupt
 * Results are stored in cfg_table->data
 */

ec_MAX1231_cfg_t * cfg_t_ptr[3];

static uint16_t DMA_buffer = 0; // for storing both ADC MSBs and LSBs
static uint8_t current_ADC_index = 0; // for DMA interrupt to know, which ADC is sending data
static uint8_t result_index = 0; // for DMA interrupt to know, where data in adc result array should be written

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
    //spi_write( (uint8_t) ( (1 << 7) | (CHSEL_bits << 3) | (SCAN_bits << 1) | (TEMP_bits) );

    // Write to setup register
    uint8_t CKSEL_bits = 01;    // 2 bits // Acquisition externally timed through CNVST
    uint8_t REFSEL_bits = 10;   // 2 bits // Reference always on; no wake-up delay.
    uint8_t DIFFSEL_bits = 10;  // 2 bits // One byte of data follows the setup byte and is written to the unipolar mode register.
    //spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );

    // Write to unipolar / bipolar config register
    //spi_write( (uint8_t) (0b11111111) ); // AIN0...AIN15 are set to unipolar mode
    //spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );
    //DIFFSEL_bits = 11;  // 2 bits // One byte of data follows the setup byte and is written to the bipolar mode register
    //spi_write( (uint8_t) (0b00000000) ); // none of AIN0...AIN15 are set to bipolar mode

    /*
    // Write to averaging register
    uint8_t AVGON_bits = 0;     // 1 bit  // Set to 1 to turn averaging on. Set to zero to turn averaging off.
    uint8_t NAVG_bits = 00;     // 2 bits // Configures the number of conversions for single-channel scans.
    uint8_t NSCAN_bits = 00;    // 2 bits // Single-channel scan count. (Scan mode 10 only.)
    //spi_write( (uint8_t) ( (0b001 << 5) | (AVGON_bits << 4) | (NAVG_bits << 2) | (NSCAN_bits) );
    */


}

void ec_MAX1231_reset(ec_MAX1231_cfg_t *cfg_table)
{
    //reset pins

    //reset pin interrupts

    // Write reset to ADC
    //spi_write(0b00010000);
    // Write reset FIFO to ADC
    //spi_write(0b00011000);

    //reset SPI

}
void ec_MAX1231_clear_FIFO(ec_MAX1231_cfg_t *cfg_table)
{
    // Write reset FIFO to ADC
    //spi_write(0b00011000);
}


void ec_MAX1231_read_results(ec_MAX1231_cfg_t *cfg_table, uint16_t *result)
{
    result = cfg_table->data;
}

// Interrupt driven ADC reading (w DMA??)
#pragma vector=PORT2_VECTOR //adc0 p2.3
__interrupt void Port_2(void)
{
    if (P2IFG && BIT3)
    {
        current_ADC_index = 0;

        P2OUT &= ~BIT2; // nCS

        //spi_read(cfg_t_ptr[0]->data, nr_of_bytes);

        P2IFG &= ~BIT3; // interrupt flag cleared

        // toggle nCNSTV
        P8OUT &= ~BIT2;
        P8OUT |= BIT2;


        // disable all ADC eoc interrupts
        P3IE &= ~BIT5;
        P3IE &= ~BIT7;
        P2IE &= ~BIT3;
    }
}

// Interrupt driven ADC reading with DMA
// EOC triggers pin interrupt
// pin interrupt sets CS, disables other pin interrupts, starts next ADC's conversion
#pragma vector=PORT3_VECTOR //adc1 p3.7     adc2 p3.6
__interrupt void Port_3(void)
{
    if (P3IFG && BIT7)
    {
        current_ADC_index = 1;

        P8OUT &= BIT0; // nCS

        P3IFG &= ~BIT7; // interrupt flag cleared

        // toggle nCNSTV
        P8OUT &= ~BIT3;
        P8OUT |= BIT3;
    }
    if (P3IFG && BIT6)
    {
        current_ADC_index = 2;

        P3OUT &= ~BIT6; // nCS

        //spi_read(cfg_t_ptr[2]->data, nr_of_bytes);

        P3IFG &= ~BIT6; // interrupt flag cleared

        // toggle nCNSTV
        P8OUT &= ~BIT1;
        P8OUT |= BIT1;
    }
}
//
//dma
/*
A transfer is triggered when USCI_Bx receives new data. UCBxRXIFG is automatically reset when the transfer starts. If
UCBxRXIE is set, the UCBxRXIFG does not trigger a transfer.

In single transfer mode, each byte or word transfer requires a separate trigger. Figure 1-3 shows the
single transfer state diagram.
The DMAxSZ register defines the number of transfers to be made. The DMADSTINCR and
DMASRCINCR bits select if the destination address and the source address are incremented or
decremented after each transfer. If DMAxSZ = 0, no transfers occur.
The DMAxSA, DMAxDA, and DMAxSZ registers are copied into temporary registers. The temporary
values of DMAxSA and DMAxDA are incremented or decremented after each transfer. The DMAxSZ
register is decremented after each transfer. When the DMAxSZ register decrements to zero, it is reloaded
from its temporary register and the corresponding DMAIFG flag is set. When DMADT = {0}, the DMAEN
bit is cleared automatically when DMAxSZ decrements to zero and must be set again for another transfer
to occur.
In repeated single transfer mode, the DMA controller remains enabled with DMAEN = 1, and a transfer
occurs every time a trigger occurs.

The DMARMWDIS bit controls when the CPU is halted for DMA transfers. When DMARMWDIS = 0, the
CPU is halted immediately and the transfer begins when a trigger is received. In this case, it is possible
that CPU read-modify-write operations can be interrupted by a DMA transfer. When DMARMWDIS = 1,
the CPU finishes the currently executing read-modify-write operation before the DMA controller halts the
CPU and the transfer begins (see Table 1-2).

trigger18 UCB0RXIFG (SPI) channel0/1

The addressing modes are configured with the DMASRCINCR and DMADSTINCR control bits. The
DMASRCINCR bits select if the source address is incremented, decremented, or unchanged after each
transfer. The DMADSTINCR bits select if the destination address is incremented, decremented, or
unchanged after each transfer
*/


void DMA_init() {
    DMACTL0 |= DMA0TSEL__UCB0RXIFG0; //DMA Control 0 Register

    // Single transfer + DMAEN + dest addr+=1 + dest,source = 1 byte
    DMA0CTL |= DMADT_0 + DMAEN + DMADSTINCR1 + DMASRCBYTE_L + DMADSTBYTE_L + DMAIE; //DMA Channel 0 Control Register

    DMA0SZ = 2; // read adc low and high bytes, then trigger interrupt

    DMA0SA = &UCB0RXBUF;
    DMA0DA = &DMA_buffer;
}

#pragma vector=DMA_VECTOR
__interrupt void DMA0_ISR (void)
{
  switch(__even_in_range(DMAIV,16))
  {
    case 0: break; // No interrupt
    case 2:        // DMA0IFG
        REG_OFFSET(cfg_t_ptr[current_ADC_index]->data + result_index) = DMA_buffer;
        result_index+=2;
        DMA0CTL |= DMAEN;

        if (result_index == (cfg_t_ptr[current_ADC_index]->N_channels)*2)
        {
            result_index = 0;
            P8OUT |= BIT0; //reset nCS
            P3OUT |= BIT6; //reset nCS
            P2OUT |= BIT2; //reset nCS
            //enable all ADC eoc interrupts
            P3IE |= BIT5;
            P3IE |= BIT7;
            P2IE |= BIT3;
        }
      break;
    case 4: break; // DMA1IFG
    case 6: break; // DMA2IFG
    case 8: break; // Reserved
    case 10: break; // Reserved
    case 12: break; // Reserved
    case 14: break; // Reserved
    case 16: break; // Reserved
    default: break;
  }
}

