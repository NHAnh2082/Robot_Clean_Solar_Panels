/******************************************************************************
 * @file    DataConversion.c
 * @brief   Định nghĩa các hàm chuyển đổi số và chuỗi ký tự
 * @details Cung cấp hàm chuyển đổi giữa số nguyên, số thực và chuỗi ký tự ASCII,
 *          phục vụ cho các module truyền nhận dữ liệu hoặc hiển thị.
 * @version 1.0
 * @date    2024-06-02
 * @author  Anh Nguyen
 ******************************************************************************/

#include "stm32f4xx.h"
#include "DelayUtils.h"
#include "DataConversion.h"

/**
 * @brief   Chuyển số nguyên 32 bit sang chuỗi ký tự ASCII dạng số
 * @details Chuyển đổi số nguyên u thành chuỗi ký tự số, không có dấu.
 *          Kết quả lưu trong mảng y có độ dài length ký tự.
 * @param   u       Giá trị số nguyên cần chuyển đổi
 * @param   y       Mảng buffer chứa chuỗi kết quả (độ dài >= length)
 * @param   length  Số ký tự muốn chuyển đổi (độ dài chuỗi kết quả)
 * @return  void
 */
void IntToStr(int32_t u, uint8_t *y, int8_t length)
{
	int32_t b = 0;
	b = u;

	for (int16_t i=(length-1); i>=0; i--)
	{
		y[i] = (b % 10) + 0x30;
		b /= 10;
	}
}

/**
 * @brief   Chuyển số thực (float) sang chuỗi ký tự ASCII có dấu thập phân
 * @details Chuyển giá trị float thành chuỗi ký tự dạng "xx.xx...", theo kiểu và độ chính xác cho trước.
 *          Hàm có hỗ trợ một số kiểu (type) để tùy chỉnh số chữ số phần nguyên và phần thập phân.
 * @param   value       Giá trị số thực cần chuyển đổi
 * @param   y           Mảng buffer lưu kết quả chuỗi (phải đủ lớn)
 * @param   type        Kiểu dữ liệu (ví dụ 1: vận tốc, 2: khoảng cách)
 * @param   precision   Độ chính xác phần thập phân (số chữ số sau dấu '.')
 * @return  void
 */
void FloatToStr6(float value, uint8_t *y, int type, int precision)
{
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
	if (int_Part == 0)
	{
		if (type == 1)
		{
			IntToStr(int_Part, y, 1);
			y[1] = '.';

			for (i=0; i<precision; i++)
			{
				frac_Part *= 10;
				y[i+2] = (int8_t)frac_Part + 0x30;
				frac_Part -= (int8_t)frac_Part;
			}
		}
		else if (type == 2)
		{
			IntToStr(int_Part, y, 2);
			y[2] = '.';

			for (i=0; i<precision; i++)
			{
				frac_Part *= 10;
				y[i+3] = (int8_t)frac_Part + 0x30;
				frac_Part -= (int8_t)frac_Part;
			}
		}
	}
	else if (int_Part > 0)
	{
		IntToStr(int_Part, y, 2);
		y[2] = '.';

		for (i=0; i<precision; i++)
		{
			frac_Part *= 10;
			y[i+3] = (int8_t)frac_Part + 0x30;
			frac_Part -= (int8_t)frac_Part;
		}
	}

	// Delay 100ms để đảm bảo dữ liệu được xử lý
	delay_01ms(100);
}

/**
 * @brief   Chuyển chuỗi 7 ký tự số sang giá trị double
 * @details Chuỗi đầu vào có định dạng "x.xxxxxx", bỏ qua ký tự thứ 1 và ký tự thứ 1 trong chuỗi
 * @param   u       Mảng byte chứa chuỗi số dạng ASCII (độ dài tối thiểu 7)
 * @param   y       Con trỏ tới biến double nhận kết quả chuyển đổi
 * @return  void
 */
void Str7ToDouble(uint8_t u[], double *y)
{
	double a;
	a = (u[0] - 0x30)*1 + (u[2] - 0x30)*0.1 + (u[3] - 0x30)*0.01
	  + (u[4] - 0x30)*0.001 + (u[5] - 0x30)*0.0001 + (u[6] - 0x30)*0.00001;
	*y = a;
}

/**
 * @brief   Chuyển chuỗi số nguyên dạng ký tự sang số nguyên int16_t
 * @details Chuỗi đầu vào gồm các ký tự ASCII '0'..'9', được chuyển thành số nguyên.
 * @param   length  Độ dài chuỗi đầu vào
 * @param   u       Mảng byte chứa chuỗi ký tự số
 * @param   y       Con trỏ trả về giá trị số nguyên kiểu int16_t
 * @return  void
 */
void StrToInt(int length, uint8_t u[], int16_t *y)
{
	int16_t a = 0;
	
	for (int i=0; i<length; i++) 
	{
		a = a*10 + (u[i] - 0x30);
	}

	*y = a;
}
