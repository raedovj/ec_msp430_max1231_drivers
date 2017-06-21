/*
 * spi.h
 *
 *  Created on: 03.01.2015
 *      Author: Martin Põder <martin.poder@estcube.eu>
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#define SPI_PORT_ADC 0
#define SPI_PORT_MISC 1

void SPI_B0_initialize(); //ADC
void SPI_B1_initialize(); //MISC
uint8_t SPI_master_swapData(uint8_t port, uint8_t data);
uint8_t SPI_checkBusy(uint8_t port);

#endif /* SPI_H_ */
