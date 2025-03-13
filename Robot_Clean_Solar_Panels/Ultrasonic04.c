#include "stm32f4xx.h"
#include <stdbool.h>
#include <math.h>
#include "Ultrasonic04.h"
#include "Delay.h"

//GPIO_TypeDef* sensorPorts[NUM_SENSORS] = {GPIOD, GPIOD, GPIOE, GPIOE};
//uint16_t echoPins[NUM_SENSORS] = {GPIO_Pin_2, GPIO_Pin_4, GPIO_Pin_1, GPIO_Pin_3};
//uint16_t trigPins[NUM_SENSORS] = {GPIO_Pin_1, GPIO_Pin_3, GPIO_Pin_0, GPIO_Pin_2};
GPIO_TypeDef* EchoPort[NUM_SENSORS] = {GPIOD, GPIOE, GPIOD, GPIOE};
GPIO_TypeDef* TrigPort[NUM_SENSORS] = {GPIOE, GPIOE, GPIOD, GPIOD};
uint16_t echoPins[NUM_SENSORS] = {GPIO_Pin_2, GPIO_Pin_1, GPIO_Pin_4, GPIO_Pin_3}; // PD2 PE1 PD4 PE3
uint16_t trigPins[NUM_SENSORS] = {GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_1, GPIO_Pin_3}; // PE0 PE2 PD1 PD3
bool sensorStatus[NUM_SENSORS] = {false};
float distance[NUM_SENSORS] = {0};

uint32_t start_time,stop_time;
float init[NUM_SENSORS] = {0};
float curr[NUM_SENSORS] = {0};
int flag[NUM_SENSORS] = {0}; // Flag used to monitor the status of the sensor
int16_t cnt_cb = 0;

uint8_t data_dis[NUM_SENSORS][5];
uint8_t data_flag[NUM_SENSORS][1];

//------------------------------------------------------------
void init_GPIO_Sensors(uint8_t sensorIndex, uint16_t Trigger, uint16_t Echo){
	GPIO_InitTypeDef  GPIO_InitStructure;

//	if (EchoPort[sensorIndex] == TrigPort[sensorIndex]){
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	}
//	else if (EchoPort[sensorIndex] != TrigPort[sensorIndex]){
//		if (EchoPort[sensorIndex] == GPIOE){
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//		}
//		else if (EchoPort[sensorIndex] == GPIOD){
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//		}
//	}
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	/* Trigger pin configuration */
	GPIO_InitStructure.GPIO_Pin   = Trigger;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_Init(TrigPort[sensorIndex], &GPIO_InitStructure);
	
	/* Echo pin configuration */
	GPIO_InitStructure.GPIO_Pin   = Echo;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_Init(EchoPort[sensorIndex], &GPIO_InitStructure);
}

//------------------------------------------------------------
void init_Sensor_TIM12(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef 			 TIM_OCInitStructure;
	
	/* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,  ENABLE);
	
		/* Timer 12 configuration */
	TIM_TimeBaseStructure.TIM_Prescaler 		= 84 - 1;   /* clk = 84MHz/84 = 1MHz */
  TIM_TimeBaseStructure.TIM_Period 				= 0xFFFF - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM12, ENABLE);
}

//------------------------------------------------------------
void read_distance(uint8_t sensorIndex){
	GPIO_TypeDef* Echo_GPIO  = EchoPort[sensorIndex];
	GPIO_TypeDef* Trig_GPIO  = TrigPort[sensorIndex];
	uint16_t Trigger_Pin = trigPins[sensorIndex];
	uint16_t Echo_Pin 	 = echoPins[sensorIndex];
	
	// Activate the trigger signal
	GPIO_ResetBits(Trig_GPIO, Trigger_Pin);
	delay_us(2);
	GPIO_SetBits(Trig_GPIO, Trigger_Pin);
	delay_us(10);
	GPIO_ResetBits(Trig_GPIO, Trigger_Pin);
	
	// Wait for echo signal
	uint32_t timeout = 100000; 
	start_time = TIM12->CNT;
	while (!(GPIO_ReadInputDataBit(Echo_GPIO, Echo_Pin))){
		if (TIM12->CNT - start_time > timeout){
			sensorStatus[sensorIndex] = false;
			return;
		}
	};
	start_time = TIM12->CNT;
	while (GPIO_ReadInputDataBit(Echo_GPIO, Echo_Pin)){
		if (TIM12->CNT - start_time > timeout){
			sensorStatus[sensorIndex] = false;
			return;
		}
	};
	stop_time  = TIM12->CNT;
	
	uint32_t duration = 0;
	if (stop_time >= start_time){
		duration = stop_time - start_time;
	}
	else if (start_time > stop_time){
		duration = (0xFFFF - start_time) + stop_time;
	}
	distance[sensorIndex] = duration / 58.0;
	distance[sensorIndex] = (float)((int32_t)(distance[sensorIndex] * 100)) / 100.0;
	
	sensorStatus[sensorIndex] = true;
}

//------------------------------------------------------------
int detect_edge(float init_dis, float curr_dis){
	int flag;
	float err_dis = 0;
	err_dis = curr_dis - init_dis;
	if ((err_dis == 0) || (fabs(err_dis) <= err_distance)) flag = 0;
	else if ((fabs(err_dis) > err_distance) || (curr_dis >= max_distance)) flag = 1;
	return flag;
}

//------------------------------------------------------------
void get_flag(void){
	flag[0] = detect_edge(init[0], curr[0]);
	flag[1] = detect_edge(init[1], curr[1]);
	flag[2] = detect_edge(init[2], curr[2]);
	flag[3] = detect_edge(init[3], curr[3]);
}
