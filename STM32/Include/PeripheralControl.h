/******************************************************************************
 * @file    PeripheralControl.h
 * @brief   Khai báo các chân LED và Relay cùng các hàm khởi tạo
 * @details Cấu hình các chân GPIO cho LED báo hiệu trạng thái di chuyển
 *          và relay điều khiển thiết bị ngoài.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef PERIPHERALCONTROL_H
#define PERIPHERALCONTROL_H

#include "stm32f4xx.h"

// Định nghĩa chân GPIO cho các LED
#define LED_GPIO_PORT    GPIOD
#define LED_FORWARD      GPIO_Pin_13
#define LED_BACKWARD     GPIO_Pin_15
#define LED_LEFT         GPIO_Pin_12
#define LED_RIGHT        GPIO_Pin_14

// Định nghĩa chân GPIO cho Relay
#define RELAY_PORT       GPIOB
#define RELAY_GPIO       GPIO_Pin_12

/**
 * @brief   Khởi tạo toàn bộ ngoại vi (LED, Relay)
 * @return  void
 */
void init_PERIPHERAL(void);

#endif  // PERIPHERALCONTROL_H