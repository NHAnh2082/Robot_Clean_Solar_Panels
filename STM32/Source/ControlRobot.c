#include "ControlRobot.h"

/**
 * @brief   Di chuyển robot về phía trước với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void move_Forward(int16_t speed)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	if (speed > 23) speed = 23;
	TIM3->CCR1 = (uint16_t)((speed/23.0) * (TIM3->ARR+1)); 
	TIM3->CCR2 = 0;
	
	TIM4->CCR1 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
	TIM4->CCR2 = 0;
	
	// t1 = TIM3->CCR1;
	// t2 = TIM3->CCR2;
	// t3 = TIM4->CCR1;
	// t4 = TIM4->CCR2;
}

/**
 * @brief   Di chuyển robot lùi với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void move_Backward(int16_t speed)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);

	if (speed > 23) speed = 23;	
	TIM3->CCR1 = 0;
	TIM3->CCR2 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
	
	TIM4->CCR1 = 0;
	TIM4->CCR2 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
}

/**
 * @brief   Quay robot sang trái với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void turn_Left(int16_t speed)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	if (speed > 23) speed = 23;	
	// motor right speed
	TIM4->CCR1 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
	TIM4->CCR2 = 0;
		
	// motor left speed
	TIM3->CCR1 = 0;
	TIM3->CCR2 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
}

/**
 * @brief   Quay robot sang phải với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void turn_Right(int16_t speed)
{
	GPIO_SetBits(LED_GPIO_PORT, LED_RIGHT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	
	if (speed > 23) speed = 23;	
	// motor right speed
	TIM4->CCR1 = 0;
	TIM4->CCR2 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
		
	// motor left speed
	TIM3->CCR1 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
	TIM3->CCR2 = 0;
}

/**
 * @brief   Dừng toàn bộ robot, tắt các LED báo hiệu
 */
void stop_Robot(void)
{
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	TIM3->CCR1 = 0; TIM3->CCR2 = 0;
	TIM4->CCR1 = 0; TIM4->CCR2 = 0;
}

/**
 * @brief   Chạy bàn chải với tốc độ cho trước
 * @param   speed   Tốc độ bàn chải (0 - 53)
 */
void run_Brush(int16_t speed)
{
	if (speed > 53) speed = 53;
	else if (speed == 0)
	{
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	}
	else
	{
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_SetBits(BRUSH_PWM_PORT, IN2_PIN);
		TIM2->CCR1 = (uint16_t)((speed/53.0) * (TIM2->ARR+1));
	}
}

/**
 * @brief   Đảo chiều quay bàn chải với tốc độ cho trước
 * @param   speed   Tốc độ bàn chải (0 - 28)
 */
void reverse_Brush(int16_t speed)
{
	if (speed > 28) speed = 28;
	else if (speed == 0)
	{
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	}
	else
	{
		GPIO_SetBits(  BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
		TIM2->CCR1 = (uint16_t)((speed/53.0) * (TIM2->ARR+1));
	}
}

/**
 * @brief   Dừng bàn chải và ngắt PWM
 */
void stop_Brush(void)
{
	GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
	GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	TIM2->CCR1 = 0;
}

/**
 * @brief   Chạy chế độ tự động robot với logic xử lý dựa trên cảm biến bụi bẩn
 * @details Robot sẽ tự động đi tới, dừng lại vệ sinh khu vực bẩn, sau đó tiếp tục di chuyển.
 */
void run_auto(void)
{
	stop_Brush();
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	do
	{
		if (detect_dirt[0] == '0')
		{
			cnt_detect = 0;
			move_Forward(wheel_speed);
		}
		else if (detect_dirt[0] == '1')
		{
			cnt_detect++;
			stop_Robot();
			delay_01ms(10000);
			if (cnt_detect <= 2)
			{
				auto1_pulse = auto_pulse;
				do
				{
					move_Forward(wheel_speed);
				}
				while (auto_pulse - auto1_pulse <= 5798);
				stop_Robot();
				delay_01ms(10000);
				run_Brush(brush_speed);
				delay_01ms(30000);
				stop_Brush();
				
				auto2_pulse = auto_pulse;
				do
				{
					move_Backward(wheel_speed);
				}
				while (auto2_pulse - auto_pulse <= 5798); 
				stop_Robot();
				delay_01ms(10000);
			}
			else
			{
				move_Forward(wheel_speed);
				delay_01ms(25000);
				cnt_detect = 0;
			}
		}
	}
	while (auto_pulse < 33269);
	stop_Robot();
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);
	
	do
	{
		turn_Left(wheel_speed);
	}
	while (auto_pulse > (65535 - 22786)); // 22786 // 21990 // 22333
	stop_Robot();	
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	delay_01ms(10000);

	do
	{
		move_Forward(wheel_speed);
		run_Brush(brush_speed);
	}
	while (auto_pulse < 1820); //1810 // 1760 // 1820 2044
	stop_Brush();
	stop_Robot();	
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);

	do
	{
		turn_Left(wheel_speed);
	}
	while (auto_pulse > (65535 - 20121)); // 19504 // 20121 23668
	stop_Robot();	
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	delay_01ms(10000);
	
	do
	{
		if (detect_dirt[0] == '0')
		{
			cnt_detect = 0;
			move_Forward(wheel_speed);
		}
		else if (detect_dirt[0] == '1')
		{
			cnt_detect++;
			stop_Robot();
			delay_01ms(10000);
			if (cnt_detect <= 2)
			{
				auto1_pulse = auto_pulse;
				do
				{
					move_Forward(wheel_speed);
				}
				while (auto_pulse - auto1_pulse <= 6023);
				stop_Robot();
				delay_01ms(10000);
				run_Brush(brush_speed);
				delay_01ms(30000);
				stop_Brush();
				
				auto2_pulse = auto_pulse;
				do
				{
					move_Backward(wheel_speed);
				}
				while (auto2_pulse - auto_pulse <= 6023); 
				stop_Robot();
				delay_01ms(10000);
			}
			else
			{
				move_Forward(wheel_speed);
				delay_01ms(25000);
				cnt_detect = 0;
			}
		}
	}
	while (auto_pulse < 30593);
	stop_Robot();
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);
}