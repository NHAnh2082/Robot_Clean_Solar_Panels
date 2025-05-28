/******************************************************************************
 * @file    UART.h
 * @brief   Khai báo các biến và hàm cấu hình UART4 và DMA cho truyền nhận
 * @details UART4 sử dụng DMA1 Stream4 Channel4 cho truyền và DMA1 Stream2 Channel4 cho nhận,
 *          bộ đệm truyền nhận cố định, xử lý ngắt DMA và tách dữ liệu theo giao thức.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"

#define BUFF_SIZE_RX      24   /**< Kích thước bộ đệm nhận (byte) */
#define BUFF_SIZE_TX      59   /**< Kích thước bộ đệm truyền (byte) */

// Bộ đệm truyền nhận
extern uint8_t rxbuff[BUFF_SIZE_RX];
extern uint8_t txbuff[BUFF_SIZE_TX];
extern uint8_t a[BUFF_SIZE_RX];

// Các biến lưu dữ liệu phân tích từ chuỗi nhận
extern uint8_t data_sp1[3];
extern uint8_t data_sp2[3];
extern int16_t wheel_speed;
extern int16_t brush_speed;
extern char dir_wheel[1];
extern char dir_brush[1];
extern char dir_auto[1];
extern char dir_pump[1];
extern char detect_dirt[1];
extern char detect_line[1];
extern uint8_t data_pulse[6];  // xung hiện tại: [0;999999]  unit: pulse
extern uint8_t data_speed[5];  // tốc độ robot: [0;0.169]   unit: m/s  (max speed = 23rpm = 0.169m/s)
extern uint8_t data_move[6];   // quãng đường di chuyển: [0;999.99]  unit: meter

extern uint16_t rcv_flag;
extern int16_t cnt_rcv;

/**
 * @brief   Khởi tạo UART4 và DMA truyền nhận
 * @return  void
 */
void init_UART(void);

/**
 * @brief   Xử lý ngắt DMA1 Stream2 khi nhận dữ liệu xong
 * @return  void
 */
void DMA1_Stream2_IRQHandler(void);

/**
 * @brief   Tách chuỗi nhận được thành các trường dữ liệu riêng biệt
 * @return  void
 */
void SplitStr(void);

#endif  // UART_H