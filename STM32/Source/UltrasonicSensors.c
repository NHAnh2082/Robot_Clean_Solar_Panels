/******************************************************************************
 * @file    UltrasonicSensors.c
 * @brief   Định nghĩa hàm điều khiển 4 cảm biến siêu âm dùng Timer12
 * @details Sử dụng Timer12 để đo xung Echo, tính khoảng cách, phát hiện thay đổi,
 *          cấu hình GPIO chân Trigger và Echo.
 * @version 1.0
 * @date    2024-06-xx
 * @author  Anh Nguyen
 ******************************************************************************/

#include "stm32f4xx.h"
#include <stdbool.h>
#include <math.h>
#include "UltrasonicSensors.h"
#include "DelayUtils.h"

// Khai báo các mảng port, chân cho Echo và Trigger
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

/**
 * @brief   Khởi tạo chân GPIO Trigger và Echo cho sensor
 * @param   sensorIndex    Chỉ số sensor [0..NUM_SENSORS-1]
 * @param   Trigger        Chân Trigger (GPIO_Pin_x)
 * @param   Echo           Chân Echo (GPIO_Pin_x)
 * @return  void
 */
void init_GPIO_Sensors(uint8_t sensorIndex, uint16_t Trigger, uint16_t Echo)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	// Bật clock GPIO cho các port Echo và Trigger
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Cấu hình chân Trigger: output push-pull
	GPIO_InitStructure.GPIO_Pin   = Trigger;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_Init(TrigPort[sensorIndex], &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	// Cấu hình chân Echo: input
	GPIO_InitStructure.GPIO_Pin   = Echo;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_Init(EchoPort[sensorIndex], &GPIO_InitStructure);
}

/**
 * @brief   Khởi tạo Timer12 làm bộ đếm thời gian xung Echo
 * @details Timer12 được cấu hình với prescaler cho xung 1 MHz (1 us tick)
 * @return  void
 */
void init_Sensor_TIM12(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,  ENABLE);
	
	/* Timer 12 configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;   /* clk = 84MHz/84 = 1MHz */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC1Init(TIM12, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM12, ENABLE);
}

/**
 * @brief   Đo khoảng cách sensor siêu âm sensorIndex
 * @param   sensorIndex    Chỉ số sensor [0..NUM_SENSORS-1]
 * @return  void
 */
void read_distance(uint8_t sensorIndex)
{
	GPIO_TypeDef* Echo_GPIO = EchoPort[sensorIndex];
	GPIO_TypeDef* Trig_GPIO = TrigPort[sensorIndex];
	uint16_t Trigger_Pin = trigPins[sensorIndex];
	uint16_t Echo_Pin = echoPins[sensorIndex];
	
	// Kích hoạt tín hiệu Trigger
	GPIO_ResetBits(Trig_GPIO, Trigger_Pin);
	delay_us(2);
	GPIO_SetBits(Trig_GPIO, Trigger_Pin);
	delay_us(10);
	GPIO_ResetBits(Trig_GPIO, Trigger_Pin);
	
	// Đợi tín hiệu Echo lên
	uint32_t timeout = 100000; 
	start_time = TIM12->CNT;
	while (!(GPIO_ReadInputDataBit(Echo_GPIO, Echo_Pin)))
	{
		if (TIM12->CNT - start_time > timeout)
		{
			sensorStatus[sensorIndex] = false;
			return;
		}
	};

	// Đo thời gian Echo
	start_time = TIM12->CNT;
	while (GPIO_ReadInputDataBit(Echo_GPIO, Echo_Pin))
	{
		if (TIM12->CNT - start_time > timeout)
		{
			sensorStatus[sensorIndex] = false;
			return;
		}
	};
	stop_time  = TIM12->CNT;
	
	// Tính toán thời gian kéo dài xung
	uint32_t duration = 0;
	if (stop_time >= start_time)
	{
		duration = stop_time - start_time;
	}
	else if (start_time > stop_time)
	{
		duration = (0xFFFF - start_time) + stop_time;
	}

	// Tính khoảng cách theo công thức: time / 58 (cm)
	distance[sensorIndex] = duration / 58.0;

	// Làm tròn 2 chữ số thập phân
	distance[sensorIndex] = (float)((int32_t)(distance[sensorIndex] * 100)) / 100.0;

	sensorStatus[sensorIndex] = true;
}

/**
 * @brief   Phát hiện thay đổi trạng thái dựa trên sai số khoảng cách
 * @param   init_dis    Khoảng cách ban đầu
 * @param   curr_dis    Khoảng cách hiện tại
 * @return  int         0: không thay đổi, 1: thay đổi hoặc vượt ngưỡng
 */
int detect_edge(float init_dis, float curr_dis)
{
	int flag;
	float err_dis = 0;
	err_dis = curr_dis - init_dis;
	if ((err_dis == 0) || (fabs(err_dis) <= err_distance)) 
	{
		flag = 0;
	}
	else if ((fabs(err_dis) > err_distance) || (curr_dis >= max_distance)) 
	{
		flag = 1;
	}
	return flag;
}

/**
 * @brief   Cập nhật cờ trạng thái cho tất cả sensor
 * @return  void
 */
void get_flag(void)
{
	for (int i = 0; i < NUM_SENSORS; i++)
    {
        flag[i] = detect_edge(init[i], curr[i]);
    }
}
