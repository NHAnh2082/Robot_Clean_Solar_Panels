#include "stm32f4xx.h"

#define TIMER_PWM_MR	 TIM4
#define TIMER_PWM_ML	 TIM3

#define MOTOR_PWM_PORT   GPIOB
#define ML_RPWM_PIN			 GPIO_Pin_4
#define ML_LPWM_PIN			 GPIO_Pin_5
#define MR_RPWM_PIN			 GPIO_Pin_6
#define MR_LPWM_PIN			 GPIO_Pin_7 

#define TIMER_PWM_MB		 TIM2
#define BRUSH_PWM_PORT	 GPIOA
#define ENA_PIN					 GPIO_Pin_0
#define IN1_PIN					 GPIO_Pin_2
#define IN2_PIN					 GPIO_Pin_4

extern int16_t frequency;

void init_PWM(void);
void Set_PWM_Freq(uint16_t freq);
void Set_PWM_Duty(int16_t speed1, int16_t speed2);
