#include "stm32f4xx.h"
#include "system_timetick.h"
#include "math.h"
#include "ADC.h"
#include "PWM.h"
#include "Encoder.h"
#include "Ultrasonic04.h"
#include "Periph.h"
#include "UART.h"
#include "Delay.h"
#include "Conversion.h"

float velo1, velo2, distance_move;
float angle 		= 0.0;
float angle_ref = 0.0;
float pre_pre_error = 0.0;
float pre_error 		 = 0.0;
float error 				 = 0.0;
float pre_angle 		 = 0.0;
float cur_angle 		 = 0.0;
float speed = 0.0;
float pre_speed = 0.0;
float P_part, I_part, D_part;
float T = 100.0; // period
volatile int32_t p_alpha = 0;
volatile int32_t current_pulse = 0;
float w = 0.92, l = 0.65;

uint8_t data_kp[7];
uint8_t data_ki[7];
uint8_t data_kd[7];

//float kp = 1.0085;
//float ki = 0.000034;
//float kd = 0.000022;
float kp = 1.23;		// on dinh tai 11rpm
float ki = 0.001;
float kd = 0;
int32_t p = 0, pre_p = 0, dp = 0;
int cur_pulse = 0;
int auto_pulse = 0, auto1_pulse = 0, auto2_pulse = 0;
int pulse = 0;
int isMove1 = 0;
int isMove2 = 0;
int16_t cnt_tim5;
int16_t cnt_detect;

void MergeStr(void);
void move_Forward(int16_t speed);
void move_Backward(int16_t speed);
void turn_Left( int16_t speed);
void turn_Right(int16_t speed);
void stop_Robot(void);
void stop_Brush(void);
void stop_Pump(void);
void run_Brush(int16_t speed);
void reverse_Brush(int16_t speed);
void run_auto(void);
void run_Pump(void);
int32_t readEncPos(int16_t angle);
void Enc_Pulse(int32_t pulse);
void calculateAnglePos(int32_t pulse);
void calculatePID(float sp, float current_speed);
void init_TIM5_SampleTime(void);
void TIM5_IRQHandler(void);

uint16_t t1=0,t2=0,t3=0,t4=0;

int main(void){
	/* Enable SysTick at 10ms interrupt */
	SysTick_Config(SystemCoreClock/100);
	/* Configure timer for PWM and encoder */
	init_PWM();
	init_ENCODER();
	Set_PWM_Freq(20000);	// Set up frequency is 20kHz
	stop_Robot();			 // Default configuration for robot
	stop_Brush();			 // Default configuration for brush
	
	init_ADC();		
	init_UART();
	init_PERIPHERAL();
	init_TIM5_SampleTime();
	
	/* Configure GPIO and timer for 4 HCSR04 */
	for (uint8_t i=0; i<NUM_SENSORS; i++){
		init_GPIO_Sensors(i, trigPins[i], echoPins[i]);
	}
	init_Sensor_TIM12();
	
	
	
	/* Disable timer */
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM8->CR1 &= ~TIM_CR1_CEN;
	
	/* Starting Timer */
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM8->CR1 |= TIM_CR1_CEN;

//	move_Forward(15);
	delay_01ms(100);

	while(1){		
		IntToStr(cur_pulse, data_pulse, 6);
		FloatToStr6(velo1, data_speed, 1, 3);
		FloatToStr6(distance_move, data_move, 2, 2);

		p_alpha = readEncPos(90);	
		
		
		cnt_cb++;
		for (uint8_t i=0; i<NUM_SENSORS; i++){
			read_distance(i);
		}
		delay_01ms(100);
		if (cnt_cb <= 5){
			for(uint8_t i=0; i<NUM_SENSORS; i++){
				init[i] = distance[i];
			}
		}
		else if (cnt_cb >= 10){
			for(uint8_t i=0; i<NUM_SENSORS; i++){
				curr[i] = distance[i];
				FloatToStr6(curr[i], data_dis[i], 2, 2);
			}
		}
		get_flag();
		for (uint8_t i=0; i<NUM_SENSORS; i++){
			IntToStr(flag[i], data_flag[i], 1);
		}
		
		/* Direction */
		switch(dir_wheel[0]){
			case 'S':
				stop_Robot();
				break;
			case 'F':
//				if (dir_brush[0] == '0') stop_Brush();
//				else if (dir_brush[0] == '1') run_Brush(brush_speed);
			
//				if ((detect_line[0] == '0' && detect_dirt[0] == '0')){
//					move_Forward(wheel_speed);
//				}
//				else if ((detect_line[0] == '0' && detect_dirt[0] == '1')) {
//					move_Forward(wheel_speed/2.0);
//				}
//				else if ((detect_line[0] == '1' && (detect_dirt[0] == '0' || detect_dirt[0] == '1'))) {
//					stop_Robot();
//				}
					if (detect_dirt[0] == '0') move_Forward(wheel_speed);
					else stop_Robot();
				break;
			case 'B':
//				if (dir_brush[0] == '0') stop_Brush();
//				else if (dir_brush[0] == '1') reverse_Brush(brush_speed);
					move_Backward(wheel_speed);
					
				break;
			case 'L':
				turn_Left(wheel_speed);
				break;
			case 'R':
				turn_Right(wheel_speed);
				break;
			default:
				break;
		}
		
		switch(dir_brush[0]){
			case '0':
				stop_Brush();
				break;
			case '1':
				run_Brush(brush_speed);
				break;
			default:
				break;
		}
		
		switch(dir_pump[0]){
			case '0':
				GPIO_SetBits(RELAY_PORT, RELAY_GPIO);
				break;
			case '1':
				GPIO_ResetBits(RELAY_PORT, RELAY_GPIO);
				break;
		}
		
		switch(dir_auto[0]){
			case '0':
				break;
			case '1':
				run_auto();
				break;
		}
		
		if ((dir_auto[0] == '1') || (dir_wheel[0] == 'F' || dir_wheel[0] == 'B' || dir_wheel[0] == 'R' || dir_wheel[0] == 'L')){
			p = (int32_t)TIM1->CNT;
			IntToStr(p, data_pulse, 5);
		}
	
		
		/* Merge string and save data into txbuff */
		MergeStr();
		delay_01ms(100);
	}	
}


