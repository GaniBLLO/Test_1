#include <SPI_.h>

#if SPI_2_INTERRUPT_DMA_ENB
	int16_t bufferOUT[RX_DATA_SIZE] __attribute__((section(".fast")));
	uint16_t bufferIN[TX_DATA_SIZE] __attribute__((section(".fast")));
#else
	uint16_t bufferIN[RX_DATA_SIZE] = {0};
	int16_t bufferOUT[TX_DATA_SIZE] = {-16000, 32000};
#endif

volatile uint8_t spi_rx_complete = 0;
volatile uint8_t rx_counter = 0;
volatile uint8_t tx_counter = 0;
uint8_t err_flag = 0;
uint8_t err_cnt = 0;

void DMA1_Stream3_IRQHandler(void){
	if((DMA1_Stream3) && (DMA_LISR_HTIF3)){
		DMA1->LIFCR |= DMA_LIFCR_CHTIF3;
	}
	if((DMA1_Stream3) && (DMA_LISR_TCIF3)){
		DMA1->LIFCR |= DMA_LIFCR_CTCIF3;
	}
	NVIC_ClearPendingIRQ(DMA1_Stream3_IRQn);
}

void DMA1_Stream4_IRQHandler(void){
	if((DMA1_Stream4) && (DMA_HISR_HTIF4)){
		DMA1->HIFCR |= DMA_HIFCR_CHTIF4;
	}
	if((DMA1_Stream4) && (DMA_HISR_TCIF4)){
		DMA1->HIFCR |= DMA_HIFCR_CTCIF4;
	}
	NVIC_ClearPendingIRQ(DMA1_Stream4_IRQn);
}

void SPI2_IRQHandler(void) {
	if (READ_BIT(SPI2->SR, SPI_SR_RXNE)) {
		uint16_t data = ((uint16_t)SPI2->DR) & 0xff;
		TIM7->CNT = 0;
		if (rx_counter < RX_DATA_SIZE){
			bufferIN[rx_counter++] = data;
			if (rx_counter >= RX_DATA_SIZE) {
				spi_rx_complete = 1;
				rx_counter = 0;
				tx_counter = 0;
				SPI2->CR2 &= ~SPI_CR2_RXNEIE;
				SPI2->CR2 &= ~SPI_CR2_TXEIE;
				CS_HIGH;
			}else{
				TIM7->CR1 |= TIM_CR1_CEN;
				SPI2->CR2 &= ~SPI_CR2_RXNEIE;
				SPI2->CR2 |= SPI_CR2_TXEIE;
			}
		}else{
			SPI2->CR2 &= ~SPI_CR2_RXNEIE;
			SPI2->CR2 |= SPI_CR2_TXEIE;
		}

	}else if (READ_BIT(SPI2->SR, SPI_SR_TXE)){
		CS_LOW;
		SPI2->DR = bufferOUT[tx_counter++];
		SPI2->CR2 &= ~SPI_CR2_TXEIE;
		SPI2->CR2 |= SPI_CR2_RXNEIE;
	}

	NVIC_ClearPendingIRQ(SPI2_IRQn);
}


void init_spi_gpio(void){

	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);
	/*PC3 - MOSI
	 Alternative func. = AF7(0111)
	 Very high speed
	 Pull-down*/
	GPIOC->MODER &= ~GPIO_MODER_MODER3;
	GPIOC->MODER |= GPIO_MODER_MODER3_1;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD3_1;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
	GPIOC->AFR[0] |= (0x5 << GPIO_AFRL_AFSEL3_Pos);

	/*PC2 - MISO
	 Alternative func. = AF7(0111)
	 Very high speed
	 Pull-down*/
	GPIOC->MODER &= ~GPIO_MODER_MODER2;
	GPIOC->MODER |= GPIO_MODER_MODER2_1;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD2_1;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR2;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
	GPIOC->AFR[0] |= (0x5 << GPIO_AFRL_AFSEL2_Pos);

	/*PB10 - SCK
	 Alternative func. = AF7(0111)
	 Very high speed
	 Pull-down*/
	GPIOB->MODER &= ~GPIO_MODER_MODER10;
	GPIOB->MODER |= GPIO_MODER_MODER10_1;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD10_1;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR10;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;
	GPIOB->AFR[1] |= (0x5 << GPIO_AFRH_AFSEL10_Pos);

	/*PE7 - CS
	 Output mode
	 Push-pull
	 Very high speed
	 Pull-down*/
	GPIOE->MODER |= GPIO_MODER_MODE7_0;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT7;
	GPIOE->PUPDR |= GPIO_PUPDR_PUPD7_1;
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR7;
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	CS_HIGH;

	/*PE9 - RST
	 Output mode
	 Push-pull
	 Very high speed
	 Pull-down*/
	GPIOE->MODER |= GPIO_MODER_MODE9_0;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT9;
	GPIOE->PUPDR |= GPIO_PUPDR_PUPD9_1;
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	RST_ON;
}

