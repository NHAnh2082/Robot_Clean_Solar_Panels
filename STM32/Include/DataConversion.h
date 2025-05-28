/******************************************************************************
 * @file    DataConversion.h
 * @brief   Khai báo các hàm chuyển đổi giữa kiểu số và chuỗi ký tự
 * @details Cung cấp các hàm chuyển đổi như chuyển số nguyên sang chuỗi,
 *          số thực sang chuỗi, chuỗi sang số nguyên, chuỗi đặc biệt sang double.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#ifndef DATACONVERSION_H
#define DATACONVERSION_H

#include "stm32f4xx.h"

/**
 * @brief   Chuyển chuỗi số nguyên thành số nguyên kiểu int16_t
 * @param   length  Độ dài chuỗi đầu vào
 * @param   u       Mảng byte chứa ký tự số (ASCII)
 * @param   y       Con trỏ trả về giá trị số nguyên
 * @return  void
 */
void StrToInt(int length, uint8_t u[], int16_t *y);

/**
 * @brief   Chuyển chuỗi 7 ký tự sang kiểu double
 * @param   u       Mảng byte chứa chuỗi số dạng "x.xxxxxx"
 * @param   y       Con trỏ trả về giá trị double
 * @return  void
 */
void Str7ToDouble(uint8_t u[], double *y);

/**
 * @brief   Chuyển số thực (float) sang chuỗi ký tự với độ chính xác xác định
 * @param   value       Giá trị số thực cần chuyển đổi
 * @param   y           Mảng chứa chuỗi ký tự kết quả
 * @param   type        Kiểu dữ liệu (ví dụ 1: vận tốc, 2: khoảng cách)
 * @param   precision   Độ chính xác phần thập phân (số chữ số sau dấu '.')
 * @return  void
 */
void FloatToStr6(float value, uint8_t *y, int type, int precision);

/**
 * @brief   Chuyển số nguyên 32 bit sang chuỗi ký tự với chiều dài xác định
 * @param   u       Giá trị số nguyên cần chuyển
 * @param   y       Mảng chứa chuỗi ký tự kết quả
 * @param   length  Chiều dài chuỗi kết quả
 * @return  void
 */
void IntToStr(int32_t u, uint8_t *y, int8_t length);

#endif  // DATACONVERSION_H