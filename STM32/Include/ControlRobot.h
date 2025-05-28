#ifndef CONTROLROBOT_H
#define CONTROLROBOT_H

#include <stdint.h>
#include "UART.h"

/**
 * @brief   Di chuyển robot về phía trước với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void move_Forward(int16_t speed);

/**
 * @brief   Di chuyển robot lùi với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void move_Backward(int16_t speed);

/**
 * @brief   Quay robot sang trái với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void turn_Left(int16_t speed);

/**
 * @brief   Quay robot sang phải với tốc độ cho trước
 * @param   speed   Tốc độ (0 - 23)
 */
void turn_Right(int16_t speed);

/**
 * @brief   Dừng toàn bộ robot, tắt các LED báo hiệu
 */
void stop_Robot(void);

/**
 * @brief   Chạy bàn chải với tốc độ cho trước
 * @param   speed   Tốc độ bàn chải (0 - 53)
 */
void run_Brush(int16_t speed);

/**
 * @brief   Đảo chiều quay bàn chải với tốc độ cho trước
 * @param   speed   Tốc độ bàn chải (0 - 28)
 */
void reverse_Brush(int16_t speed);

/**
 * @brief   Dừng bàn chải và ngắt PWM
 */
void stop_Brush(void);


/**
 * @brief   Chạy chế độ tự động robot với logic xử lý dựa trên cảm biến bụi bẩn
 * @details Robot sẽ tự động đi tới, dừng lại vệ sinh khu vực bẩn, sau đó tiếp tục di chuyển.
 */
void run_auto(void);

#endif  // CONTROLROBOT_H