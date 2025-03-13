#include "stm32f4xx.h"
#include "PWM.h"

int16_t frequency = 20000; //20kHz



//------------------------------------------------------------
static void init_GPIO_TIM3(void){
	GPIO_InitTypeDef				 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 			 TIM_OCInitStructure;
	
	/* TIM and GPIO clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	/* PWM pulse pin configuration */
	GPIO_InitStructure.GPIO_Pin 	= ML_RPWM_PIN | ML_LPWM_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(MOTOR_PWM_PORT, &GPIO_InitStructure); 
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource5, GPIO_AF_TIM3);
	
	/* Configure TIM3 channel 1 as PWM */
	TIM_TimeBaseStructure.TIM_Prescaler 		= 42 - 1;   /* clk = 84MHz/42 = 2MHz */
//	TIM_TimeBaseStructure.TIM_Period 				= 1000 - 1; /* frequency = clk/ARR = 2MHz/1000 = 2kHz */
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMER_PWM_ML, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode 			 = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	
	/* Configure TIM3 channel 1 as PWM */
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1PreloadConfig(TIMER_PWM_ML, TIM_OCPreload_Enable); // CCMR1 - OC1M = 110
	TIM_OC1Init(TIMER_PWM_ML, &TIM_OCInitStructure);
	
	/* Configure TIM3 channel 2 as PWM */
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2PreloadConfig(TIMER_PWM_ML, TIM_OCPreload_Enable); // CCMR1 - OC2M = 110
	TIM_OC2Init(TIMER_PWM_ML, &TIM_OCInitStructure);
	
	TIM_CCxCmd(TIM3, TIM_Channel_1, ENABLE); //CCER
	TIM_CCxCmd(TIM3, TIM_Channel_2, ENABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);
}

//------------------------------------------------------------
static void init_GPIO_TIM4(void){	
	GPIO_InitTypeDef				 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 			 TIM_OCInitStructure;
	
	/* TIM and GPIO clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,  ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	/* PWM pulse pin configuration */
	GPIO_InitStructure.GPIO_Pin 	= MR_RPWM_PIN | MR_LPWM_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP ;
	GPIO_Init(MOTOR_PWM_PORT, &GPIO_InitStructure); 
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource7, GPIO_AF_TIM4);
	
	/* Configure TIM4 channel 1 as PWM */
	TIM_TimeBaseStructure.TIM_Prescaler 		= 42 - 1;   /* clk = 84MHz/42 = 2MHz */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMER_PWM_MR, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode 			 = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1PreloadConfig(TIMER_PWM_MR, TIM_OCPreload_Enable);
	TIM_OC1Init(TIMER_PWM_MR, &TIM_OCInitStructure);
	
	/* Configure TIM4 channel 2 as PWM */
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2PreloadConfig(TIMER_PWM_MR, TIM_OCPreload_Enable);
	TIM_OC2Init(TIMER_PWM_MR, &TIM_OCInitStructure);
	
	TIM_CCxCmd(TIM4, TIM_Channel_1, ENABLE);
	TIM_CCxCmd(TIM4, TIM_Channel_2, ENABLE);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	
	TIM_Cmd(TIM4, ENABLE);
}

//------------------------------------------------------------
static void init_Brush(void){
	GPIO_InitTypeDef				 GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 			 TIM_OCInitStructure;
	
	/* TIM and GPIO clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,  ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
	/* Dir pins configuration */
	GPIO_InitStructure.GPIO_Pin 	= IN1_PIN | IN2_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(BRUSH_PWM_PORT, &GPIO_InitStructure); 
	
	/* PWM pulse pin configuration */
	GPIO_InitStructure.GPIO_Pin 	= ENA_PIN;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(BRUSH_PWM_PORT, &GPIO_InitStructure); 
	GPIO_PinAFConfig(BRUSH_PWM_PORT, GPIO_PinSource0, GPIO_AF_TIM2);
	
	/* Configure TIM2 channel 2 as PWM */
	TIM_TimeBaseStructure.TIM_Prescaler 		= 42 - 1;   /* clk = 84MHz/42 = 2MHz */
	TIM_TimeBaseStructure.TIM_Period 				= 1000 - 1; /* frequency = clk/ARR = 2MHz/1000 = 2kHz */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMER_PWM_MB, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode 			 = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1PreloadConfig(TIMER_PWM_MB, TIM_OCPreload_Enable); // CCMR1 - OC1M = 110
	TIM_OC1Init(TIMER_PWM_MB, &TIM_OCInitStructure);
		
	TIM_CCxCmd(TIMER_PWM_MB, TIM_Channel_1, ENABLE); //CCER
	TIM_ARRPreloadConfig(TIMER_PWM_MB, ENABLE);
}

//------------------------------------------------------------
void Set_PWM_Freq(uint16_t freq){
	uint32_t period1 = 0, period2 = 0, period3 = 0;
	period1 = 84000000/(freq*(TIM3->PSC+1))-1;
	period2 = 84000000/(freq*(TIM4->PSC+1))-1;
	period3 = 84000000/(freq*(TIM2->PSC+1))-1;
	if (period1 > 0xFFFF) period1 = 0xFFFF;
	if (period2 > 0xFFFF) period2 = 0xFFFF;
	if (period3 > 0xFFFF) period3 = 0xFFFF;
	TIM3->ARR = period1;
	TIM4->ARR = period2;
	TIM2->ARR = period3;
}

//------------------------------------------------------------
void Set_PWM_Duty(int16_t speed1, int16_t speed2){
	if (speed1 > 145) speed1 = 145;
	if (speed2 > 255) speed2 = 255;
	//TIM12->CCR1 = (uint32_t)(duty * (TIM12->ARR+1))/100;
	TIM1->CCR1 = (uint16_t)((speed1/145) * (TIM1->ARR+1));
	TIM1->CCR2 = (uint16_t)((speed2/255) * (TIM1->ARR+1));
}

//------------------------------------------------------------
void init_PWM(void){
	init_GPIO_TIM3();		// Initialize PWM for motor left
	init_GPIO_TIM4();		// Initialize PWM for motor right
	init_Brush();				// Initialize PWM for motor brush
}
