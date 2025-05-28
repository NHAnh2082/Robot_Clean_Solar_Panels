/******************************************************************************
 * @file    PeripheralControl.c
 * @brief   Định nghĩa hàm khởi tạo các ngoại vi như LED và Relay
 * @details Cấu hình chân GPIO, bật clock và thiết lập trạng thái ban đầu cho
 *          các LED và Relay trên STM32F4.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#include "stm32f4xx.h"
#include "PeripheralControl.h"

/**
 * @brief   Khởi tạo 4 LED tương ứng với các trạng thái di chuyển của robot
 * @details
 *      + LED cam    - Tiến (Forward)
 *      + LED đỏ     - Phải (Right)
 *      + LED xanh dương - Lùi (Backward)
 *      + LED vàng   - Trái (Left)
 *      Các LED được cấu hình GPIO output push-pull, tốc độ cao, pull-none.
 *      Trạng thái mặc định là tắt.
 * @return  void
 */
static void init_LED(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;

	// Bật clock cho GPIOD
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	// Cấu hình chân LED
	GPIO_InitStructure.GPIO_Pin   = LED_FORWARD | LED_BACKWARD | LED_LEFT | LED_RIGHT;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
	// Tắt tất cả LED mặc định
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
}

/**
 * @brief   Khởi tạo chân Relay điều khiển
 * @details Cấu hình chân GPIO output push-pull, tốc độ cao, pull-none,
 *          trạng thái mặc định relay bật (GPIO SetBits).
 * @return  void
 */
static void init_Relay(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Bật clock cho GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// Cấu hình chân Relay
	GPIO_InitStructure.GPIO_Pin   = RELAY_GPIO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);
	
	// Relay mặc định ON
	GPIO_SetBits(RELAY_PORT, RELAY_GPIO);
}

/**
 * @brief   Khởi tạo toàn bộ ngoại vi (LED và Relay)
 * @details Gọi các hàm init_LED và init_Relay
 * @return  void
 */
void init_PERIPHERAL(void)
{
	init_LED();
	init_Relay();
}
