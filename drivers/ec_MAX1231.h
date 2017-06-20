#ifndef _EC_MAX1231_H
#define _EC_MAX1231_H

#include "echal_spi.h"
#include <MSP430.h>
#include <stdint.h>

#define ADC0_N_channels 13
#define ADC1_N_channels 13
#define ADC2_N_channels 13

#define REG_OFFSET(x) (*(uint32_t *)((uint32_t)x))

/*typedef enum {
    CLK_MODE00,
    CLK_MODE01,
    CLK_MODE10,
    CLK_MODE11
} clock_mode;*/

typedef enum {
    ADC0,
    ADC1,
    ADC2
} ADC_index;

typedef enum {
    ADC_not_ready,
    ADC_ready
} ADC_data_ready;

typedef struct {
    hal_spi_channel_cfg_t *spi_conf; ///< SPI conf

    // Need GPIO.c first for proper pin configuration
    /*IO_t nCNVST_pin; ///< data is requested if nCNVST is pulled low
    IO_t nEOC_pin; ///< data is valid if EOC pulls low. Triggers interrupt
    IO_t nCS_pin; ///< Active-Low Chip Select Input. When CS is low, the serial interface is enabled.*/

    uint16_t *data; ///< buffer where MAX1231 writes data. ## max size 16 ##
    ADC_index index;
    uint8_t N_channels; ///< number of channels to be read
    ADC_data_ready ready;
} ec_MAX1231_cfg_t;

extern uint16_t all_ADC_readings[];

extern ec_MAX1231_cfg_t MAX1231_0, MAX1231_1, MAX1231_2;

void ec_MAX1231_GPIO_init();

//init GPIO before
void ec_MAX1231_init(
        ec_MAX1231_cfg_t *cfg_table
);

// init all ADCs before use
void ec_MAX1231_start_all_ADC_conversions();

ADC_data_ready ec_MAX1231_check_results_available();

void ec_MAX1231_reset(
        ec_MAX1231_cfg_t *cfg_table
);

void ec_MAX1231_clear_FIFO(
        ec_MAX1231_cfg_t *cfg_table
);

void ec_MAX1231_read_results(
        ec_MAX1231_cfg_t *cfg_table, ///< [in] to indicate which ADC to read
        uint16_t *result ///< [out] ADC result(s)
);

#endif
