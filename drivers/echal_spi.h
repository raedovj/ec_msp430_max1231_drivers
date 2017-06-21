
#ifndef _ECHAL_SPI_H
#define _ECHAL_SPI_H

#include <stdint.h>
#include "echal_common.h"

/**
 * Error codes that can be returned by SPI module functions
 */
typedef enum {
    HAL_SPI_OK, ///< All function calls must check if this code is set
    HAL_SPI_UNKNOWN_ERROR, ///< Fallback error if no more information can be returned
    HAL_SPI_NOT_IMPLEMENTED, ///< Both incomplete implementation and lacking hardware support
    HAL_SPI_ENABLED, ///< Requested task impossible while peripheral in question is enabled
    HAL_SPI_DISABLED, ///< Requested task impossible while peripheral in question is disabled
    HAL_SPI_GPIO_ERROR, ///< GPIO tied with SPI peripheral returned error state
    HAL_SPI_IO_PORT_INCOMPATIBLE, ///< Chosen IO port for SOMI/SIMO is not compatible with current SPI port
    HAL_SPI_PORT_INVALID
} hal_spi_errors_t;


/**
 * Possible SPI ports
 */
typedef enum
{
    HAL_SPI_A0, ///< SPI port A0
    HAL_SPI_A1, ///< SPI port A1
    HAL_SPI_B0, ///< SPI port B0
    HAL_SPI_B1  ///< SPI port B1
} hal_spi_port_t;

/**
 * Possible SPI IO ports
 */
typedef enum
{
    HAL_SPI_A0_P2, ///< SPI A0 SOMI/SIMO in port 2
    HAL_SPI_A0_P4, ///< SPI A0 SOMI/SIMO in port 4
    HAL_SPI_A1_P3, ///< SPI A1 SOMI/SIMO in port 3
    HAL_SPI_A1_P5, ///< SPI A1 SOMI/SIMO in port 5
    HAL_SPI_B0_P1, ///< SPI B0 SOMI/SIMO in port 1
    HAL_SPI_B1_P3, ///< SPI B1 SOMI/SIMO in port 3
    HAL_SPI_B1_P4  ///< SPI B1 SOMI/SIMO in port 4
} hal_spi_io_port_t;

/**
 * Clock polarity
 */
typedef enum {
    HAL_SPI_POLARITY_IDLE_0, ///< Clock to 0 when idle
    HAL_SPI_POLARITY_IDLE_1 ///< Clock to 1 when idle
} hal_spi_polarity_t;

/**
 * Clock source select
 */
typedef enum {
    HAL_SPI_CLOCK_SOURCE_ALCK, ///< ALCK in master mode
    HAL_SPI_CLOCK_SOURCE_SMCLK, ///< SMCLK in master mode
    HAL_SPI_CLOCK_SOURCE_UCXCLK ///< UCxCLK in slave mode
} hal_spi_clock_source_t;

/**
 * Clock phase
 */
typedef enum {
    HAL_SPI_PHASE_FIRST_TRANSITION, ///< The first clock transition is the first data capture edge
    HAL_SPI_PHASE_SECOND_TRANSITION ///< The second clock transition is the first data capture edge
} hal_spi_phase_t;

/**
 * Possible (most significant, least significant) bit order values for SPI peripheral
 */
typedef enum {
    HAL_SPI_MSB_FIRST, ///< Bit order most significant first
    HAL_SPI_LSB_FIRST ///< Bit order least significant first
} hal_spi_bit_order_t;

/**
 * Possible slave read data modes
 */
typedef enum {
    HAL_SPI_READ_RISING_EDGE, ///< Read data mode rising edge
    HAL_SPI_READ_FALLING_EDGE ///< Read data mode falling edge
} hal_spi_data_read_mode_t;

/**
 * Possible communication modes
 */
typedef enum {
        HAL_SPI_MODE_3_WIRE, ///<
        HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_1, ///<
        HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_0
} hal_spi_communication_mode_t;

/**
 * Software slave management
 */
typedef enum {
        HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_DISABLED, ///< Software slave managent disable
        HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_ENABLED ///< Software slave managent enable
} hal_spi_software_slave_management_t;

/**
 * Possible internal slave select bit values
 */
typedef enum {
    HAL_SPI_INTERNAL_SS_0, ///< Internal slave select bit value 0
    HAL_SPI_INTERNAL_SS_1 ///< Internal slave select bit value 1
} hal_spi_internal_ss_t;

/**
 * Master selection
 */
typedef enum {
    HAL_SPI_SLAVE_CONFIGURATION, ///< Slave configuration
    HAL_SPI_MASTER_CONFIGURATION ///< Master configuration
} hal_spi_master_select_t;

/**
 * Possible data sizes
 */
typedef enum {
  HAL_SPI_DATA_SIZE_4, ///< Data size 4
  HAL_SPI_DATA_SIZE_5, ///< Data size 5
  HAL_SPI_DATA_SIZE_6, ///< Data size 6
  HAL_SPI_DATA_SIZE_7, ///< Data size 7
  HAL_SPI_DATA_SIZE_8, ///< Data size 8
  HAL_SPI_DATA_SIZE_9, ///< Data size 9
  HAL_SPI_DATA_SIZE_10, ///< Data size 10
  HAL_SPI_DATA_SIZE_11, ///< Data size 11
  HAL_SPI_DATA_SIZE_12, ///< Data size 12
  HAL_SPI_DATA_SIZE_13, ///< Data size 13
  HAL_SPI_DATA_SIZE_14, ///< Data size 14
  HAL_SPI_DATA_SIZE_15, ///< Data size 15
  HAL_SPI_DATA_SIZE_16 ///< Data size 16
} hal_spi_data_size_t;

