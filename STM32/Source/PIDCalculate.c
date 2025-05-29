/******************************************************************************
 * @file    PIDCalculate.h
 * @brief   Module tính toán thuật toán PID
 * @details Cung cấp hàm khởi tạo tham số PID và cập nhật giá trị điều khiển PID
 *          dựa trên sai số đầu vào (setpoint - measurement).
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#include "PIDCalculate.h"

static float kp = 1.23f;    /**< Hằng số tỷ lệ PID */
static float ki = 0.001f;   /**< Hằng số tích phân PID */
static float kd = 0.00f;    /**< Hằng số đạo hàm PID */

static int T = 100;

static float pre_error, pre_pre_error;
static float integral, prev_output;

/**
 * @brief   Khởi tạo tham số cho bộ điều khiển PID
 * @param   _kp   Hằng số tỷ lệ (Proportional gain)
 * @param   _ki   Hằng số tích phân (Integral gain)
 * @param   _kd   Hằng số đạo hàm (Derivative gain)
 * @return  void
 * @note    Hàm này reset các trạng thái trước đó của PID
 */
void PID_Init(float _kp, float _ki, float _kd) 
{
    kp = _kp; ki = _ki; kd = _kd;
    pre_error = 0; pre_pre_error = 0;
    integral = 0; prev_output = 0;
}

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
float PID_Update(float setpoint, float measurement) 
{
    // sai số
    float error = setpoint - measurement;

    // tính toán các hệ số P,I,D
    float P = kp * (error - pre_error);
    integral += 0.5f * ki * (error + pre_error) * (T / 1000.0f);
    float D = kd * (error - 2*pre_error + pre_pre_error) / (T / 1000.0f);

    // Tính output PID dựa trên output trước đó cộng P, I, D
    float output = prev_output + P + integral + D;
    
    // Cập nhật trạng thái
    pre_pre_error = pre_error;
    pre_error = error;
    prev_output = output;
    
    return output;
}
