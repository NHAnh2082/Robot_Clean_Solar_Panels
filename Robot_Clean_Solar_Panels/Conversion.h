#include "stm32f4xx.h"

void StrToInt(int length, uint8_t u[], int16_t *y);
void Str7ToDouble(uint8_t u[], double *y);
void FloatToStr6(float value, uint8_t *y, int type, int precision);
void IntToStr(int32_t u, uint8_t *y, int8_t length);
