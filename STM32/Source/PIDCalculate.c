#include "PIDCalculate.h"

static float kp = 1.23f;    /**< Hằng số tỷ lệ PID */
static float ki = 0.001f;   /**< Hằng số tích phân PID */
static float kd = 0.00f;    /**< Hằng số đạo hàm PID */

static float pre_error, pre_pre_error;
static float integral, prev_output;

void PID_Init(float _kp, float _ki, float _kd) 
{
    kp = _kp; ki = _ki; kd = _kd;
    pre_error = 0; pre_pre_error = 0;
    integral = 0; prev_output = 0;
}

float PID_Update(float setpoint, float measurement) 
{
    float error = setpoint - measurement;

    float P = kp * (error - pre_error);
    integral += 0.5f * ki * (error + pre_error) * (T / 1000.0f);
    float D = kd * (error - 2*pre_error + pre_pre_error) / (T / 1000.0f);
    float output = prev_output + P + integral + D;
    
    // Cập nhật trạng thái
    pre_pre_error = pre_error;
    pre_error = error;
    prev_output = output;
    
    return output;
}
