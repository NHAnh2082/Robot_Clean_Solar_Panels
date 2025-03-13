#include "stm32f4xx.h"

#define	BUFF_SIZE_RX		 		  24 // byte
#define BUFF_SIZE_TX					59

extern uint8_t rxbuff[BUFF_SIZE_RX];
extern uint8_t txbuff[BUFF_SIZE_TX];
extern uint8_t a[BUFF_SIZE_RX];

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
extern uint8_t data_pulse[6]; // current pulse: 					[0;999999]		unit: pulse
extern uint8_t data_speed[5]; // speed of robot:					[0;0.169]			unit: m/s			(max speed = 23rpm = 0.169m/s)
extern uint8_t data_move[6];	 // distance move of robot: [0;999.99]		unit: meter

extern uint16_t rcv_flag;
extern int16_t cnt_rcv;

void init_UART(void);
void DMA1_Stream2_IRQHandler(void);
void SplitStr(void);