/**
 * Select character length
 */
typedef enum {
    HAL_SPI_CHARACTER_LENGTH_7_BIT, ///< Set character length to 7 bits
    HAL_SPI_CHARACTER_LENGTH_8_BIT ///< Set character length to 8 bits
} hal_spi_character_length_t;

/**
 * Slave select output enable
 */
typedef enum {
    HAL_SPI_SS_OUTPUT_DISABLED, ///< Slave output disable
    HAL_SPI_SS_OUTPUT_ENABLED ///< Slave output enable
} hal_spi_ss_output_t;


typedef enum {
    HAL_SPI_SYNCRONUS_MODE_ENBLED,
    HAL_SPI_SYNCRONUS_MODE_DISABLED
} hal_spi_synchronus_mode_t;

/**
 * SPI peripheral config struct
 */
typedef struct {
    hal_api_version_t hal_api_version;
    hal_spi_port_t port;
    hal_spi_io_port_t io_port;
    hal_spi_phase_t phase;
    hal_spi_polarity_t polarity;
    hal_spi_bit_order_t bit_order;
    hal_spi_character_length_t character_length;
    hal_spi_master_select_t master;
    hal_spi_communication_mode_t communication_mode;
    hal_spi_synchronus_mode_t synchronus_mode;
    hal_spi_clock_source_t clock_source;
    hal_spi_software_slave_management_t software_slave_management;
    uint32_t prescaler;
    volatile unsigned int * base_address;
    uint8_t extra_offset;
} hal_spi_channel_cfg_t;

static hal_spi_channel_cfg_t hal_spi_channel_cfg_B1 = {
    .hal_api_version = HAL_API_UNKNOWN,
    .port = HAL_SPI_B1,
    .io_port = HAL_SPI_B1_P3,
    .phase = HAL_SPI_PHASE_FIRST_TRANSITION,
    .polarity = HAL_SPI_POLARITY_IDLE_0,
    .bit_order = HAL_SPI_MSB_FIRST,
    .character_length = HAL_SPI_CHARACTER_LENGTH_8_BIT,
    .master = HAL_SPI_MASTER_CONFIGURATION,
    .communication_mode = HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_0,
    .synchronus_mode = HAL_SPI_SYNCRONUS_MODE_ENBLED,
    .clock_source = HAL_SPI_CLOCK_SOURCE_SMCLK,
    .software_slave_management = HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_ENABLED,
    .prescaler = 32
};

static hal_spi_channel_cfg_t hal_spi_channel_cfg_B0 = {
    .hal_api_version = HAL_API_UNKNOWN,
    .port = HAL_SPI_B0,
    .io_port = HAL_SPI_B0_P1,
    .phase = HAL_SPI_PHASE_SECOND_TRANSITION,
    .polarity = HAL_SPI_POLARITY_IDLE_0,
    .bit_order = HAL_SPI_MSB_FIRST,
    .character_length = HAL_SPI_CHARACTER_LENGTH_8_BIT,
    .master = HAL_SPI_MASTER_CONFIGURATION,
    .communication_mode = HAL_SPI_MODE_3_WIRE,
    .synchronus_mode = HAL_SPI_SYNCRONUS_MODE_ENBLED,
    .clock_source = HAL_SPI_CLOCK_SOURCE_UCXCLK,
    .software_slave_management = HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_DISABLED,
    .prescaler = 32
};

static hal_spi_channel_cfg_t hal_spi_channel_cfg_A1 = {
    .hal_api_version = HAL_API_UNKNOWN,
    .port = HAL_SPI_A1,
    .io_port = HAL_SPI_A1_P5,
    .phase = HAL_SPI_PHASE_FIRST_TRANSITION,
    .polarity = HAL_SPI_POLARITY_IDLE_0,
    .bit_order = HAL_SPI_MSB_FIRST,
    .character_length = HAL_SPI_CHARACTER_LENGTH_8_BIT,
    .master = HAL_SPI_MASTER_CONFIGURATION,
    .communication_mode = HAL_SPI_MODE_4_WIRE_SLAVE_ACTIVE_0,
    .synchronus_mode = HAL_SPI_SYNCRONUS_MODE_ENBLED,
    .clock_source = HAL_SPI_CLOCK_SOURCE_SMCLK,
    .software_slave_management = HAL_SPI_SOFTWARE_SLAVE_MANAGEMENT_DISABLED,
    .prescaler = 32
};

hal_spi_errors_t hal_spi_init(hal_spi_channel_cfg_t *spi_cfg);
hal_spi_errors_t hal_spi_write(hal_spi_channel_cfg_t *spi_cfg, char c);
hal_spi_errors_t hal_spi_read(hal_spi_channel_cfg_t *table, uint8_t *arr, uint8_t bytes);

#endif
