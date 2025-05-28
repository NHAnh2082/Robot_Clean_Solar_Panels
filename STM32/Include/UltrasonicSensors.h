/******************************************************************************
 * @file    UltrasonicSensors.h
 * @brief   Khai báo các biến và hàm điều khiển 4 cảm biến siêu âm dùng Timer12
 * @details Module sử dụng 4 cảm biến siêu âm đặt trên robot để đo khoảng cách,
 *          sử dụng Timer12 để đo thời gian xung Echo.
 * @version 1.0
 * @date    2024-06-xx
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef ULTRASONICSENSORS_H
#define ULTRASONICSENSORS_H

#include "stm32f4xx.h"
#include <stdbool.h>

#define NUM_SENSORS     4           /**< Số lượng cảm biến siêu âm */
#define err_distance    1.5f        /**< Sai số cho phép khi so sánh khoảng cách (cm) */
#define max_distance    15.0f       /**< Khoảng cách tối đa đo được (cm) */

// Mảng con trỏ port và chân GPIO cho Echo và Trigger của từng sensor
extern GPIO_TypeDef* EchoPort[NUM_SENSORS];
extern GPIO_TypeDef* TrigPort[NUM_SENSORS];
extern uint16_t echoPins[NUM_SENSORS];
extern uint16_t trigPins[NUM_SENSORS];

// Trạng thái hoạt động của từng sensor
extern bool sensorStatus[NUM_SENSORS];

// Trạng thái hoạt động của từng sensor
extern float distance[NUM_SENSORS];
extern float init[NUM_SENSORS];
extern float curr[NUM_SENSORS];
extern int flag[NUM_SENSORS];

// Biến đo thời gian bắt đầu và kết thúc xung Echo
extern uint32_t start_time;
extern uint32_t stop_time;

Bộ đếm callback
extern int16_t cnt_cb;

// Mảng dữ liệu khoảng cách và cờ trạng thái truyền qua giao tiếp
extern uint8_t data_dis[NUM_SENSORS][5];
extern uint8_t data_flag[NUM_SENSORS][1];

/**
 * @brief   Khởi tạo chân GPIO cho sensor cụ thể
 * @param   sensorIndex    Chỉ số sensor [0..NUM_SENSORS-1]
 * @param   Trigger        Chân Trigger của sensor
 * @param   Echo           Chân Echo của sensor
 * @return  void
 */
void init_GPIO_Sensors(uint8_t sensorIndex, uint16_t Trigger, uint16_t Echo);

/**
 * @brief   Khởi tạo Timer12 để đo xung Echo từ sensor siêu âm
 * @return  void
 */
void init_Sensor_TIM12(void);

/**
 * @brief   Đo khoảng cách sensor siêu âm theo chỉ số sensorIndex
 * @param   sensorIndex    Chỉ số sensor [0..NUM_SENSORS-1]
 * @return  void
 */
void read_distance(uint8_t sensorIndex);

/**
 * @brief   Phát hiện thay đổi trạng thái sensor dựa trên khoảng cách
 * @param   init_dis    Khoảng cách ban đầu
 * @param   curr_dis    Khoảng cách hiện tại
 * @return  int         Trả về 0 nếu không thay đổi, 1 nếu có thay đổi hoặc vượt ngưỡng
 */
int detect_edge(float init_dis, float curr_dis);

/**
 * @brief   Cập nhật các cờ trạng thái cho tất cả sensor
 * @return  void
 */
void get_flag(void);

#endif  // ULTRASONICSENSORS_H