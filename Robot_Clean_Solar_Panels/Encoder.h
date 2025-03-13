#include "stm32f4xx.h"

#define ENCR_GPIO_PORT	 GPIOE
#define ENCR_CHA_PIN		 GPIO_Pin_9
#define ENCR_CHB_PIN		 GPIO_Pin_11

#define ENCL_GPIO_PORT	 GPIOC
#define ENCL_CHA_PIN		 GPIO_Pin_6
#define ENCL_CHB_PIN		 GPIO_Pin_7

#define TIMER_ENC_MR		 TIM1
#define TIMER_ENC_ML		 TIM8


#define ENCODER_mode	 				4
#define PULSE_PER_REVOLUTION	13*139

void init_ENCODER(void);
