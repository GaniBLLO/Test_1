#include "stm32f4xx.h"

void system_clock_168m_25m_hse(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    
    /* enable HSE */
    SET_BIT(RCC->CR, RCC_CR_HSEON);

    /* wait until HSE is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = READ_BIT(RCC->CR, RCC_CR_HSERDY);
    }while((0U == stab_flag) && (0xFFFF != timeout));

    /* if fail */
    if(0U == READ_BIT(RCC->CR, RCC_CR_HSERDY)){
        while(1){
        }
    }
    
    /* Flash access (read RM0090) */
    FLASH->ACR |= FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    /* Enable APB1 */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    SET_BIT(PWR->CR, PWR_CR_VOS);

    /* HSE is stable */
    /* AHB = SYSCLK = 168 MHz */
    /* APB2 = AHB/2 = 84 MHz */
    /* APB1 = AHB/4 = 42 MHz */
    SET_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4);

    /* Configure the main PLL, PSC = 25, PLL_N = 336, PLL_P = 2 (set to PLLP = 00), PLL_Q = 7 */ 
    uint32_t PLLM = 25 << RCC_PLLCFGR_PLLM_Pos;
    uint32_t PLLN = 336 << RCC_PLLCFGR_PLLN_Pos;
    uint32_t PLLP = 0 << RCC_PLLCFGR_PLLP_Pos;
    uint32_t PLLQ = 7 << RCC_PLLCFGR_PLLQ_Pos;
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR,
               RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLP_Msk | RCC_PLLCFGR_PLLQ_Msk,
               PLLM | PLLN | PLLP | PLLQ);
    
    /* enable PLL */
    SET_BIT(RCC->CR, RCC_CR_PLLON);

    /* wait until PLL is stable */
    while(0U == READ_BIT(RCC->CR, RCC_CR_PLLRDY)){
    }
    
    /* select PLL as system clock */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_PLL);
    
    /* wait until PLL is selected as system clock */
    while(RCC_CFGR_SWS_PLL != (RCC->CFGR & RCC_CFGR_SWS_Msk) ){
    }

    SystemCoreClockUpdate();
} 