#include "ec_MAX1231.h"
#include <stdint.h>

void ec_MAX1231_init(ec_MAX1231_cfg_t *cfg_table)
{
    //Configure pins - hardcode

    //configure pin interrupts - hardcode

    //configure SPI

    //SPI writes to init MAX1231
    // Write to conversion register
    uint8_t CHSEL_bits = 0000;  // 4 bits // AIN0
    uint8_t SCAN_bits = 00;     // 2 bits // Scans channels 0 through N.
    uint8_t TEMP_bits = 1;      // 1 bit  // reads temperature
    spi_write( (uint8_t) ( (1 << 7) | (CHSEL_bits << 3) | (SCAN_bits << 1) | (TEMP_bits) );

    // Write to setup register
    uint8_t CKSEL_bits = 01;    // 2 bits // Acquisition externally timed through CNVST
    uint8_t REFSEL_bits = 10;   // 2 bits // Reference always on; no wake-up delay.
    uint8_t DIFFSEL_bits = 10;  // 2 bits // One byte of data follows the setup byte and is written to the unipolar mode register.
    spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );

    // Write to unipolar / bipolar config register
    spi_write( (uint8_t) (0b11111111) ); // AIN0...AIN15 are set to unipolar mode
    spi_write( (uint8_t) ( (0b01 << 6) | (CKSEL_bits << 4) | (REFSEL_bits << 2) | (DIFFSEL_bits) );
    uint8_t DIFFSEL_bits = 11;  // 2 bits // One byte of data follows the setup byte and is written to the bipolar mode register
    spi_write( (uint8_t) (0b00000000) ); // none of AIN0...AIN15 are set to bipolar mode

    /*
    // Write to averaging register
    uint8_t AVGON_bits = 0;     // 1 bit  // Set to 1 to turn averaging on. Set to zero to turn averaging off.
    uint8_t NAVG_bits = 00;     // 2 bits // Configures the number of conversions for single-channel scans.
    uint8_t NSCAN_bits = 00;    // 2 bits // Single-channel scan count. (Scan mode 10 only.)
    spi_write( (uint8_t) ( (0b001 << 5) | (AVGON_bits << 4) | (NAVG_bits << 2) | (NSCAN_bits) );
    */
}

void ec_MAX1231_reset(ec_MAX1231_cfg_t *cfg_table)
{
    //reset pins

    //reset pin interrupts

    // Write reset to ADC
    spi_write(0b00010000);

    //reset SPI

}
void ec_MAX1231_clear_FIFO(ec_MAX1231_cfg_t *cfg_table)
{
    // Write reset FIFO to ADC
    spi_write(0b00011000);

}


void ec_MAX1231_read_results(ec_MAX1231_cfg_t *cfg_table, uint16_t *result)
{
    // maybe somehow add how many results should be read from FIFO... number ranges from 0...17
    // ADC converison nr can be set with SCAN_bits

    // pull CNVST low for at least 40ns

    // wait for EOC to go low

    // pull CS low and read data with spi
}

// Interrupt driven ADC reading (w DMA)
