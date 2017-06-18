#include "echal_spi.h"

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
    echal_spi_cfg_t spi_conf*; ///< SPI conf

    // Need GPIO.c first for proper pin configuration
    /*IO_t nCNVST_pin; ///< data is requested if nCNVST is pulled low
    IO_t nEOC_pin; ///< data is valid if EOC pulls low. Triggers interrupt
    IO_t nCS_pin; ///< Active-Low Chip Select Input. When CS is low, the serial interface is enabled.*/

    uint16_t *data; ///< buffer where MAX1231 writes data. ## max size 16 ##
    ADC_index index;
    uint8_t N_channels; ///< number of channels to be read
} ec_MAX1231_cfg_t;

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
