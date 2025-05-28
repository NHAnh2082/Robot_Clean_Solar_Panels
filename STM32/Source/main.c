#include "stm32f4xx.h"
#include "system_timetick.h"
#include "math.h"
#include "Battery.h"
#include "MotorPWM.h"
#include "MotorEncoder.h"
#include "UltrasonicSensors.h"
#include "PeripheralControl.h"
#include "UART.h"
#include "DelayUtils.h"
#include "DataConversion.h"
#include "PIDCalculate.h"
#include "ControlRobot.h"

// tính và gửi tốc độ, khoảng cách
float velo1, velo2, distance_move;

int cur_pulse = 0;   /**< Tổng pulse hiện tại */
int auto_pulse = 0;  /**< Pulse dùng cho chế độ tự động */
int auto1_pulse = 0; /**< Biến phụ trợ trong chế độ tự động */
int auto2_pulse = 0; /**< Biến phụ trợ trong chế độ tự động */

int pulse = 0;       /**< Biến pulse phụ */
int isMove1 = 0;     /**< Trạng thái di chuyển 1 */
int isMove2 = 0;     /**< Trạng thái di chuyển 2 */

int16_t cnt_tim5;    /**< Bộ đếm ngắt Timer5 */
int16_t cnt_detect;  /**< Bộ đếm phát hiện bụi bẩn */

/**
 * @brief   Khởi tạo timer 5 làm bộ đếm mẫu thời gian
 */
void init_TIM5_SampleTime(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	

	/* Enable clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,  ENABLE);
	
	/* Configure parameters */
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1; 
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	/* Clear interrupt update flag 1 */
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	
	/* Cau hinh ngat cho Timer 5 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief   Xử lý ngắt Timer5
 */
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update))
	{
		p = (int32_t)TIM1->CNT;
		Enc_Pulse(p);
		velo1 = (float)((float)dp /(PULSE_PER_REVOLUTION * ENCODER_mode) * (float)(0.14*3.14));	// m/s
		velo2 = (float)((float)dp /(PULSE_PER_REVOLUTION * ENCODER_mode) * 60.0);						  // rpm
		distance_move = (float)((float)cur_pulse / (PULSE_PER_REVOLUTION * ENCODER_mode) * (float)(0.14*3.14));
		
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
		DMA1_Stream4->NDTR = BUFF_SIZE_TX;
		DMA_Cmd(DMA1_Stream4, ENABLE);		
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}

/**
 * @brief   Gộp các trường dữ liệu thành chuỗi truyền
 */
void MergeStr(void)
{
	txbuff[0]  = 'D';
	txbuff[1]  = '_';
	for (int16_t i=0; i<3; i++)
	{
		txbuff[i+2] = data_per[i];	// percent
	}
	txbuff[5]  = '_';
	for (int16_t i=0; i<6; i++)
	{
		txbuff[i+6] = data_pulse[i];	// pulse
	}
	txbuff[12] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+13] = data_speed[i];	// speed
	}
	txbuff[18] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+19] = data_move[i];	// distance move
	}
	txbuff[24] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+25] = data_dis[0][i];	// distance 1
	}
	txbuff[30] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+31] = data_dis[1][i];	// distance 2
	}
	txbuff[36] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+37] = data_dis[2][i];	// distance 3
	}
	txbuff[42] = '_';
	for (int16_t i=0; i<5; i++)
	{
		txbuff[i+43] = data_dis[3][i]; // distance 4
	}
	txbuff[48] = '_';
	txbuff[49] = data_flag[0][0];
	txbuff[50] = '_';
	txbuff[51] = data_flag[1][0];
	txbuff[52] = '_';
	txbuff[53] = data_flag[2][0];
	txbuff[54] = '_';
	txbuff[55] = data_flag[3][0];
	txbuff[56] = '_';
	txbuff[57] = '\r';
	txbuff[58] = '\n';
}

int main(void)
{
	/* Enable SysTick at 10ms interrupt */
	SysTick_Config(SystemCoreClock/100);
	
	init_PWM();
	init_ENCODER();
	Set_PWM_Freq(20000);	
	stop_Robot();		
	stop_Brush();		
	
	init_ADC();		
	init_UART();
	init_PERIPHERAL();
	init_TIM5_SampleTime();
	
	for (uint8_t i=0; i<NUM_SENSORS; i++)
	{
		init_GPIO_Sensors(i, trigPins[i], echoPins[i]);
	}
	init_Sensor_TIM12();

	/* Disable timer */
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM8->CR1 &= ~TIM_CR1_CEN;
	
	/* Starting Timer */
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM8->CR1 |= TIM_CR1_CEN;

	delay_01ms(100);

	while(1)
	{	
		// Cập nhật giá trị sensor và biến truyền
		p_alpha = readEncPos(90);	
		
		cnt_cb++;
		for (uint8_t i=0; i<NUM_SENSORS; i++){
			read_distance(i);
		}
		delay_01ms(100);

		if (cnt_cb <= 5)
		{
			for(uint8_t i=0; i<NUM_SENSORS; i++)
			{
				init[i] = distance[i];
			}
		}
		else if (cnt_cb >= 10)
		{
			for(uint8_t i=0; i<NUM_SENSORS; i++)
			{
				curr[i] = distance[i];
				FloatToStr6(curr[i], data_dis[i], 2, 2);
			}
		}

		get_flag();
		for (uint8_t i=0; i<NUM_SENSORS; i++)
		{
			IntToStr(flag[i], data_flag[i], 1);
		}

		// Cập nhật chuỗi dữ liệu
		IntToStr(cur_pulse, data_pulse, 6);
		FloatToStr6(velo1, data_speed, 1, 3);
		FloatToStr6(distance_move, data_move, 2, 2);
		
		// Điều khiển robot
		switch(dir_wheel[0])
		{
			case 'S':
				stop_Robot();
				break;

			case 'F':
				if (detect_dirt[0] == '0') 
				{
					move_Forward(wheel_speed);
				}
				else 
				{
					stop_Robot();
				}
				break;

			case 'B':
				move_Backward(wheel_speed);
				break;

			case 'L':
				turn_Left(wheel_speed);
				break;

			case 'R':
				turn_Right(wheel_speed);
				break;

			default:
				break;
		}
		
		// Điểu khiển chổi
		switch(dir_brush[0])
		{
			case '0':
				stop_Brush();
				break;

			case '1':
				run_Brush(brush_speed);
				break;

			default:
				break;
		}
		
		// Điều khiển máy bơm nước
		switch(dir_pump[0])
		{
			case '0':
				GPIO_SetBits(RELAY_PORT, RELAY_GPIO);
				break;

			case '1':
				GPIO_ResetBits(RELAY_PORT, RELAY_GPIO);
				break;
		}
		
		// Chuyển sang chế độ tự động
		if (dir_auto[0] == '1') run_auto();
		
		if ((dir_auto[0] == '1') || (dir_wheel[0] == 'F' || dir_wheel[0] == 'B' || dir_wheel[0] == 'R' || dir_wheel[0] == 'L'))
		{
			p = (int32_t)TIM1->CNT;
			IntToStr(p, data_pulse, 5);
		}
	
		// Gộp chuỗi và lưu dữ liệu vào txbuff
		MergeStr();
		delay_01ms(100);
	}	
}