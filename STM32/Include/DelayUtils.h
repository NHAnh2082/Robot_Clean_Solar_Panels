/******************************************************************************
 * @file    DelayUtils.h
 * @brief   Khai báo các hàm delay bằng Timer6 trên STM32F4
 * @details Cung cấp các hàm delay theo đơn vị 0.1ms và microsecond sử dụng Timer6
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef DELAYUTILS_H
#define DELAYUTILS_H

#include "stm32f4xx.h"

/**
 * @brief   Delay với độ chính xác 0.1 ms (100 us)
 * @param   period  Thời gian delay, tính theo đơn vị 0.1 ms
 * @return  void
 */
void delay_01ms(uint16_t period);

/**
 * @brief   Delay với độ chính xác micro giây
 * @param   period  Thời gian delay, tính theo micro giây
 * @return  void
 */
void delay_us(uint16_t period);

#endif  // DELAYUTILS_H