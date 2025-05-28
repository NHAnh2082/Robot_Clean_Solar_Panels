/******************************************************************************
 * @file    Battery.c
 * @brief   Định nghĩa các hàm cấu hình và đọc giá trị điện áp từ ADC
 * @details File này thực hiện cấu hình ADC1 kênh 1 và DMA2 Stream0 để đọc 
 *          điện áp từ chân ADC dùng STM32F4, tính toán điện áp thực tế và
 *          phần trăm pin tương ứng.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#include "stm32f4xx.h"
#include "DataConversion.h"
#include "Battery.h"

// Các hằng số cấu hình
static const float max_voltage = 12.60;		/**< Điện áp tối đa của pin */
const float R1 = 33000.0;					/**< Trở chia áp R1 */
const float R2 = 9800.0;					/**< Trở chia áp R2 */
const float Vref = 3.3;						/**< Điện áp tham chiếu ADC */

float out_voltage = 0;						/**< Điện áp đầu ra ADC tính theo V */
float in_voltage = 0;						/**< Điện áp thực tế sau chia áp */
uint16_t adc_value[ADC_BUFF_SIZE];			/**< Mảng lưu giá trị ADC */
int16_t percent = 0;						/**< Phần trăm pin */
uint8_t data_per[3];   						/**< Chuỗi ký tự phần trăm pin */

/**
 * @brief   Khởi tạo ADC1 trên kênh 1 sử dụng chế độ scan và chuyển đổi liên tục
 * @details Hàm cấu hình chân GPIO, ADC1, và bắt đầu chuyển đổi ADC với DMA
 * @note    Sử dụng ADC chuẩn 12 bit, tần số lấy mẫu 84 chu kỳ
 * @return  void
 */
static void init_adc1(void)
{		
	GPIO_InitTypeDef	GPIO_InitStructure;
	ADC_InitTypeDef	 	ADC_InitStructure;
	
	// Bật clock cho GPIOA và ADC1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); /* Enable GPIO Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);	/* Enable ADC clock */
	
	// Cấu hình chân ADC
    GPIO_InitStructure.GPIO_Pin   = ADC_PIN;
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
	
	/**
   	 * Resolution			12-bit		Cho ra giá trị 0–4095 ứng với 0–3.3V
     * ScanConvMode			ENABLE		Chuẩn bị sẵn cho nhiều kênh (nhưng đang dùng 1)
     * ContinuousConvMode 	ENABLE		ADC liên tục đo không cần kích hoạt lại
	 * Trigger				NONE		Không cần tín hiệu ngoài để kích hoạt
  	 * DataAlign			Right		Dữ liệu nằm ở 12 bit thấp, 4 bit cao bằng 0
	 * NbrOfConversion		1			Đang đo 1 kênh duy nhất (ADC_Channel_1)
     */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	// Cấu hình kênh ADC1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

	// Cho phép DMA sau chuyển đổi
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	// Bật DMA cho ADC1
	ADC_DMACmd(ADC1, ENABLE);

	// Bật ADC1
	ADC_Cmd(ADC1, ENABLE);

	// Bắt đầu chuyển đổi ADC bằng phần mềm
	ADC_SoftwareStartConv(ADC1);
}

/**
 * @brief   Khởi tạo DMA2 Stream0 cho ADC1, cấu hình chuyển dữ liệu từ ADC sang bộ nhớ
 * @details DMA hoạt động ở chế độ vòng (circular), kích thước 16 bit, ưu tiên cao,
 *          có ngắt khi chuyển xong.
 * @return  void
 */
static void init_adcDMA2(void)
{		
	DMA_InitTypeDef	  DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;	
	
	// Bật clock cho DMA2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/**
   	 * Địa chỉ nguồn	Thanh ghi dữ liệu ADC1 (ADC->DR)
     * Đích đến			Mảng adc_value[]
     * Chiều truyền 	Từ ngoại vi về bộ nhớ
	 * Mode				Circular → sau khi hết adc_value[], quay lại đầu
  	 * DataSize			16-bit (half-word) phù hợp với dữ liệu 12-bit của ADC
     */
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adc_value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = ADC_BUFF_SIZE;								
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	/* Default configure */
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	// Cấu hình ngắt DMA2 Stream0
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Bật ngắt hoàn thành truyền DMA
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
}

/**
 * @brief   Khởi tạo ADC và DMA
 * @details Hàm này gọi các hàm cấu hình ADC1 và DMA2 Stream0
 * @return  void
 */
void init_ADC(void)
{
	init_adc1();		// Khởi tạo ADC1 channel1
	init_adcDMA2();		// Khởi tạo DMA cho ADC
}

/**
 * @brief   Đọc giá trị ADC một lần (không dùng DMA)
 * @details Bắt đầu chuyển đổi ADC bằng phần mềm, chờ chuyển đổi xong,
 *          trả về giá trị ADC đọc được.
 * @return  uint16_t Giá trị ADC (0..4095)
 */
uint16_t read_adc(void)
{
	// Bắt đầu đọc 
	ADC_SoftwareStartConv(ADC1);
	
	// Chờ cờ chuyển đổi hoàn thành
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	// Trả về giá trị ADC
	return ADC_GetConversionValue(ADC1);
}

/**
 * @brief   Hàm xử lý ngắt DMA2 Stream0
 * @details Khi DMA hoàn thành truyền dữ liệu ADC vào bộ nhớ,
 *          hàm sẽ tính toán điện áp thực tế, phần trăm pin và chuyển
 *          phần trăm sang chuỗi ký tự để hiển thị.
 * @return  void
 */
void DMA2_Stream0_IRQHandler(void)
{
	// Kiểm tra cờ ngắt hoàn thành truyền
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		// Tính điện áp đầu ra ADC theo công thức
		out_voltage = adc_value[0] * Vref/4095.0;

		// Tính điện áp thực tế từ mạch chia áp
		in_voltage = out_voltage * (R1+R2)/R2;

		// Tính phần trăm pin, giá trị từ 0 đến 100
		percent = (int8_t)(in_voltage / max_voltage * 100.0);

		// Chuyển phần trăm thành chuỗi ký tự (vd: "100")
		IntToStr(percent, data_per, 3);

		// Xóa cờ ngắt
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}
