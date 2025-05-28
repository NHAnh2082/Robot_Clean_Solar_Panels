/******************************************************************************
 * @file    Battery.h
 * @brief   Khai báo các biến và hàm sử dụng để đọc điện áp đầu vào từ pin ADC
 * @details File này khai báo các biến toàn cục và các hàm cấu hình ADC1 và DMA2
 *          dùng để đọc giá trị điện áp từ pin.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#ifndef BATTERY_H
#define BATTERY_H

#include "stm32f4xx.h"

// Chân GPIO sử dụng cho ADC
#define ADC_GPIO_PORT		GPIOA
#define ADC_PIN				GPIO_Pin_1

// Kích thước buffer ADC
#define ADC_BUFF_SIZE		1

// phần trăm pin
extern int16_t percent;

// điện áp đầu ra ADC tính theo V
extern float out_voltage;

// điện áp thực tế sau khi chia áp
extern float in_voltage;

// mảng chứa giá trị ADC đọc được
extern uint16_t adc_value[ADC_BUFF_SIZE];

// chuỗi ký tự phần trăm
extern uint8_t data_per[3];   

/**
 * @brief   Khởi tạo ADC và DMA
 * @details Hàm này gọi các hàm cấu hình ADC1 và DMA2 Stream0
 * @return  void
 */
void init_ADC(void);

/**
 * @brief   Đọc giá trị ADC một lần (không dùng DMA)
 * @details Bắt đầu chuyển đổi ADC bằng phần mềm, chờ chuyển đổi xong,
 *          trả về giá trị ADC đọc được.
 * @return  uint16_t Giá trị ADC (0..4095)
 */
uint16_t read_adc(void);

/**
 * @brief   Hàm xử lý ngắt DMA2 Stream0
 * @details Khi DMA hoàn thành truyền dữ liệu ADC vào bộ nhớ,
 *          hàm sẽ tính toán điện áp thực tế, phần trăm pin và chuyển
 *          phần trăm sang chuỗi ký tự để hiển thị.
 * @return  void
 */
void DMA2_Stream0_IRQHandler(void);

#endif  // BATTERY_H