#include "stm32f4xx.h"
#include "Conversion.h"
#include "ADC.h"

const float max_voltage = 12.60;
const float R1 = 33000.0;
const float R2 = 9800.0;
const float Vref = 3.3;
float out_voltage = 0;
float in_voltage = 0;
uint16_t adc_value[ADC_BUFF_SIZE];
int16_t percent = 0;
uint8_t data_per[3];   // percent of battery: 	[0;100]		unit: %

//------------------------------------------------------------
static void init_adc1(void){		
	GPIO_InitTypeDef	GPIO_InitStructure;
	ADC_InitTypeDef	 	ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); /* Enable GPIO Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,  ENABLE);	/* Enable ADC clock */
	
	/* GPIO Configuration for ADC1*/
  GPIO_InitStructure.GPIO_Pin   = ADC_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
	
	/* ADC Configuration */
	ADC_InitStructure.ADC_Resolution 			 = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode 		 = ENABLE;
	ADC_InitStructure.ADC_DataAlign        = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion  = 1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ContinuousConvMode 	 = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	/* Start ADC conversion */
	ADC_SoftwareStartConv(ADC1);
}

//------------------------------------------------------------
static void init_adcDMA2(void){		
	DMA_InitTypeDef	 	DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;	
	
	/* Enable DMA2 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/* DMA2 Stream0 Channel0 for ADC1 configuration */			
  DMA_InitStructure.DMA_Channel 					 = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr 	 = (uint32_t)&adc_value;
	DMA_InitStructure.DMA_DIR 							 = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize 				 = ADC_BUFF_SIZE;								
  DMA_InitStructure.DMA_PeripheralInc 		 = DMA_PeripheralInc_Disable;	
  DMA_InitStructure.DMA_MemoryInc 				 = DMA_MemoryInc_Enable;			
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize 	 	 = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode 							 = DMA_Mode_Circular; 
  DMA_InitStructure.DMA_Priority 					 = DMA_Priority_High;
	
	/* Default configure */
  DMA_InitStructure.DMA_FIFOMode 				= DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold 	= DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
	
	/* Enable DMA Interrupt to the highest priority */
  NVIC_InitStructure.NVIC_IRQChannel 									 = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 			 = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd 							 = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Transfer complete interrupt mask */
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
}

//------------------------------------------------------------
void init_ADC(void){
	init_adc1();			// Initialize ADC1 channel1
	init_adcDMA2();		// Initialize DMA for ADC
}

//------------------------------------------------------------
uint16_t read_adc(void){
	/* Start ADC conversion */
	ADC_SoftwareStartConv(ADC1);
	/* Wait until conversion is High */
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

//------------------------------------------------------------
void DMA2_Stream0_IRQHandler(void){
	/* Kiem tra ngat DMA */
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)){
		out_voltage = adc_value[0] * Vref/4095.0;
		in_voltage = out_voltage * (R1+R2)/R2;
		percent = (int8_t)(in_voltage / max_voltage * 100.0);
		IntToStr(percent, data_per, 3);
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}
