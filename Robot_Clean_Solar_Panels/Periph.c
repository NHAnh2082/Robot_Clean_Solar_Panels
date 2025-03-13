#include "stm32f4xx.h"
#include "Periph.h"

static void init_LED(void){	
	/* 
		Initialize 4 leds corresponding to 4 moving states of the robot
			+ Orange - Forward
			+ Red		 - Right
			+ Blue	 - Backward
			+ Yellow - Left
	*/
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	/* Led pin configuration */
  GPIO_InitStructure.GPIO_Pin   = LED_FORWARD | LED_BACKWARD | LED_LEFT | LED_RIGHT;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
	/* Default configuration for led is off */
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
}

//------------------------------------------------------------
static void init_Relay(void){	
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* Led pin configuration */
  GPIO_InitStructure.GPIO_Pin   = RELAY_GPIO;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(RELAY_PORT, &GPIO_InitStructure);
	
	/* Default off relay */
	GPIO_SetBits(RELAY_PORT, RELAY_GPIO);
}

//------------------------------------------------------------
void init_PERIPHERAL(void){
	init_LED();
	init_Relay();
}
