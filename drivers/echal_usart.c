#include <drivers/echal_usart.h>
#include <msp430.h>
#include <math.h>

hal_usart_channel_cfg_t *uartA0_cfg;
hal_usart_channel_cfg_t *uartA1_cfg;

hal_usart_errors_t hal_usart_init(uint8_t peripheral_index,
                                  hal_usart_channel_cfg_t *cfg_table)
{

    // Determine base address of USART peripheral

    uint32_t *base_addr;
    if (peripheral_index == UART_A0)
    {
        uartA0_cfg = cfg_table;
        base_addr = (uint32_t *) &UCA0CTLW0;
    }
    else if (peripheral_index == UART_A1)
    {
        uartA1_cfg = cfg_table;
        base_addr = (uint32_t *) &UCA1CTLW0;
    }

    // Configure GPIO

    if (cfg_table->RXpin.portNo == 2 && cfg_table->RXpin.pinNo == 1)
    {
        P2SEL0 |= BIT1;
        P2SEL1 &= ~BIT1;
    }
    else if (cfg_table->RXpin.portNo == 4 && cfg_table->RXpin.pinNo == 3)
    {
        P4SEL1 |= BIT3;
        P4SEL0 &= ~BIT3;
    }
    else if (cfg_table->RXpin.portNo == 5 && cfg_table->RXpin.pinNo == 5)
    {
        P5SEL1 |= BIT5;
        P5SEL0 &= ~BIT5;
    }
    else if (cfg_table->RXpin.portNo == 3 && cfg_table->RXpin.pinNo == 5)
    {
        P3SEL0 |= BIT5;
        P3SEL1 &= ~BIT5;
    }
    else
        return HAL_USART_GPIO_ERROR;

    if (cfg_table->TXpin.portNo == 2 && cfg_table->TXpin.pinNo == 0)
    {
        P2SEL0 |= BIT0;
        P2SEL1 &= ~BIT0;
    }
    else if (cfg_table->TXpin.portNo == 4 && cfg_table->TXpin.pinNo == 2)
    {
        P4SEL0 |= BIT2;
        P4SEL1 &= ~BIT2;
    }
    else if (cfg_table->TXpin.portNo == 5 && cfg_table->TXpin.pinNo == 4)
    {
        P5SEL1 |= BIT4;
        P5SEL0 &= ~BIT4;
    }
    else if (cfg_table->TXpin.portNo == 3 && cfg_table->TXpin.pinNo == 4)
    {
        P3SEL0 |= BIT4;
        P3SEL1 &= ~BIT4;
    }
    else
        return HAL_USART_GPIO_ERROR;

    REG_OFFSET(base_addr + UCAxCTLW0_offset) = UCSWRST;   // Put eUSCI in reset

    // Configure parity

    if (cfg_table->parity == HAL_USART_PARITY_NONE)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) &= ~UCPEN;
    }
    else if (cfg_table->parity == HAL_USART_PARITY_ODD)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) |= UCPEN;
        REG_OFFSET(base_addr + UCAxCTLW0_offset) &= ~UCPAR;
    }
    else
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) |= UCPEN | UCPAR;
    }

    // Configure number of stop bits

    if (cfg_table->stopbits == HAL_USART_STOPBITS_1)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) &= ~UCSPB;
    }
    else if (cfg_table->stopbits == HAL_USART_STOPBITS_2)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) |= UCSPB;
    }

    // Configure word length

    if (cfg_table->word_length == HAL_USART_WORD_LENGTH_7)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) |= UC7BIT;
    }
    else if (cfg_table->word_length == HAL_USART_WORD_LENGTH_8)
    {
        REG_OFFSET(base_addr + UCAxCTLW0_offset) &= ~UC7BIT;
    }

    REG_OFFSET(base_addr + UCAxCTLW0_offset) |= UCSSEL__SMCLK; // Configure USART input clock to be SMCLK

    REG_OFFSET(base_addr + UCAxCTLW1_offset ) = UCGLIT_3; // Deglitch time 10b = Approximately 100 ns

    // Bits that determine baud rate and modulations

    uint16_t UCBRx_bits; //16 bits in UCAxBRW register
    uint16_t UCBRSx_bits; //8 bits in UCAxMCTLW high byte
    uint8_t UCBRFx_bits; //4 high bits in UCAxMCTLW low byte
    uint8_t UCOS16_bits; //1 low bit in UCAxMCTLW low byte

    switch (cfg_table->baudrate)
    {
    case (HAL_USART_BAUDRATE_9600):
        UCOS16_bits = 1;
        UCBRx_bits = 52;
        UCBRSx_bits = 0;
        UCBRFx_bits = 1;
        break;
    case (HAL_USART_BAUDRATE_57600):
        UCOS16_bits = 1;
        UCBRx_bits = 8;
        UCBRSx_bits = 0;
        UCBRFx_bits = 11;
        break;
    case (HAL_USART_BAUDRATE_115200):
        UCOS16_bits = 1;
        UCBRx_bits = 4;
        UCBRSx_bits = 5;
        UCBRFx_bits = 3;
        break;
    case (HAL_USART_BAUDRATE_230400):
        UCOS16_bits = 1;
        UCBRx_bits = 2;
        UCBRSx_bits = 3;
        UCBRFx_bits = 2;
        break;
    }

    /*
     // Baudrate and modulation registers could be calculated
     // but determining UCBRS modulation bits requires testing
     // which value gives least amount of error
     if (clk_div < 16)
     {
     UCOS16_bits = 0;
     UCBRx_bits = (uint16_t) clk_div;
     UCBRSx_bits = ((uint16_t) round( (clk_div - (uint16_t)clk_div ) * 8) );
     UCBRFx_bits = 0;
     }
     else
     {
     UCOS16_bits = 1;
     UCBRx_bits = (uint16_t) (clk_div / 16);
     UCBRSx_bits = 0;
     UCBRFx_bits = (uint8_t) round( ( clk_div/16 - (uint16_t)(clk_div / 16)) * 16 );
     }*/
    REG_OFFSET(base_addr + UCAxBRW_offset) = UCBRx_bits;
    REG_OFFSET(base_addr + UCAxMCTLW_offset) = UCBRSx_bits << 8
            | UCBRFx_bits << 4 | UCOS16_bits;
    REG_OFFSET(base_addr + UCAxCTLW0_offset) &= ~UCSWRST;   // Initialize eUSCI
    REG_OFFSET(base_addr + UCAxIE_offset) = UCRXIE; // Enable USCI_Ax RX interrupt
    REG_OFFSET(base_addr + UCAxIFG_offset) |= UCTXIFG; // Indicate TX buffer to be empty so interrupts can happen
    return HAL_OK;
}