//------------------------------------------------------------
void Enc_Pulse(int32_t pulse){
	dp = pulse - pre_p;
	if (dp >= 0) 		 cur_pulse +=  dp;
	else if (dp < 0) cur_pulse += -dp;
	if (dp > 32768) dp -= 65536;
	else if (dp < -32768) dp += 65536;
//	cur_pulse += dp;
	pre_p = pulse;
	auto_pulse += dp;
}

//------------------------------------------------------------
void calculateAnglePos(int32_t pulse){
	cur_angle = (pulse * 360)/(PULSE_PER_REVOLUTION * ENCODER_mode);
	
	if (pulse > 360){
		pulse = pulse % (PULSE_PER_REVOLUTION * ENCODER_mode);
		cur_angle = (pulse * 360)/(PULSE_PER_REVOLUTION * ENCODER_mode);
	}
	else if (cur_angle < -360){
		pulse = pulse % (PULSE_PER_REVOLUTION * ENCODER_mode);
		cur_angle = (pulse * 360)/(PULSE_PER_REVOLUTION * ENCODER_mode);
	}
}

//------------------------------------------------------------
void calculatePID(float sp, float current_speed){
	error = sp - current_speed;
	P_part = kp  * (error - pre_error);
	I_part = 0.5 * ki * T/1000.0 * (error + pre_error);
	D_part = (kd * (error - 2 * pre_error + pre_pre_error))/(T/1000.0);
	speed = pre_speed + P_part + I_part + D_part;
	
	pre_pre_error = pre_error;
	pre_error			= error;
	pre_speed 		= speed;		
	
	if (speed > 23) 		speed = 23;
	else if (speed < 0) speed = 0;
	move_Forward(speed);
}

