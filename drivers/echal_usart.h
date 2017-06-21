#include <stdint.h>
#include "echal_common.h"
#include "echal_circFIFO64_k.h"

/**
 * Offset address values of registers from base address
 */
#define UCAxCTLW0_offset    0x00
#define UCAxCTL0_offset     0x01
#define UCAxCTL1_offset     0x00
#define UCAxCTLW1_offset    0x02
#define UCAxBRW_offset      0x06
#define UCAxBR0_offset      0x06
#define UCAxBR1_offset      0x07
#define UCAxMCTLW_offset    0x08
#define UCAxSTATW_offset    0x0A
#define UCAxRXBUF_offset    0x0C
#define UCAxTXBUF_offset    0x0E
#define UCAxABCTL_offset    0x10
#define UCAxIRCTL_offset    0x12
#define UCAxIRTCTL_offset   0x12
#define UCAxIRRCTL_offset   0x13
#define UCAxIE_offset       0x1A
#define UCAxIFG_offset      0x1C
#define UCAxIV_offset       0x1E

/**
 * Used to write to registers based on offset address.
 * x = base address + offset address
 */
#define REG_OFFSET(x) (*(uint32_t *)((uint32_t)x))
//#define REG_OFFSET(x) (*(x))

/**
 * USART modules available on MSP430
 */
typedef enum {
    UART_A0,
    UART_A1,
} peripheral_index;

/**
 * Used to define specific pin in a port
 */
typedef struct {
    uint8_t portNo;
    uint8_t pinNo;
} IO_t;

/**
 * Possible parity values for USART peripheral
 */
typedef enum {
    HAL_USART_PARITY_NONE, ///< Parity bit is not used
    HAL_USART_PARITY_ODD, ///< Odd parity is used
    HAL_USART_PARITY_EVEN ///< Even parity is used
} hal_usart_parity_t;

/**
 * Most frequently used baudrates
 */
typedef enum {
    HAL_USART_BAUDRATE_9600,
    HAL_USART_BAUDRATE_57600,
    HAL_USART_BAUDRATE_115200,
    HAL_USART_BAUDRATE_230400
} hal_usart_baudrate_t;

/**
 * Possible stop bit values for USART peripheral
 */
typedef enum {
    HAL_USART_STOPBITS_1, ///< Usart stop bit 1
    HAL_USART_STOPBITS_2 ///< Usart stop bit 2
} hal_usart_stopbit_t;

/**
 * Possible word lengths for USART peripheral
 */
typedef enum {
    HAL_USART_WORD_LENGTH_7, ///< Peripheral word length 7
    HAL_USART_WORD_LENGTH_8, ///< Peripheral word length 8
} hal_usart_word_length_t;

/**
 * Error codes that can be returned by USART module functions
 */
typedef enum {
    HAL_USART_ENABLED=64, ///< Requested task impossible while peripheral in question is enabled
    HAL_USART_OK, ///< All function calls must check if this code is set
    HAL_USART_DISABLED, ///< Requested task impossible while peripheral in question is disabled
    HAL_USART_GPIO_ERROR, ///< GPIO tied with USART peripheral returned error state
    HAL_USART_BUFFER_DEPLETED, ///< Not enough data to be read
    HAL_USART_BUFFER_FULL, ///< Data requested to be written didn't fit into buffer
} hal_usart_errors_t;

/**
 * USART peripheral config struct
 */
typedef struct {
    hal_api_version_t hal_api_version;
    hal_usart_parity_t parity : 2;
    hal_usart_stopbit_t stopbits : 2;
    hal_usart_word_length_t word_length : 2;
    uint32_t baudrate;
    IO_t RXpin;
    IO_t TXpin;
    circFIFO64k_t *TX_FIFO;
    circFIFO64k_t *RX_FIFO;
} hal_usart_channel_cfg_t;

/**
 * Initializes a USART peripheral.
 */
hal_usart_errors_t hal_usart_init(
    uint8_t peripheral_index, ///< [in] specify USART (sometimes called channel)
    hal_usart_channel_cfg_t *cfg_table ///< [in] pointer to a struct containing configuration table
);

/**
 * Sets all configurations to reset values.
 */
hal_usart_errors_t hal_usart_reset(
    uint8_t peripheral_index ///< [in] specify USART (sometimes called channel)
);

/**
 * Sends a batch of data over USART using interrupts. Function copies data to TX buffer.
 *
 * Buffer must have enough free space to accommodate word_count of words, otherwise returns an error
 * and won't write data to TX buffer.
 */
hal_usart_errors_t hal_usart_write(
    uint8_t peripheral_index, ///< [in] specify USART (sometimes called channel)
    uint16_t word_count, ///< [in] number of words to send
    uint8_t *data ///< [in] pointer to beginning of data being sent
);

/**
 * Receives a batch of data via USART recieved by interrupts. Function copies data from RX buffer.
 *
 * Word_count may be greater that RX buffer has data in it, but it will return
 * BUFFER_DEPLETED error.
 */
hal_usart_errors_t hal_usart_read(
    uint8_t peripheral_index, ///< [in] specify USART (sometimes called channel)
    uint16_t word_count, ///< [in] number of words to read before returning
    uint8_t *data, ///< [out] pointer to a buffer to be filled
    uint16_t *output_len ///< [out] number of words in output data buffer
);
