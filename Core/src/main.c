/*
 * main.c
 *
 *  Created on: Mar 11, 2025
 *      Author: ganibllo
 */

#include "main.h"
/************************************************************************Defines*/
/* ----------------------- Static variables ---------------------------------*/
/************************************************************************VARs*****/
/************************************************************************Functions*/
void system_clock_168m_25m_hse(void);
void debug_usart_init(void);
void SPI_init(void);
void SPI_16BitTxRx(const uint8_t address, const int16_t data, uint16_t *RxData);
void SPI_DMA_SendData(const uint8_t address, const int16_t *data);


/***********************************************TIM_6 = 1sec*/
void oneSec_timer_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->CR1 &= ~TIM_CR1_CEN;
	//1s = 1Hz
	TIM6->PSC = 1343;
	TIM6->ARR = 62499;
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM6_DAC_IRQHandler(void) {
	if (TIM6->SR & TIM_SR_UIF) {

#if SPI_2_INTERRUPT_ENB
		SPI2->CR2 |= SPI_CR2_TXEIE;
		spi_rx_complete = 0;
		bufferOUT[0] = ADDR_0X30;
#elif SPI_2_INTERRUPT_DMA_ENB
	    SPI_DMA_SendData(ADDR_0X30, bufferOUT);
#else
		SPI_16BitTxRx(ADDR_0X30, bufferOUT[1], bufferIN);
#endif
		NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
		TIM6->SR &= ~TIM_SR_UIF;
		TIM7->CNT = 0;
		TIM7->CR1 |= TIM_CR1_CEN;
	}
}
/***********************************************TIM_7 = 200ms*/
void check200ms_timer_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	//200 ms
	TIM7->PSC = 335;
	TIM7->ARR = 49999;
	TIM7->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM7_IRQn);
}

void TIM7_IRQHandler(void){
	if (TIM7->SR & TIM_SR_UIF) {
		switch (bufferIN[0]) {
			case ADDR_0X20:
				if (bufferIN[1])
#if SPI_2_INTERRUPT_DMA_ENB
					bufferOUT[0] = 0xFF;
#else
					bufferOUT[1] = 0xFF;
#endif
				break;
			default:
				break;
		}
		spi_rx_complete = 0;
		rx_counter = 0;
		CS_HIGH;
	}
	NVIC_ClearPendingIRQ(TIM7_IRQn);
	TIM7->SR &= ~TIM_SR_UIF;
	TIM7->CR1 &= ~TIM_CR1_CEN;
}

/************************************************************************MAIN*/
int main(void) {
    system_clock_168m_25m_hse();

    debug_usart_init();
    bufferOUT[0] = 0x55;
    oneSec_timer_init();
    check200ms_timer_init();
    SPI_init();
    __enable_irq();

	while (1){
//		LED_15_ON;
	}
}

/*************************** End of file ****************************/