//------------------------------------------------------------
void init_TIM5_SampleTime(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef  			 NVIC_InitStructure;	

	/* Enable clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,  ENABLE);
	
	/* Configure parameters */
	TIM_TimeBaseStructure.TIM_Period 		  = 10000 - 1; 
	TIM_TimeBaseStructure.TIM_Prescaler   = 8400 - 1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	/* Clear interrupt update flag 1 */
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	
	/* Cau hinh ngat cho Timer 5 */
	NVIC_InitStructure.NVIC_IRQChannel 									 = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			 = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd 							 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//------------------------------------------------------------
void TIM5_IRQHandler(void){
	if (TIM_GetITStatus(TIM5, TIM_IT_Update)){
		p = (int32_t)TIM1->CNT;
		Enc_Pulse(p);
		velo1 = (float)((float)dp /(PULSE_PER_REVOLUTION * ENCODER_mode) * (float)(0.14*3.14));	// m/s
		velo2 = (float)((float)dp /(PULSE_PER_REVOLUTION * ENCODER_mode) * 60.0);						  // rpm
		distance_move = (float)((float)cur_pulse / (PULSE_PER_REVOLUTION * ENCODER_mode) * (float)(0.14*3.14));
		
		
		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
		DMA1_Stream4->NDTR = BUFF_SIZE_TX;
		DMA_Cmd(DMA1_Stream4, ENABLE);		
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}

//------------------------------------------------------------
void MergeStr(void){
	txbuff[0]  = 'D';
	txbuff[1]  = '_';
	for (int16_t i=0; i<3; i++){
		txbuff[i+2] = data_per[i];		// percent
	}
	txbuff[5]  = '_';
	for (int16_t i=0; i<6; i++){
		txbuff[i+6] = data_pulse[i];	// pulse
	}
	txbuff[12] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+13] = data_speed[i]; // speed
	}
	txbuff[18] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+19] = data_move[i]; // distance move
	}
	txbuff[24] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+25] = data_dis[0][i]; // distance 1
	}
	txbuff[30] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+31] = data_dis[1][i]; // distance 2
	}
	txbuff[36] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+37] = data_dis[2][i]; // distance 3
	}
	txbuff[42] = '_';
	for (int16_t i=0; i<5; i++){
		txbuff[i+43] = data_dis[3][i]; // distance 4
	}
	txbuff[48] = '_';
	txbuff[49] = data_flag[0][0];
	txbuff[50] = '_';
	txbuff[51] = data_flag[1][0];
	txbuff[52] = '_';
	txbuff[53] = data_flag[2][0];
	txbuff[54] = '_';
	txbuff[55] = data_flag[3][0];
	txbuff[56] = '_';
	txbuff[57] = '\r';
	txbuff[58] = '\n';
}
//------------------------------------------------------------
int32_t readEncPos(int16_t angle){
	int32_t pulse_angle = 0;
	pulse_angle = (int32_t)((PULSE_PER_REVOLUTION * 4.0)/0.44 * angle/360.0 * 3.14 * sqrt(w*w + l*l));
	return pulse_angle;
}
//------------------------------------------------------------
void move_Forward(int16_t speed){
	GPIO_SetBits(	 LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	if (speed > 23) speed = 23;
	TIM3->CCR1 = (uint16_t)((speed/23.0) * (TIM3->ARR+1)); 
	TIM3->CCR2 = 0;
	
	TIM4->CCR1 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
	TIM4->CCR2 = 0;
	
	t1 = TIM3->CCR1;
	t2 = TIM3->CCR2;
	t3 = TIM4->CCR1;
	t4 = TIM4->CCR2;
}

//------------------------------------------------------------
void move_Backward(int16_t speed){
	GPIO_SetBits(	 LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);

	if (speed > 23) speed = 23;	
	TIM3->CCR1 = 0;
	TIM3->CCR2 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
	
	TIM4->CCR1 = 0;
	TIM4->CCR2 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
}

//------------------------------------------------------------
void turn_Left(int16_t speed){
	GPIO_SetBits(	 LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	if (speed > 23) speed = 23;	
	// motor right speed
	TIM4->CCR1 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
	TIM4->CCR2 = 0;
		
	// motor left speed
	TIM3->CCR1 = 0;
	TIM3->CCR2 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
}

//------------------------------------------------------------
void turn_Right(int16_t speed){
	GPIO_SetBits(	 LED_GPIO_PORT, LED_RIGHT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	
	if (speed > 23) speed = 23;	
	// motor right speed
	TIM4->CCR1 = 0;
	TIM4->CCR2 = (uint16_t)((speed/23.0) * (TIM4->ARR+1));
		
	// motor left speed
	TIM3->CCR1 = (uint16_t)((speed/23.0) * (TIM3->ARR+1));
	TIM3->CCR2 = 0;
}

//------------------------------------------------------------
void stop_Robot(void){
	GPIO_ResetBits(LED_GPIO_PORT, LED_LEFT);
	GPIO_ResetBits(LED_GPIO_PORT, LED_FORWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_BACKWARD);
	GPIO_ResetBits(LED_GPIO_PORT, LED_RIGHT);
	
	TIM3->CCR1 = 0; TIM3->CCR2 = 0;
	TIM4->CCR1 = 0; TIM4->CCR2 = 0;
}

//------------------------------------------------------------
void run_Brush(int16_t speed){
	if (speed > 53) speed = 53;
	else if (speed == 0){
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	}
	else{
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_SetBits(  BRUSH_PWM_PORT, IN2_PIN);
		TIM2->CCR1 = (uint16_t)((speed/53.0) * (TIM2->ARR+1));
	}
}
//------------------------------------------------------------
void reverse_Brush(int16_t speed){
	if (speed > 28) speed = 28;
	else if (speed == 0){
		GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	}
	else{
		GPIO_SetBits(  BRUSH_PWM_PORT, IN1_PIN);
		GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
		TIM2->CCR1 = (uint16_t)((speed/53.0) * (TIM2->ARR+1));
	}
}

//------------------------------------------------------------
void stop_Brush(void){
	GPIO_ResetBits(BRUSH_PWM_PORT, IN1_PIN);
	GPIO_ResetBits(BRUSH_PWM_PORT, IN2_PIN);
	TIM2->CCR1 = 0;
}

//------------------------------------------------------------
void run_auto(void){
// 34642 thang 1 - 17300 trai 1 - 3856 thang 2 - 17476 trai 2 
// 34650 thang 3 - 12239 phai 1 - 5872 thang 4 - <17225> phai 2
// 35475 thang 5 - <14000> trai 3 - 7591 thang 6 - 18746 trai 4
	
	
	stop_Brush();
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	do{
		if (detect_dirt[0] == '0'){
			cnt_detect = 0;
			move_Forward(wheel_speed);
		}
		else if (detect_dirt[0] == '1'){
			cnt_detect++;
			stop_Robot();
			delay_01ms(10000);
			if (cnt_detect <= 2){
				auto1_pulse = auto_pulse;
				do{
					move_Forward(wheel_speed);
				}
				while (auto_pulse - auto1_pulse <= 5798);
				stop_Robot();
				delay_01ms(10000);
				run_Brush(brush_speed);
				delay_01ms(30000);
				stop_Brush();
				
				auto2_pulse = auto_pulse;
				do{
					move_Backward(wheel_speed);
				}
				while (auto2_pulse - auto_pulse <= 5798); 
				stop_Robot();
				delay_01ms(10000);
			}
			else{
				move_Forward(wheel_speed);
				delay_01ms(25000);
				cnt_detect = 0;
			}
		}
	}
	while (auto_pulse < 33269); //32960 // 33577
	stop_Robot();
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);
	
	do{
		turn_Left(wheel_speed);
	}
	while (auto_pulse > (65535 - 22786)); // 22786 // 21990 // 22333
	stop_Robot();	
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	delay_01ms(10000);

	do{
		move_Forward(wheel_speed);
		run_Brush(brush_speed);
	}
	while (auto_pulse < 1820); //1810 // 1760 // 1820 2044
	stop_Brush();
	stop_Robot();	
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);

	do{
		turn_Left(wheel_speed);
	}
	while (auto_pulse > (65535 - 20121)); // 19504 // 20121 23668
	stop_Robot();	
	TIM1->CNT = 0;
	pre_p = 0;
	auto_pulse = 0;
	delay_01ms(10000);
	
	do{
		if (detect_dirt[0] == '0'){
			cnt_detect = 0;
			move_Forward(wheel_speed);
		}
		else if (detect_dirt[0] == '1'){
			cnt_detect++;
			stop_Robot();
			delay_01ms(10000);
			if (cnt_detect <= 2){
				auto1_pulse = auto_pulse;
				do{
					move_Forward(wheel_speed);
				}
				while (auto_pulse - auto1_pulse <= 6023);
				stop_Robot();
				delay_01ms(10000);
				run_Brush(brush_speed);
				delay_01ms(30000);
				stop_Brush();
				
				auto2_pulse = auto_pulse;
				do{
					move_Backward(wheel_speed);
				}
				while (auto2_pulse - auto_pulse <= 6023); 
				stop_Robot();
				delay_01ms(10000);
			}
			else{
				move_Forward(wheel_speed);
				delay_01ms(25000);
				cnt_detect = 0;
			}
		}
	}
	while (auto_pulse < 30593); //32960 // 33577
	stop_Robot();
	TIM1->CNT = 65535;
	pre_p = 0;
	auto_pulse = 65535;
	delay_01ms(10000);
	
	//--------------------------------------------------------------------
//	stop_Brush();
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	do{
//		move_Forward(wheel_speed);
//	}
//	while (auto_pulse < 33412);
//	stop_Robot();
//	TIM1->CNT = 65535;
//	pre_p = 0;
//	auto_pulse = 65535;
//	delay_01ms(10000);
//	
//	do{
//		turn_Left(wheel_speed);
//	}
//	while (auto_pulse > (65535 - 22786)); // 22786 // 21990 // 22333
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 1820); //1810 // 1760 // 1820 2044
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 65535;
//	pre_p = 0;
//	auto_pulse = 65535;
//	delay_01ms(10000);
//	
//	
//	do{
//		turn_Left(wheel_speed);
//	}
//	while (auto_pulse > (65535 - 23668)); // 19504 // 20121 23668
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	//////////
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 35093); // 34845 // 35093 // 34456
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	do{
//		turn_Right(wheel_speed);
//	}
//	while (auto_pulse < 13767); // 13553 // 13214
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 4487); // 2759 // 6214
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	do{
//		turn_Right(wheel_speed);
//	}
//	while (auto_pulse < 20052); // 22278 //17826
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	//////////
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 35779); // 36076 // 35482
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 65535;
//	pre_p = 0;
//	auto_pulse = 65535;
//	delay_01ms(10000);
//	
//	
//	do{
//		turn_Left(wheel_speed);
//	}
//	while (auto_pulse > (65535 - 16457)); //17201 15713
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 6123); // 7123 6123
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 65535;
//	pre_p = 0;
//	auto_pulse = 65535;
//	delay_01ms(10000);
//	
//	
//	do{
//		turn_Left(wheel_speed);
//	}
//	while (auto_pulse > (65535 - 23704)); //24456 22951
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
//	
//	/////////
//	do{
//		move_Forward(wheel_speed);
//		run_Brush(brush_speed);
//	}
//	while (auto_pulse < 36152); // 35970 36334
//	stop_Brush();
//	stop_Robot();	
//	TIM1->CNT = 0;
//	pre_p = 0;
//	auto_pulse = 0;
//	delay_01ms(10000);
	
	
//		for (int i=0; i<1; i++){
//			move_Forward(wheel_speed);
//			delay_01ms(50000);
//			delay_01ms(25000);
//			stop_Robot();
//			turn_Left(wheel_speed);
//			delay_01ms(50000);
//			delay_01ms(50000);
//			stop_Robot();
//			move_Forward(wheel_speed);
//			delay_01ms(60000);
//			stop_Robot();
//			turn_Left(wheel_speed);
//			delay_01ms(50000);
//			delay_01ms(50000);
//			stop_Robot();
//			move_Forward(wheel_speed);
//			delay_01ms(50000);
//			delay_01ms(25000);
//			stop_Robot();
//		}
}

