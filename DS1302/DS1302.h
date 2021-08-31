#ifndef DS1302
#define DS1302
#include "main.h"

//CE pin PA9
#define DS1302_CE_0 HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET)
#define DS1302_CE_1 HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET)
//IO pin PC7
#define DS1302_IO_0 HAL_GPIO_WritePin(IO_GPIO_Port, IO_Pin, GPIO_PIN_RESET)
#define DS1302_IO_1 HAL_GPIO_WritePin(IO_GPIO_Port, IO_Pin, GPIO_PIN_SET)
//SCLK pin PB6
#define DS1302_SCLK_0 HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET)
#define DS1302_SCLK_1 HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET)

//register address read-mode LSB=1 0x80->0x81 write-mode LSB=0 0x80
#define DS1302_SECOND  0x80
#define DS1302_MINUTE  0x82
#define DS1302_HOUR    0x84 
#define DS1302_DAY    0x86
#define DS1302_MONTH  0x88
#define DS1302_WEEK    0x8a
#define DS1302_YEAR    0x8c 

typedef struct{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  char week[10];
} Time;

typedef enum{
  SUNDAY = 1, //DS1302 week start from 1 Sunday
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
} Week;

uint8_t DS1302_Read(uint8_t addr);
uint8_t DS1302_OutputByte(void);
void DS1302_InputByte(unsigned char dat);
void DS1302_Init(void);
void DS1302_SetTime(uint8_t address, uint8_t value);
void DS1302_SetProtect(uint8_t flag);
void DS1302_Write(uint8_t addr, uint8_t dat);
void DS1302_GetTime(Time* now_time);
void DS1302_Burst_GetTime(Time* now_time);

#endif
