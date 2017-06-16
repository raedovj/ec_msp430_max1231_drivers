#include <drivers/echal_usart.h>
#include <msp430.h>

//Hardware 16bit crc calculation
uint16_t crc16_hw(uint8_t *buf, uint8_t sz)
{
    uint8_t i;
    uint8_t ch;
    //CRCINIRES = 0xffff; // init. CRCINIRES
    for (i = 0; i < sz; i++)
    {
        ch = *(buf + i);
        CRCDI_L = ch;
    }
    return CRCINIRES;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers

    //Init UART with RX and TX FIFO buffers
    uint8_t uartA1_TX_fifo_buffer[16] = { 0 };
    circFIFO64k_t uartA1_TX_fifo;
    circFIFO_init(&uartA1_TX_fifo, uartA1_TX_fifo_buffer, 16);

    uint8_t uartA1_RX_fifo_buffer[16] = { 0 };
    circFIFO64k_t uartA1_RX_fifo;
    circFIFO_init(&uartA1_RX_fifo, uartA1_RX_fifo_buffer, 16);

    hal_usart_channel_cfg_t uartA1_cfg = {
            .hal_api_version = HAL_API_UNKNOWN, .parity = HAL_USART_PARITY_NONE,
            .stopbits = HAL_USART_STOPBITS_1,
            .word_length = HAL_USART_WORD_LENGTH_8,
            .baudrate = HAL_USART_BAUDRATE_115200,
            .RXpin = (IO_t ) { .portNo = 3, .pinNo = 5 },
            .TXpin = (IO_t ) { .portNo = 3, .pinNo = 4 },
            .TX_FIFO = &uartA1_TX_fifo,
            .RX_FIFO = &uartA1_RX_fifo };

    hal_usart_init(UART_A1, &uartA1_cfg);

    //interrupts enabled
    __bis_SR_register(GIE);

    char valja[] = "XXX|";
    char sisse[1];

    //Init CRCINIRES for CRC16 calculation
    uint16_t crc_val;
    CRCINIRES = 0xffff;

    while (1)
    {
        uint16_t pikkus = 0;
        hal_usart_read(UART_A1, 1, (uint8_t *) sisse, &pikkus);
        if (pikkus)
        {
            valja[0] = sisse[0];
            crc_val = crc16_hw((uint8_t *) sisse, 1);
            valja[1] = (uint8_t) (crc_val >> 8);
            valja[2] = (uint8_t) (crc_val);
            hal_usart_write(UART_A1, 4, (uint8_t *) valja);
        }
    }
}
