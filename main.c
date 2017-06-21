#include "drivers/echal_usart.h"
#include <msp430.h>
#include "drivers/ec_MAX1231.h"
#include "drivers/SPI_master.h"

void clock_init() {
// Enable HFXIN pins primary function
    PJSEL0 |= BIT6 | BIT7;
    PJSEL1 &= ~(BIT6 | BIT7);

// Unlock clock registers
    CSCTL0_H = CSKEY >> 8;
//Set clock sources
    CSCTL2 = SELA__VLOCLK | SELS__HFXTCLK | SELM__HFXTCLK;
//Set all dividers 1
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
//Configure crystal drive strength and frequency
    CSCTL4 = HFXTDRIVE_3 | HFFREQ1 | LFXTDRIVE_0 | LFXTBYPASS;
    CSCTL4 &= ~(HFXTOFF | LFXTOFF);
    CSCTL4 |= LFXTOFF;
//Wait for the HFXT error flag to clear
    do {
        CSCTL5 &= ~(HFXTOFFG | LFXTOFFG);
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG); //while(CSCTL5 & HFXTOFFG);
// Lock clock registers
    CSCTL0_H = 0;


}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    /*CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers */

    clock_init();

    P1IE = 0x00;
    P2IE = 0x00;
    P3IE = 0x00;
    P4IE = 0x00;

    //Configure RS485 direction to TX
    P5DIR |= BIT6;
    P5OUT |= BIT6;

    //Init UART with RX and TX FIFO buffers
    static uint8_t uartA1_TX_fifo_buffer[256] = { 0 };
    circFIFO64k_t uartA1_TX_fifo;
    circFIFO_init(&uartA1_TX_fifo, uartA1_TX_fifo_buffer, 256);

    static uint8_t uartA1_RX_fifo_buffer[16] = { 0 };
    circFIFO64k_t uartA1_RX_fifo;
    circFIFO_init(&uartA1_RX_fifo, uartA1_RX_fifo_buffer, 16);

    hal_usart_channel_cfg_t uartA1_cfg = {
            .hal_api_version = HAL_API_UNKNOWN,
            .parity = HAL_USART_PARITY_NONE,
            .stopbits = HAL_USART_STOPBITS_1,
            .word_length = HAL_USART_WORD_LENGTH_8,
            .baudrate = HAL_USART_BAUDRATE_115200,
            .RXpin = (IO_t ) { .portNo = 5, .pinNo = 5 },
            .TXpin = (IO_t ) { .portNo = 5, .pinNo = 4 },
            .TX_FIFO = &uartA1_TX_fifo,
            .RX_FIFO = &uartA1_RX_fifo };

    hal_usart_init(UART_A1, &uartA1_cfg);

    SPI_B0_initialize();

    ec_MAX1231_GPIO_init();
    ec_MAX1231_init(&MAX1231_0);
    ec_MAX1231_init(&MAX1231_1);
    ec_MAX1231_init(&MAX1231_2);

    __enable_interrupt();

    __delay_cycles(200000);
    ec_MAX1231_start_all_ADC_conversions();
    while (1)
    {
        if (ec_MAX1231_check_results_available() == ADC_ready) {
            hal_usart_write(UART_A1, ADC0_N_channels*2, (uint8_t *)"----------ADC0-----------|");
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            hal_usart_write(UART_A1, ADC0_N_channels * 2, (uint8_t *)MAX1231_0.data);
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            hal_usart_write(UART_A1, ADC1_N_channels*2, (uint8_t *)"----------ADC1-----------|");
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            hal_usart_write(UART_A1, ADC1_N_channels * 2, (uint8_t *)MAX1231_1.data);
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            hal_usart_write(UART_A1, ADC2_N_channels*2, (uint8_t *)"----------ADC2-----------|");
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            hal_usart_write(UART_A1, ADC2_N_channels*2, (uint8_t *)MAX1231_2.data);
            //hal_usart_write(UART_A1, 2, (uint8_t *)"\r\n");
            __delay_cycles(200000);
            ec_MAX1231_start_all_ADC_conversions();
        }
    }
}
