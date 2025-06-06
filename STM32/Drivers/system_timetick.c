
/* Includes ------------------------------------------------------------------*/
#include "system_timetick.h"

uint32_t	tick_count;
uint32_t	tick_flag;
uint32_t  tick_sp_count;
uint32_t   tick_sp_flag;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}
	  
void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  tick_flag = 1;
  tick_count++;
	tick_sp_count++;
//	if (tick_sp_count >= 100){
//		tick_sp_count = 0;
//		tick_sp_flag = 1;
//	}
}