/*
	ppr: PULSE_PER_REVOLUTION
	
	C = 44cm -> 1807*4 = 7228 xung
	
	p = (ppr * ENCODER_mode) * d / C:
	- d: distance want to go
	- C: perimeter of wheel ~ 44cm = 0.44m
	- p: pulse to go d (m)
	
	3.155m -> 51828 pulse
	2.11m  -> 34662 pulse
	
	1 PV: w x h : 17249 x 34662
	3 PV: w x h : 34662 x 51828
	
	
	d = p / (ppr * ENCODER_mode) * C
	
	The distance when turning angle ALPHA:
			d(alpha) = (alpha)/360.0 * pi * sqrt(a^2 + b^2)
			- a: width
			- b: length
	
	--> p(alpha) = (ppr * ENCODER_mode)/0.44 * d(alpha)
			p(alpha) = (ppr * ENCODER_mode)/0.44 * alpha/360 * pi * sqrt(a^2 + b^2)
	
	turn 90: p = 29947
*/

/*
	Format the data string sent to the ESP32:
	- 4 distances of 4 ultrasonic sensors: dis1, dis2, dis3, dis4: abc.xy
	- percent of battery: percent (0-100): abc
	- pulse: 16bit (65535)
	- speed: m/s: ab.xy
	 
	D _ percent _ pulse  _  speed  _ distance move _  dis1   _ dis2   _ dis3   _ dis4   _ flag1 _ flag2 _ flag3 _ flag4 _ \r\n

	D _   abc   _ abcxyz _  a.xyz  _   bc.xy       _  bc.xy  _ bc.xy  _ bc.xy  _ bc.xy  _   x   _   x   _   x   _   x   _ \r\n
	0 1   234   5        12       18               24       30       36       42       48       50      52      54      56
	BUFF_SIZE_TX = 59 (bytes)
	
*/