void init_spi_DMARx(void){
	//DMA SPI2_RX Channel_0 Stream_3
	DMA1_Stream3->PAR = (uint32_t)&(SPI2->DR);
	DMA1_Stream3->M0AR = (uint32_t)bufferIN;
	DMA1_Stream3->NDTR = 2;

	DMA1_Stream3->FCR &= ~DMA_SxFCR_DMDIS;
	DMA1_Stream3->CR &= ~DMA_SxCR_CHSEL;

	//Single transfer
	DMA1_Stream3->CR &= ~(DMA_SxCR_PBURST | DMA_SxCR_MBURST);
	//Double buffer mode = disable
	DMA1_Stream3->CR &= ~DMA_SxCR_DBM;
	//Priority level = Very high
	DMA1_Stream3->CR &= ~DMA_SxCR_PL;
	DMA1_Stream3->CR |= DMA_SxCR_PL;
	//Mem data size 16bits (HALF WORD)
	DMA1_Stream3->CR &= ~DMA_SxCR_MSIZE;
	DMA1_Stream3->CR |= DMA_SxCR_MSIZE_0;
	//Peripheral data size 16bits (HALF WORD)
	DMA1_Stream3->CR &= ~DMA_SxCR_PSIZE;
	DMA1_Stream3->CR |= DMA_SxCR_PSIZE_0;
	//Memory address pointer is inc
	DMA1_Stream3->CR &= ~DMA_SxCR_MINC;
	DMA1_Stream3->CR |= DMA_SxCR_MINC;
	//Peripheral address pointer is inc
	DMA1_Stream3->CR &= ~DMA_SxCR_PINC;
//	DMA1_Stream3->CR |= DMA_SxCR_PINC;
	//Cycle data RX transfer
	DMA1_Stream3->CR &= ~DMA_SxCR_CIRC;
	DMA1_Stream3->CR |= DMA_SxCR_CIRC;
	//PeriphToMem
	DMA1_Stream3->CR &= ~DMA_SxCR_DIR;

	//Transfer complete interrupt enable
	DMA1_Stream3->CR &= ~DMA_SxCR_TCIE;
	DMA1_Stream3->CR |= DMA_SxCR_TCIE;
	NVIC_SetPriority(DMA1_Stream3_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}


void init_spi_DMATx(void){
	//DMA SPI2_TX Channel_0 Stream_4
	DMA1_Stream4->PAR = (uint32_t)&SPI2->DR;
	DMA1_Stream4->M0AR = (uint32_t)bufferOUT;
	DMA1_Stream4->NDTR = 2;

	DMA1_Stream4->FCR &= ~DMA_SxFCR_DMDIS;
	DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;

	//Single transfer
	DMA1_Stream4->CR &= ~(DMA_SxCR_PBURST | DMA_SxCR_MBURST);
	//Double buffer mode = disable
	DMA1_Stream4->CR &= ~DMA_SxCR_DBM;
	//Priority level = Very high
	DMA1_Stream4->CR &= ~DMA_SxCR_PL;
	DMA1_Stream4->CR |= DMA_SxCR_PL;
	//Mem data size 16bits (HALF WORD)
	DMA1_Stream4->CR &= ~DMA_SxCR_MSIZE;
	DMA1_Stream4->CR |= DMA_SxCR_MSIZE_0;
	//Peripheral data size 16bits (HALF WORD)
	DMA1_Stream4->CR &= ~DMA_SxCR_PSIZE;
	DMA1_Stream4->CR |= DMA_SxCR_PSIZE_0;
	//Memory address pointer is increment
	DMA1_Stream4->CR &= ~DMA_SxCR_MINC;
	DMA1_Stream4->CR |= DMA_SxCR_MINC;
	//Peripheral address pointer is increment
	DMA1_Stream4->CR &= ~DMA_SxCR_PINC;
//	DMA1_Stream4->CR |= DMA_SxCR_PINC;

	DMA1_Stream4->CR &= ~DMA_SxCR_CIRC;
	//MemToPeriph
	DMA1_Stream4->CR &= ~DMA_SxCR_DIR;
	DMA1_Stream4->CR |= DMA_SxCR_DIR_0;
	//Transfer complete interrupt enable
	DMA1_Stream4->CR &= ~DMA_SxCR_TCIE;
	DMA1_Stream4->CR |= DMA_SxCR_TCIE;

	NVIC_SetPriority(DMA1_Stream4_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}

void DISABLE_SPI_DMA(void){
	DMA1_Stream3->CR &= ~DMA_SxCR_EN;
	DMA1_Stream4->CR &= ~DMA_SxCR_EN;
}

void ENABLE_SPI_DMA(void){

	DMA1_Stream3->CR |= DMA_SxCR_EN;
	DMA1_Stream4->CR |= DMA_SxCR_EN;

}


void init_spi_dma(void){
	//Enb DMA1 RCC
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	//DMA SPI2_RX/TX in Channel_0
	DISABLE_SPI_DMA();
	init_spi_DMARx();
	init_spi_DMATx();
	ENABLE_SPI_DMA();

}


//MASTER_SPI2
void SPI_init(void) {

	init_spi_gpio();
	//tick SPI bus & GPIOa
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	//APB1 = 42Mhz => /8 = 4Mhz
	SPI2->CR1 &= ~SPI_CR1_BR;
	SPI2->CR1 |= SPI_CR1_BR_1;
	//Second front signal detect
	SPI2->CR1 |= SPI_CR1_CPHA;
	SPI2->CR1 |= SPI_CR1_CPOL;
	//Master
	SPI2->CR1 |= SPI_CR1_MSTR;
	//Frame 16 bit
	SPI2->CR1 &= ~SPI_CR1_DFF;
	SPI2->CR1 |= SPI_CR1_DFF;
	//Internal CS output
	SPI2->CR1 |= SPI_CR1_SSM;
	SPI2->CR1 |= SPI_CR1_SSI;

	//Enb SPI
#if SPI_2_INTERRUPT_ENB
	NVIC_SetPriority(SPI2_IRQn, 5);
	NVIC_EnableIRQ(SPI2_IRQn);
#endif
#if SPI_2_INTERRUPT_DMA_ENB
	SPI2->CR2 |= SPI_CR2_RXDMAEN;
	SPI2->CR2 |= SPI_CR2_TXDMAEN;
	init_spi_dma();
#endif
	SPI2->CR1 |= SPI_CR1_SPE;
}


uint8_t SPI_8Tx(const uint8_t data) {

	SPI2->DR = data;
	while ((SPI2->SR & SPI_SR_TXE) == 0);
	while ((SPI2->SR & SPI_SR_RXNE) == 0);

	return (uint8_t) SPI2->DR;
}


void SPI_16BitTx(const uint8_t address, const int16_t data){

	uint16_t msg_array[TX_DATA_SIZE] = {0};
	msg_array[0] = (uint16_t)(address & 0x00ff);
	msg_array[1] = (uint16_t)data & 0xffff;
	CS_LOW;
	for (int i = 0; i < TX_DATA_SIZE; ++i){
		SPI2->DR = msg_array[i];
		while((SPI2->SR & SPI_SR_TXE) == 0);
	}
	CS_HIGH;

}

void SPI_DMA_SendData(const uint8_t address, const int16_t *data){

	DISABLE_SPI_DMA();

	uint16_t msg_array[16] = {0};
	msg_array[0] = (uint16_t)(address & 0x00ff);
	msg_array[1] = (uint16_t)(*data) & 0xffff;
	for (int i = 2; i < 16; ++i){
		msg_array[i] = i;
	}

	DMA1_Stream4->PAR = (uint32_t)&(SPI2->DR);
	DMA1_Stream4->M0AR = (uint32_t)&(msg_array);
	DMA1_Stream4->NDTR = 2;
	CS_LOW;
	ENABLE_SPI_DMA();
}

void SPI_16BitTxRx(const uint8_t address, const int16_t data, uint16_t *RxData){

	uint16_t msg_array[TX_DATA_SIZE] = {0};
	msg_array[0] = (uint16_t)(address & 0x00ff);
	msg_array[1] = (uint16_t)data & 0xffff;

	CS_LOW;
	for (int i = 0; i < TX_DATA_SIZE; ++i){
		SPI2->DR = (uint16_t)msg_array[i];
		while((SPI2->SR & SPI_SR_TXE) == 0);
		while ((SPI2->SR & SPI_SR_RXNE) == 0);
		RxData[i] = (uint16_t)SPI2->DR;
	}
	CS_HIGH;
}


