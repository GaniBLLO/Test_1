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

#define RX_DATA_SIZE 2
#define TX_DATA_SIZE 2
//------------------------SPI CTRL PINS
#define CS_LOW  GPIOE->BSRR |= GPIO_BSRR_BR7;
#define CS_HIGH GPIOE->BSRR |= GPIO_BSRR_BS7;

#define RST_OFF  GPIOE->BSRR |= GPIO_BSRR_BR9;
#define RST_ON	 GPIOE->BSRR |= GPIO_BSRR_BS9;

#define DEFAULT_SPI_SETTINGS 	0x0
//------------------------SPI STRUCT
typedef enum {
	SPI_MODE_SLAVE = 0x0,         		// Slave mode
	SPI_MODE_MASTER = SPI_CR1_MSTR  	// Master mode
} SPI_MasterSlave;

typedef enum {
	SPI_OFF = 0, SPI_ON
} SPI_WorkState;

typedef enum {
	SPI_DATA_SIZE_8BIT = 0x0,			//8 bit frame
	SPI_DATA_SIZE_16BIT = SPI_CR1_DFF	//16 bit frame
} SPI_DataTxRxSize;

typedef enum {
	SPI_MODE_0 = 0x0,  	// CPOL=0, CPHA=0
	SPI_MODE_1,      	// CPOL=0, CPHA=1
	SPI_MODE_2,      	// CPOL=1, CPHA=0
	SPI_MODE_3       	// CPOL=1, CPHA=1
} SPI_PhasePolarity;

typedef enum {
	SPI_NSS_MODE_0 = 0x0,  						//Hardware
	SPI_NSS_MODE_1 = (SPI_CR1_SSM | SPI_CR1_SSI)  			//Software (need CS)
} SPI_CS_Setting;

typedef enum {
	SPI_DATA_MSB_FIRST = 0x0,				//MSB TX first
	SPI_DATA_LSB_FIRST = SPI_CR1_LSBFIRST	//LSB TX first
} SPI_FrameFormat;

typedef enum {
	SPI_IRQ_NONE = 0x00, SPI_IRQ_RX = 0x01, SPI_IRQ_TX = 0x02,

	SPI_DMA_RX = 0x04, SPI_DMA_TX = 0x08
} SPI_IRQ_DMA_Mode;

typedef struct {

	SPI_PhasePolarity SPI_Mode;
	SPI_DataTxRxSize SPI_DataSize;
	SPI_MasterSlave SPI_MS;
	SPI_CS_Setting SPI_CS;
	SPI_FrameFormat SPI_FrstBit;
	SPI_IRQ_DMA_Mode SPI_IrqDMA;
} SPI_InitTypeDef;

typedef struct SPI_ {
	SPI_TypeDef* 	regs;
	SPI_InitTypeDef SPI_InitStruct;
	SPI_WorkState 	SPI_State;

	void (*init)(struct SPI_* driver);
	void (*StartStop)(SPI_TypeDef *SPIx, SPI_WorkState SPI_State);
	uint8_t (*Tx8bit)(SPI_TypeDef *SPIx, const uint8_t data);
	void (*Tx16bit)(SPI_TypeDef *SPIx, const uint8_t address, const int16_t data);
	void (*TxRx16bit)(SPI_TypeDef *SPIx, const uint8_t address, const int16_t data,uint16_t *RxData);
//	void (*IRQHandler)(SPI_TypeDef *SPIx);
} SPI_STRUCT;


extern SPI_STRUCT memSPI, w5500SPI;

#define SPIxSPI_InitStruct_DEFAULT   {\
						.SPI_Mode = SPI_MODE_0,\
						.SPI_DataSize = SPI_DATA_SIZE_8BIT,\
						.SPI_MS = SPI_MODE_SLAVE,\
						.SPI_CS = SPI_NSS_MODE_0,\
						.SPI_FrstBit = SPI_DATA_MSB_FIRST,\
						.SPI_IrqDMA = SPI_IRQ_NONE\
}

#define SPIx_DEFAULT(spi)	{\
						.regs = spi,\
						.SPI_InitStruct = SPIxSPI_InitStruct_DEFAULT,\
						.SPI_State = SPI_OFF,\
						.init = SPI_init,\
						.StartStop = SPI_StartStop,\
						.Tx8bit = SPI_8Tx,\
						.Tx16bit = SPI_16BitTx,\
						.TxRx16bit = SPI_16BitTxRx\
}

void SPI_init(struct SPI_* driver);
uint8_t SPI_8Tx(SPI_TypeDef *SPIx, const uint8_t data);
void SPI_StartStop(SPI_TypeDef *SPIx, SPI_WorkState SPI_State);
void SPI_16BitTx(SPI_TypeDef *SPIx, const uint8_t address, const int16_t data);
void SPI_16BitTxRx(SPI_TypeDef *SPIx, const uint8_t address, const int16_t data, uint16_t *RxData);

extern volatile uint8_t spi_rx_complete;
extern volatile uint8_t rx_counter;
extern uint8_t err_flag;
extern uint8_t err_cnt;

extern int16_t bufferOUT[TX_DATA_SIZE];
extern uint16_t bufferIN[RX_DATA_SIZE];

#endif /* INC_SPI__H_ */
