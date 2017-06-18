//#include "echal_spi.h"
#include <MSP430.h>
#include <stdint.h>

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

typedef struct {
    //hal_spi_cfg_t spi_conf*; ///< SPI conf

    // Need GPIO.c first for proper pin configuration
    /*IO_t nCNVST_pin; ///< data is requested if nCNVST is pulled low
    IO_t nEOC_pin; ///< data is valid if EOC pulls low. Triggers interrupt
    IO_t nCS_pin; ///< Active-Low Chip Select Input. When CS is low, the serial interface is enabled.*/

    uint16_t *data; ///< buffer where MAX1231 writes data. ## max size 16 ##
    ADC_index index;
    uint8_t N_channels; ///< number of channels to be read
} ec_MAX1231_cfg_t;

uint16_t MAX1231_0_result[12+1] = {0};
ec_MAX1231_cfg_t MAX1231_0 =
{
    /*.spi_conf = (echal_spi_cfg_t)
        {
        },*/
    .data = MAX1231_0_result,
    .index = ADC0,
    .N_channels = 12+1  ///< +1 temperature
};

uint16_t MAX1231_1_result[12+1] = {0};
ec_MAX1231_cfg_t MAX1231_1 =
{
    /*.spi_conf = (echal_spi_cfg_t)
        {
        },*/
    .data = MAX1231_1_result,
    .index = ADC1,
    .N_channels = 12+1  ///< +1 temperature
};

uint16_t MAX1231_2_result[12+1] = {0};
ec_MAX1231_cfg_t MAX1231_2 =
{
    /*.spi_conf = (echal_spi_cfg_t)
        {
        },*/
    .data = MAX1231_2_result,
    .index = ADC2,
    .N_channels = 12+1 ///< +1 temperature
};

void ec_MAX1231_init(
        ec_MAX1231_cfg_t *cfg_table
);

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
