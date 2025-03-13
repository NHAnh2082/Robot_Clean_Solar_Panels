/***** Use ADC & DMA to read the input voltage *****/

#include "stm32f4xx.h"

#define ADC_GPIO_PORT		GPIOA
#define ADC_PIN					GPIO_Pin_1
#define ADC_BUFF_SIZE		1

extern int16_t percent;
extern float out_voltage;
extern float in_voltage;
extern uint16_t adc_value[ADC_BUFF_SIZE];
extern uint8_t data_per[3];   // percent of battery: 		[0;100]				unit: %

void init_ADC(void);
uint16_t read_adc(void);
void DMA2_Stream0_IRQHandler(void);
