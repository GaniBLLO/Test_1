#include "stm32f4xx.h"

void debug_usart_init(void) {
  /* Using USART1 on PA9, PA10, tx only in polling mode (no interrupts) */

  /* Enable PORT E clock */
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
  // TX - PA9, AF7
  SET_BIT(GPIOA->MODER, GPIO_MODER_MODE9_1);
  SET_BIT(GPIOA->AFR[1], GPIO_AFRH_AFSEL9_0 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_2);
  // RX - PA10, AF7
  SET_BIT(GPIOA->MODER, GPIO_MODER_MODE10_1);
  SET_BIT(GPIOA->AFR[1], GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2);

  // Init USART1 module
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
  /*TX/RX_baundrate = (84MHz / 16)/115200 = 45.57; 45 = 0x2D; 57 = 0x9*/
  USART1->BRR = 0x2D9;

  // Tx enable
  SET_BIT(USART1->CR1, USART_CR1_TE);
//  SET_BIT(USART1->CR1, USART_CR1_TCIE);
  // 8 bit, no parity, 1 stop
  CLEAR_BIT(USART1->CR1, USART_CR1_M | USART_CR1_PCE);
  CLEAR_BIT(USART1->CR2, USART_CR2_STOP);

  //Enb interrupt & USART
  NVIC_EnableIRQ(USART1_IRQn);
  // enable USART
  SET_BIT(USART1->CR1, USART_CR1_UE);
}


void RS_485_init(void) {

  /* Enable PORT E clock */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);
	// TX - PD5, AF7
	SET_BIT(GPIOD->MODER, GPIO_MODER_MODE5_1);
	SET_BIT(GPIOD->AFR[0],
			GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_1 | GPIO_AFRL_AFSEL5_2);
	// RX - PD6, AF7
	SET_BIT(GPIOD->MODER, GPIO_MODER_MODE6_1);
	SET_BIT(GPIOD->AFR[0],
			GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2);

	// DE/RE - PD7
	SET_BIT(GPIOD->MODER, GPIO_MODER_MODE7_0);
	CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT7);
	CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD7);


	// Init USART2 module
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);
	/*TX/RX_baundrate = (84MHz / 16)/115200 = 45.57; 45 = 0x2D; 57 = 0x9*/
	USART2->BRR = 0x2D9;

	// 8 bit, no parity, 1 stop
	CLEAR_BIT(USART2->CR1, USART_CR1_M | USART_CR1_PCE);
	CLEAR_BIT(USART2->CR2, USART_CR2_STOP);
	// enable USART

	SET_BIT(USART2->CR1, USART_CR1_UE);
}


void USARTx_send_data(char *buffer){
  
  while(*buffer){
    USART1->DR = (*buffer++ &(uint16_t)0x01FF);
    while(!READ_BIT(USART1->SR, USART_SR_TC));
  }
}


char USARTx_receive_data(void){
  char c;
  do{;
  }while(!(READ_BIT(USART1->SR, USART_SR_RXNE)));
    c = (char)(USART1->DR & (uint16_t)0x01FF);
    return c;
}


int _write(void *r, uint8_t *text, int len){
	char *p = (char*)text;
	text[len - 1] = 0;
	while(*p){
		do{;
		}while(!(READ_BIT(USART1->SR, USART_SR_TXE)));
		USART1->DR = *p;
		p++;
	}
	return len;
}

