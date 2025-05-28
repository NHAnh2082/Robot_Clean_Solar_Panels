/******************************************************************************
 * @file    DelayUtils.c
 * @brief   Định nghĩa các hàm delay bằng Timer6 trên STM32F4
 * @details Các hàm delay sử dụng Timer6 với cấu hình bộ chia phù hợp
 *          để tạo ra độ trễ chính xác theo micro giây và 0.1 ms.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#include "stm32f4xx.h"
#include "DelayUtils.h"


/**
 * @brief   Tạo độ trễ theo micro giây sử dụng Timer6
 * @details Hàm khởi tạo Timer6 với prescaler phù hợp để có xung 1 MHz (1 us),
 *          sau đó đếm đến giá trị period, tạo độ trễ chính xác.
 * @param   period  Thời gian delay, tính bằng micro giây (us)
 * @return  void
 */
void delay_us(uint16_t period)
{
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	// Cấu hình prescaler: 84 MHz / 84 = 1 MHz -> 1 tick = 1 us
  	TIM6->PSC = 83;	
	
	// Giá trị auto-reload = period - 1 
  	TIM6->ARR = period-1;

	// Cập nhật giá trị đăng ký
  	TIM6->CNT = 0;
  	TIM6->EGR = 1;		

	// Cập nhật giá trị đăng ký
  	TIM6->SR  = 0;		

	// Bật Timer6
  	TIM6->CR1 = 1;		

	// Chờ đến khi cờ tràn được set
  	while (!TIM6->SR);
    
	// Tắt Timer6
  	TIM6->CR1 = 0;	
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
}


/**
 * @brief   Tạo độ trễ theo 0.1 ms (100 micro giây) sử dụng Timer6
 * @details Hàm khởi tạo Timer6 với prescaler phù hợp để có xung 10 kHz (0.1 ms),
 *          sau đó đếm đến giá trị period, tạo độ trễ chính xác.
 * @param   period  Thời gian delay, tính theo đơn vị 0.1 ms
 * @return  void
 */
void delay_01ms(uint16_t period)
{
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	// Cấu hình prescaler: 84 MHz / 8400 = 10 kHz -> 1 tick = 0.1 ms
  	TIM6->PSC = 8399;		

	// Giá trị auto-reload = period - 1
  	TIM6->ARR = period-1;

	// Cập nhật giá trị đăng ký
  	TIM6->CNT = 0;
  	TIM6->EGR = 1;		

	// Cập nhật giá trị đăng ký
  	TIM6->SR  = 0;		

	// Bật Timer6
  	TIM6->CR1 = 1;	
	
	// Chờ đến khi cờ tràn được set
  	while (!TIM6->SR);
    
	// Tắt Timer6
  	TIM6->CR1 = 0;		
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
}
