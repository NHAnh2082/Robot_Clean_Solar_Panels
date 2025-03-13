#include "stm32f4xx.h"

#define LED_GPIO_PORT		 GPIOD
#define LED_FORWARD			 GPIO_Pin_13
#define LED_BACKWARD		 GPIO_Pin_15
#define LED_LEFT				 GPIO_Pin_12
#define LED_RIGHT				 GPIO_Pin_14

#define RELAY_PORT			 GPIOB
#define RELAY_GPIO			 GPIO_Pin_12

void init_PERIPHERAL(void);