hal_usart_errors_t hal_usart_reset(uint8_t peripheral_index)
{
    uint32_t *base_addr;
    hal_usart_channel_cfg_t *cfg_table;
    if (peripheral_index == UART_A0)
    {
        base_addr = (uint32_t *) &UCA0CTLW0;
        cfg_table = uartA0_cfg;
    }
    else if (peripheral_index == UART_A1)
    {
        base_addr = (uint32_t *) &UCA1CTLW0;
        cfg_table = uartA1_cfg;
    }

    // GPIO reset

    if (cfg_table->RXpin.portNo == 2 && cfg_table->RXpin.pinNo == 1)
    {
        P2SEL0 &= ~BIT1;
        P2SEL1 &= ~BIT1;
    }
    else if (cfg_table->RXpin.portNo == 4 && cfg_table->RXpin.pinNo == 3)
    {
        P4SEL1 &= ~BIT3;
        P4SEL0 &= ~BIT3;
    }
    else if (cfg_table->RXpin.portNo == 5 && cfg_table->RXpin.pinNo == 5)
    {
        P5SEL1 &= ~BIT5;
        P5SEL0 &= ~BIT5;
    }
    else if (cfg_table->RXpin.portNo == 3 && cfg_table->RXpin.pinNo == 5)
    {
        P3SEL0 &= ~BIT5;
        P3SEL1 &= ~BIT5;
    }

    if (cfg_table->TXpin.portNo == 2 && cfg_table->TXpin.pinNo == 0)
    {
        P2SEL0 &= ~BIT0;
        P2SEL1 &= ~BIT0;
    }
    else if (cfg_table->TXpin.portNo == 4 && cfg_table->TXpin.pinNo == 2)
    {
        P4SEL0 &= ~BIT2;
        P4SEL1 &= ~BIT2;
    }
    else if (cfg_table->TXpin.portNo == 5 && cfg_table->TXpin.pinNo == 4)
    {
        P5SEL1 &= ~BIT4;
        P5SEL0 &= ~BIT4;
    }
    else if (cfg_table->TXpin.portNo == 3 && cfg_table->TXpin.pinNo == 4)
    {
        P3SEL0 &= ~BIT4;
        P3SEL1 &= ~BIT4;
    }

    // Reset registers

    REG_OFFSET(base_addr + UCAxCTLW0_offset ) = (uint16_t) 0x01;
    REG_OFFSET(base_addr + UCAxCTLW1_offset ) = (uint16_t) 0x03;
    REG_OFFSET(base_addr + UCAxBRW_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxMCTLW_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxSTATW_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxABCTL_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxIRCTL_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxTXBUF_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxRXBUF_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxIE_offset ) = (uint16_t) 0x00;
    REG_OFFSET(base_addr + UCAxIFG_offset ) = (uint16_t) 0x02;
    REG_OFFSET(base_addr + UCAxIV_offset ) = (uint16_t) 0x00;

    return HAL_OK;
}

