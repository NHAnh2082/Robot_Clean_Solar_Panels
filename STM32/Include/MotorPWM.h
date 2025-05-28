/******************************************************************************
 * @file    MotorPWM.h
 * @brief   Khai báo các hằng số và hàm điều khiển PWM cho động cơ và bàn chải
 * @details Cấu hình Timer và GPIO cho PWM điều khiển động cơ trái, phải và bàn chải,
 *          bao gồm hàm khởi tạo, điều chỉnh tần số và duty cycle.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef MOTORPWM_H
#define MOTORPWM_H

#include "stm32f4xx.h"

// Timer điều khiển PWM động cơ trái và phải
#define TIMER_PWM_MR	     TIM4
#define TIMER_PWM_ML	     TIM3

// GPIO và chân PWM cho động cơ trái và phải
#define MOTOR_PWM_PORT     GPIOB
#define ML_RPWM_PIN        GPIO_Pin_4
#define ML_LPWM_PIN        GPIO_Pin_5
#define MR_RPWM_PIN        GPIO_Pin_6
#define MR_LPWM_PIN        GPIO_Pin_7 

// Timer và GPIO cho động cơ quay chổi
#define TIMER_PWM_MB       TIM2
#define BRUSH_PWM_PORT     GPIOA
#define ENA_PIN            GPIO_Pin_0
#define IN1_PIN            GPIO_Pin_2
#define IN2_PIN            GPIO_Pin_4

// Tần số PWM mặc định 
extern int16_t frequency;

/**
 * @brief   Khởi tạo các kênh PWM cho động cơ và bàn chải
 * @return  void
 */
void init_PWM(void);

/**
 * @brief   Thiết lập tần số PWM
 * @param   freq  Tần số PWM mong muốn (Hz)
 * @return  void
 */
void Set_PWM_Freq(uint16_t freq);

/**
 * @brief   Thiết lập duty cycle PWM điều khiển tốc độ động cơ trái và phải
 * @param   speed1  Giá trị tốc độ động cơ trái
 * @param   speed2  Giá trị tốc độ động cơ phải
 * @return  void
 */
void Set_PWM_Duty(int16_t speed1, int16_t speed2);

#endif  // MOTORPWM_H