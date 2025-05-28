/******************************************************************************
 * @file    MotorEncoder.h
 * @brief   Khai báo chân GPIO và timer cho Encoder trái và phải
 * @details Cấu hình các chân tín hiệu Encoder CHA, CHB cho 2 encoder bên trái và phải,
 *          sử dụng Timer1 và Timer8 làm bộ đếm xung Encoder trên STM32F4.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef MOTORENCODER_H
#define MOTORENCODER_H

#include "stm32f4xx.h"

// Chân GPIO cho Encoder bên phải
#define ENCR_GPIO_PORT	        GPIOE
#define ENCR_CHA_PIN		    GPIO_Pin_9
#define ENCR_CHB_PIN		    GPIO_Pin_11

// Chân GPIO cho Encoder bên trái
#define ENCL_GPIO_PORT	        GPIOC
#define ENCL_CHA_PIN		    GPIO_Pin_6
#define ENCL_CHB_PIN		    GPIO_Pin_7

// Timer sử dụng cho Encoder
#define TIMER_ENC_MR		    TIM1
#define TIMER_ENC_ML		    TIM8

// Chế độ đọc encoder (x4 mode)
#define ENCODER_mode	 		4

//  Số xung trên vòng quay encoder
#define PULSE_PER_REVOLUTION	13*139

/**
 * @brief   Khởi tạo toàn bộ module Encoder, cấu hình Timer và GPIO
 * @details Gọi các hàm cấu hình timer1 và timer8 cho encoder phải và trái.
 * @return  void
 */
void init_ENCODER(void);

#endif  // MOTORENCODER_H