/******************************************************************************
 * @file    UART.c
 * @brief   Định nghĩa hàm cấu hình UART4, DMA và xử lý ngắt DMA nhận
 * @details Cấu hình GPIO PC10, PC11, UART4, DMA1 Stream4/Stream2,
 *          xử lý ngắt DMA nhận, phân tích dữ liệu truyền nhận.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>
#include "DataConversion.h"
#include "UART.h"

Định nghĩa biến bộ đệm và dữ liệu
uint8_t rxbuff[BUFF_SIZE_RX];
uint8_t txbuff[BUFF_SIZE_TX];
uint8_t a[BUFF_SIZE_RX];
int16_t wheel_speed = 0;
int16_t brush_speed = 0;
uint8_t data_sp1[3];
uint8_t data_sp2[3];
char dir_wheel[1];
char dir_brush[1];
char dir_auto[1];
char dir_pump[1];
char detect_dirt[1];
char detect_line[1];

uint8_t data_pulse[6];  // current pulse:          [0;999999]  unit: pulse
uint8_t data_speed[5];  // speed of robot:         [0;0.169]   unit: m/s  (max speed = 23rpm = 0.169m/s)
uint8_t data_move[6];   // distance move of robot: [0;999.99]  unit: meter

uint16_t rcv_flag = 0;
int16_t cnt_rcv = 0;

/**
 * @brief   Cấu hình UART4, GPIO PC10 (TX), PC11 (RX)
 * @return  void
 */
static void init_UART4(void)
{	
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	// Bật clock cho GPIOC và UART4
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	// Cấu hình GPIO cho UART Tx & Rx
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); 
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
       
	/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	
	// Cấu hình UART4
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = DISABLE;
	USART_Init(UART4, &USART_InitStructure);

	// Bật UART
	USART_Cmd(UART4, ENABLE);
}

/**
 * @brief   Khởi tạo DMA1 Stream4 Channel4 cho UART4 truyền dữ liệu
 * @return  void
 */
static void init_txDMA1(void)
{	
	DMA_InitTypeDef	 DMA_InitStructure;
	
	/* Enable DMA1 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	/* Enable DMA2 clock */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/* DMA1 Stream4 Channel4 for UART4 Tx configuration */			
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)txbuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = BUFF_SIZE_TX;								
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// Default configure
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
	
	/* Enable DMA1, Stream 4 */
	DMA_Cmd(DMA1_Stream4, ENABLE);
	
	/* Enable UART Tx DMA */
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
}

/**
 * @brief   Khởi tạo DMA1 Stream2 Channel4 cho UART4 nhận dữ liệu
 * @return  void
 */
static void init_rxDMA1(void)
{	
	DMA_InitTypeDef	 	DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;	
	
	/* Enable DMA1 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	/* Enable DMA2 clock */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/* DMA1 Stream2 Channel4 for UART4 Rx configuration */			
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rxbuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = BUFF_SIZE_RX;								
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// Default configure
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE);
	
	/* Enable UART Rx DMA */
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
	
	/* Enable DMA Interrupt to the highest priority */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Transfer complete interrupt mask */
	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
}

/**
 * @brief   Khởi tạo UART4 và DMA truyền nhận
 * @return  void
 */
void init_UART(void)
{
	init_UART4();	// Initialize UART4
	init_txDMA1();	// Initialize DMA for UART transmit
	init_rxDMA1();	// Initialize DMA for UART receive
}

/**
 * @brief   Xử lý ngắt DMA1 Stream2 khi nhận dữ liệu xong
 * @details Kiểm tra gói dữ liệu hợp lệ, cập nhật cờ nhận, gọi tách chuỗi,
 *          nếu dữ liệu sai reset hệ thống.
 * @return  void
 */
void DMA1_Stream2_IRQHandler(void)
{
	uint8_t i;

	/* Kiem tra ngat DMA */
	if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2))
	{
		DMA_Cmd(DMA1_Stream2, DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
		cnt_rcv++;
		
		if ((rxbuff[0] == 'M') && (rxbuff[BUFF_SIZE_RX-1] == '\n'))
		{
			for(i=0; i<BUFF_SIZE_RX; i++) a[i] = rxbuff[i];
			rcv_flag = 1;
		}
		else
		{
			rcv_flag = 2;
		}
		
		if (rcv_flag)
		{
			SplitStr();
			StrToInt(3,data_sp1,&wheel_speed);
			StrToInt(3,data_sp2,&brush_speed);
			rxbuff[BUFF_SIZE_RX-2] = 0x0D;
			rxbuff[BUFF_SIZE_RX-1] = 0x0A;
		}
		else if (rcv_flag == 2)
		{
			memset(rxbuff, 0, sizeof(rxbuff));
			NVIC_SystemReset();
		}

		DMA_Cmd(DMA1_Stream2, ENABLE);
	}
}

/**
 * @brief   Tách chuỗi dữ liệu nhận được thành các biến dữ liệu riêng biệt
 * @return  void
 */
void SplitStr(void)
{
	int cnt = 0;
	int index = -1;
	uint8_t tempbuff[BUFF_SIZE_RX];
	for (int i=0; i<BUFF_SIZE_RX; i++) tempbuff[i] = rxbuff[i];
	for (int i=0; i<BUFF_SIZE_RX; i++)
	{
		if ((tempbuff[i] == '_') && (tempbuff[i-1] == 'M'))
		{
			index = 1;
			cnt = 0;
		}
		else if ((tempbuff[i] == '_') && (tempbuff[i-1] != 'M'))
		{
			cnt++;
			switch (cnt)
			{
				case 1: /* Navigate for robot */
					for (int j=index+1; j<i; j++) dir_wheel[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 2: /* Get wheel speed */
					for (int j=index+1; j<i; j++) data_sp1[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 3: /* Turn on/off brush */
					for (int j=index+1; j<i; j++) dir_brush[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 4: /* Get brush speed */
					for (int j=index+1; j<i; j++) data_sp2[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 5: /* Turn on/off auto mode */
					for (int j=index+1; j<i; j++) dir_auto[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 6: /* Turn on/off water pump */
					for (int j=index+1; j<i; j++) dir_pump[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 7: /* Detect line */
					for (int j=index+1; j<i; j++) detect_line[j-index-1] = tempbuff[j];
					index = i;
					break;

				case 8: /* Detect dirt */
					for (int j=index+1; j<i; j++) detect_dirt[j-index-1] = tempbuff[j];
					index = i;
					break;

				default:
					break;
			}
		}
		else if ((tempbuff[i] == 0x0D) && (tempbuff[i+1] == 0x0A)) return;
	}
}
