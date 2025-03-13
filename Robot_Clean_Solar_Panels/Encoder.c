#include "stm32f4xx.h"
#include "Encoder.h"

//------------------------------------------------------------
static void init_ENCR_TIM1(void){	
	GPIO_InitTypeDef				 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	/* TIM1 channel1,2 configuration */
	GPIO_InitStructure.GPIO_Pin   = ENCR_CHA_PIN | ENCR_CHB_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(ENCR_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(ENCR_GPIO_PORT, GPIO_PinSource9,  GPIO_AF_TIM1);
	GPIO_PinAFConfig(ENCR_GPIO_PORT, GPIO_PinSource11, GPIO_AF_TIM1);
	
	/* TIM1 configuration */
	TIM_TimeBaseStructure.TIM_Period 		  = 0xFFFF - 1;  // count:0 -> 0xFFFF -> 0 -> ...
	TIM_TimeBaseStructure.TIM_Prescaler   = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIMER_ENC_MR, &TIM_TimeBaseStructure);
	
	/* Cau hinh che do doc xung tu encoder tren Timer1 */
	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIMER_ENC_MR->CNT = 0;
	TIM_Cmd(TIMER_ENC_MR, ENABLE);
}

//------------------------------------------------------------
static void init_ENCL_TIM8(void){	
	GPIO_InitTypeDef				 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* Enable clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	/* TIM1 channel1,2 configuration */
	GPIO_InitStructure.GPIO_Pin   = ENCL_CHA_PIN | ENCL_CHB_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(ENCL_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(ENCL_GPIO_PORT, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(ENCL_GPIO_PORT, GPIO_PinSource7, GPIO_AF_TIM8);
	
	/* TIM8 configuration */
	TIM_TimeBaseStructure.TIM_Period 		  = 0xFFFF; // 2M/1000=2k
	TIM_TimeBaseStructure.TIM_Prescaler   = 0; // 84M/42=2M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIMER_ENC_ML, &TIM_TimeBaseStructure);
	
	/* Initialise encoder interface */
	TIM_EncoderInterfaceConfig(TIMER_ENC_ML, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	/* TIM enable counter */
	TIMER_ENC_ML->CNT = 0;
	TIM_Cmd(TIMER_ENC_ML, ENABLE);
}

//------------------------------------------------------------
void init_ENCODER(void){
	init_ENCR_TIM1();		// Initialize Timer for Encoder right
	init_ENCL_TIM8();		// Initialize Timer for Encoder left
}