hal_usart_errors_t hal_usart_write(uint8_t peripheral_index,
                                   uint16_t word_count, uint8_t *data)
{
    circFIFO64k_t *TX_fifo_buffer;

    if (peripheral_index == UART_A0)
        TX_fifo_buffer = uartA0_cfg->TX_FIFO;
    else if (peripheral_index == UART_A1)
        TX_fifo_buffer = uartA1_cfg->TX_FIFO;

    uint16_t *base_addr;
    if (peripheral_index == UART_A0)
        base_addr = (uint16_t *) &UCA0CTLW0;
    else
        base_addr = (uint16_t *) &UCA1CTLW0;

    if (word_count <= circFIFO_size_free(TX_fifo_buffer))
    {
        circFIFO_push(TX_fifo_buffer, data, word_count);
        REG_OFFSET(base_addr + UCAxIE_offset ) |= UCTXIE; // Enable transmit interrupt
        return HAL_OK;
    }
    REG_OFFSET(base_addr + UCAxIE_offset ) |= UCTXIE; // Enable transmit interrupt
    return HAL_USART_BUFFER_FULL;
}

hal_usart_errors_t hal_usart_read(uint8_t peripheral_index, uint16_t word_count,
                                  uint8_t *data, uint16_t *output_len)
{
    circFIFO64k_t *RX_fifo_buffer;

    if (peripheral_index == UART_A0)
        RX_fifo_buffer = uartA0_cfg->RX_FIFO;
    else
        RX_fifo_buffer = uartA1_cfg->RX_FIFO;

    *output_len = circFIFO_pop(RX_fifo_buffer, data, word_count);

    if (word_count <= circFIFO_size_used(RX_fifo_buffer))
        return HAL_OK;
    else
        return HAL_USART_BUFFER_DEPLETED;
}

void uartAx_interrupt_function(uint32_t UCAxIV)
{
    circFIFO64k_t *RX_FIFO;
    circFIFO64k_t *TX_FIFO;
    uint32_t *base_addr;
    if (UCAxIV == UCA0IV)
    {
        RX_FIFO = uartA0_cfg->RX_FIFO;
        TX_FIFO = uartA0_cfg->TX_FIFO;
        base_addr = (uint32_t *) &UCA0CTLW0;
    }
    else
    {
        RX_FIFO = uartA1_cfg->RX_FIFO;
        TX_FIFO = uartA1_cfg->TX_FIFO;
        base_addr = (uint32_t *) &UCA1CTLW0;
    }
    switch (__even_in_range(UCAxIV, UCTXCPTIFG))
    {
    case USCI_NONE:
        break;
    case USCI_UART_UCRXIFG:
        if (RX_FIFO->free > 0)
        {
            RX_FIFO->data[RX_FIFO->head++] = REG_OFFSET(
                    base_addr + UCAxRXBUF_offset);
            RX_FIFO->free--;
            if (RX_FIFO->head >= RX_FIFO->size)
                RX_FIFO->head = 0;
        }
        break;
    case USCI_UART_UCTXIFG:
        if (TX_FIFO->free < TX_FIFO->size)
        {
            REG_OFFSET(base_addr + UCAxTXBUF_offset) =
                    TX_FIFO->data[TX_FIFO->tail++];
            TX_FIFO->free++;
            if (TX_FIFO->tail >= TX_FIFO->size)
                TX_FIFO->tail = 0;
        }
        else
        {
            // nothing to send
            REG_OFFSET(base_addr + UCAxIE_offset) = UCRXIE; //enable only RX interrupt
            REG_OFFSET(base_addr + UCAxIFG_offset) |= UCTXIFG; // set interrupt flag 1, so interrupt can happen again if enabled
        }
        break;
    case USCI_UART_UCSTTIFG:
        break;
    case USCI_UART_UCTXCPTIFG:
        break;
    }
}
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    uartAx_interrupt_function(UCA0IV);
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    uartAx_interrupt_function(UCA1IV);
}
