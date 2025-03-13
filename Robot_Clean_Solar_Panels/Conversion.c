#include "stm32f4xx.h"
#include "Delay.h"
#include "Conversion.h"

//------------------------------------------------------------
void IntToStr(int32_t u, uint8_t *y, int8_t length){
	int32_t b = 0;
	b = u;
	for (int16_t i=length-1; i>=0; i--){
		y[i] = (b % 10) + 0x30;
		b /= 10;
	}
}

//------------------------------------------------------------
void FloatToStr6(float value, uint8_t *y, int type, int precision){
	int16_t i;
	int32_t int_Part  = (int32_t)(value);
	float frac_Part = value - (float)(int_Part);
	
//	int count = (int)log10(int_Part) + 1;	// count the number of digits of the integer part, example: 123->count=3, 12->count=2, 1->count=1
//	if ((count == 1) || (int_Part == 0)){
//		IntToStr(int_Part,y,1);
//		y[1] = '.';
//		for (i=0; i<precision; i++){
//			frac_Part *= 10;
//			y[i+2] = (int8_t)frac_Part + 0x30;
//			frac_Part -= (int8_t)frac_Part;
//		}
//	}
//	else if (count == 2){
//		IntToStr(int_Part,y,2);
//		y[2] = '.';
//		for (i=0; i<precision; i++){
//			frac_Part *= 10;
//			y[i+3] = (int8_t)frac_Part + 0x30;
//			frac_Part -= (int8_t)frac_Part;
//		}
//	}
//	else if (count == 3){
//		IntToStr(int_Part,y,3);
//		y[3] = '.';
//		for (i=0; i<precision; i++){
//			frac_Part *= 10;
//			y[i+4] = (int8_t)frac_Part + 0x30;
//			frac_Part -= (int8_t)frac_Part;
//		}
//	}
	
	// type = 1 -> read velocity
	// type = 2 -> read space, value of 4 HCSR04
	if (int_Part == 0){
		if (type == 1){
			IntToStr(int_Part, y, 1);
			y[1] = '.';
			for (i=0; i<precision; i++){
				frac_Part *= 10;
				y[i+2] = (int8_t)frac_Part + 0x30;
				frac_Part -= (int8_t)frac_Part;
			}
		}
		else if (type == 2){
			IntToStr(int_Part, y, 2);
			y[2] = '.';
			for (i=0; i<precision; i++){
				frac_Part *= 10;
				y[i+3] = (int8_t)frac_Part + 0x30;
				frac_Part -= (int8_t)frac_Part;
			}
		}
	}
	else if (int_Part > 0){
		IntToStr(int_Part, y, 2);
		y[2] = '.';
		for (i=0; i<precision; i++){
			frac_Part *= 10;
			y[i+3] = (int8_t)frac_Part + 0x30;
			frac_Part -= (int8_t)frac_Part;
		}
	}
	delay_01ms(100);
}

//------------------------------------------------------------
void Str7ToDouble(uint8_t u[], double *y){
	double a;
	a = (u[0] - 0x30)*1 + (u[2] - 0x30)*0.1 + (u[3] - 0x30)*0.01
		+ (u[4] - 0x30)*0.001 + (u[5] - 0x30)*0.0001 + (u[6] - 0x30)*0.00001;
	*y = a;
}

//------------------------------------------------------------
void StrToInt(int length, uint8_t u[], int16_t *y){
	int16_t a = 0;
	for (int i=0; i<length; i++) a = a*10 + (u[i] - 0x30);
	*y = a;
}
