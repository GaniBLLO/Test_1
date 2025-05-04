/*
 * SPI.h
 *
 *  Created on: 12 мар. 2025 г.
 *      Author: ganibllo
 */

#ifndef INC_SPI__H_
#define INC_SPI__H_

#include "stm32f4xx.h"
#include <string.h>

/*V --- SETTINGS SPI WORK--- V
 * ENB INTERRUPT 		<--Default
 * OR *
 * ENB INTERRUPT + DMA				*/

#define SPI_2_INTERRUPT_ENB			1
#define SPI_2_INTERRUPT_DMA_ENB		0

//------------------------SPI COMMANDS
#define ADDR_0X30	0x30
#define ADDR_0X20	0x20

//------------------------SPI CTRL PINS
#define CS_LOW  GPIOE->BSRR |= GPIO_BSRR_BR7;
#define CS_HIGH GPIOE->BSRR |= GPIO_BSRR_BS7;

#define RST_OFF  GPIOE->BSRR |= GPIO_BSRR_BR9;
#define RST_ON	 GPIOE->BSRR |= GPIO_BSRR_BS9;


#define RX_DATA_SIZE 2
#define TX_DATA_SIZE 2

extern volatile uint8_t spi_rx_complete;
extern volatile uint8_t rx_counter;
extern uint8_t err_flag;
extern uint8_t err_cnt;


extern int16_t bufferOUT[TX_DATA_SIZE];
extern uint16_t bufferIN[RX_DATA_SIZE];


#endif /* INC_SPI__H_ */
