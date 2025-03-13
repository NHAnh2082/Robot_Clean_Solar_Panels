/* Use ultrasonic sensors to prevent the robot from falling off the solar panels during movement */
/* Used Timer 12 to read pulse when Echo is high */

#include "stm32f4xx.h"
#include <stdbool.h>

#define NUM_SENSORS 	4
#define err_distance  1.5
#define max_distance  15

//extern GPIO_TypeDef* sensorPorts[NUM_SENSORS];
extern GPIO_TypeDef* EchoPort[NUM_SENSORS];
extern GPIO_TypeDef* TrigPort[NUM_SENSORS];
extern uint16_t echoPins[NUM_SENSORS];
extern uint16_t trigPins[NUM_SENSORS];
extern bool sensorStatus[NUM_SENSORS];
extern float distance[NUM_SENSORS];
extern float init[NUM_SENSORS];
extern float curr[NUM_SENSORS];
extern int flag[NUM_SENSORS];
extern uint32_t start_time;
extern uint32_t stop_time;
extern int16_t cnt_cb;
extern uint8_t data_dis[NUM_SENSORS][5];
extern uint8_t data_flag[NUM_SENSORS][1];

void init_GPIO_Sensors(uint8_t sensorIndex, uint16_t Trigger, uint16_t Echo);
void init_Sensor_TIM12(void);
void read_distance(uint8_t sensorIndex);
int detect_edge(float init_dis, float curr_dis);
void get_flag(void);
