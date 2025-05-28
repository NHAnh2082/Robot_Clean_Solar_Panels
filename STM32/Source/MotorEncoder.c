/******************************************************************************
 * @file    MotorEncoder.c
 * @brief   Định nghĩa hàm cấu hình Timer và GPIO cho Encoder trái và phải
 * @details Sử dụng TIM1 và TIM8 với chế độ Encoder Interface, cấu hình chân GPIO AF,
 *          bộ đếm xung theo chế độ TIM_EncoderMode_TI12, hỗ trợ đọc xung đầy đủ.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#include "stm32f4xx.h"
#include "MotorEncoder.h"

/**
 * @brief   Cấu hình Timer1 và GPIO cho Encoder bên phải
 * @details Bật clock, cấu hình GPIO PE9, PE11 chức năng Alternate Function TIM1,
 *          cấu hình timer ở chế độ Encoder Interface, reset bộ đếm, bật timer.
 * @return  void
 */
static void init_ENCR_TIM1(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	// Bật clock cho TIM1 và GPIOE
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	// Cấu hình GPIO PE9, PE11 cho Alternate Function TIM1
	GPIO_InitStructure.GPIO_Pin   = ENCR_CHA_PIN | ENCR_CHB_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(ENCR_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(ENCR_GPIO_PORT, GPIO_PinSource9,  GPIO_AF_TIM1);
	GPIO_PinAFConfig(ENCR_GPIO_PORT, GPIO_PinSource11, GPIO_AF_TIM1);
	
	// Cấu hình Timer1
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF - 1;  // count:0 -> 0xFFFF -> 0 -> ...
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIMER_ENC_MR, &TIM_TimeBaseStructure);
	
	// Cấu hình chế độ đọc xung từ Encoder trên Timer1
	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	// Reset bộ đếm
	TIMER_ENC_MR->CNT = 0;

	// Bật timer
	TIM_Cmd(TIMER_ENC_MR, ENABLE);
}

/**
 * @brief   Cấu hình Timer8 và GPIO cho Encoder bên trái
 * @details Bật clock, cấu hình GPIO PC6, PC7 chức năng Alternate Function TIM8,
 *          cấu hình timer ở chế độ Encoder Interface, reset bộ đếm, bật timer.
 * @return  void
 */
static void init_ENCL_TIM8(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	// Bật clock cho TIM8 và GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	// Cấu hình GPIO PC6, PC7 cho Alternate Function TIM8
	GPIO_InitStructure.GPIO_Pin   = ENCL_CHA_PIN | ENCL_CHB_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(ENCL_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(ENCL_GPIO_PORT, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(ENCL_GPIO_PORT, GPIO_PinSource7, GPIO_AF_TIM8);
	
	// Cấu hình Timer8
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 2M/1000=2k
	TIM_TimeBaseStructure.TIM_Prescaler = 0; // 84M/42=2M
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIMER_ENC_ML, &TIM_TimeBaseStructure);
	
	// Cấu hình chế độ Encoder
	TIM_EncoderInterfaceConfig(TIMER_ENC_ML, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	// Reset bộ đếm 
	TIMER_ENC_ML->CNT = 0;

	// Bật timer
	TIM_Cmd(TIMER_ENC_ML, ENABLE);
}

/**
 * @brief   Khởi tạo toàn bộ module Encoder (trái và phải)
 * @details Gọi hàm cấu hình timer và GPIO cho Encoder bên phải và bên trái
 * @return  void
 */
void init_ENCODER(void)
{
	init_ENCR_TIM1();		
	init_ENCL_TIM8();		
}