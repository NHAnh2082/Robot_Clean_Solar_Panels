#ifndef PIDCALCULATE_H
#define PIDCALCULATE_H

void PID_Init(float kp, float ki, float kd);
float PID_Update(float setpoint, float measurement);

#endif  // PIDCALCULATE_H