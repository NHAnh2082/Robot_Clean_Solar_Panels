/******************************************************************************
 * @file    PIDCalculate.h
 * @brief   Module tính toán thuật toán PID
 * @details Cung cấp hàm khởi tạo tham số PID và cập nhật giá trị điều khiển PID
 *          dựa trên sai số đầu vào (setpoint - measurement).
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#ifndef PIDCALCULATE_H
#define PIDCALCULATE_H

/**
 * @brief   Khởi tạo tham số cho bộ điều khiển PID
 * @param   _kp   Hằng số tỷ lệ (Proportional gain)
 * @param   _ki   Hằng số tích phân (Integral gain)
 * @param   _kd   Hằng số đạo hàm (Derivative gain)
 * @return  void
 * @note    Hàm này reset các trạng thái trước đó của PID
 */
void PID_Init(float kp, float ki, float kd);

/**
 * @brief   Cập nhật bộ điều khiển PID và tính giá trị điều khiển mới
 * @param   setpoint     Giá trị mục tiêu (đặt)
 * @param   measurement  Giá trị đo thực tế
 * @return  Giá trị điều khiển PID (output)
 * @details
 *  - Thuật toán PID dạng rời rạc sử dụng sai phân sai số
 *  - Tính toán phần P, I, D và cộng dồn với giá trị output trước đó
 *  - Cập nhật trạng thái để sử dụng ở lần gọi tiếp theo
 * @note    Biến T là khoảng thời gian chu kỳ tính toán (ms), phải khai báo ngoài module
 */
float PID_Update(float setpoint, float measurement);

#endif  // PIDCALCULATE_H