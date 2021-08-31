#include "DS1302.h"
#include "main.h"
#include <stdio.h>

extern void Delay_Us(uint16_t wait);

void DS1302_InputByte(unsigned char dat){ 
  unsigned char i;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(IO_GPIO_Port, &GPIO_InitStruct);
  
  for(i=0; i<8; i++){
    if((dat & 0x01) == 1)
      DS1302_IO_1;
    else
      DS1302_IO_0;
    
    DS1302_SCLK_0;
    //Delay_Us(2);
    DS1302_SCLK_1;
    //Delay_Us(2);

    dat >>= 1; 
  }
}
uint8_t DS1302_OutputByte(void){ 
  uint8_t i;
  uint8_t dat;
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(IO_GPIO_Port, &GPIO_InitStruct);
  
  for(i=0; i<8; i++){
    DS1302_SCLK_1;
    //Delay_Us(2);
    DS1302_SCLK_0;
    //Delay_Us(2);
    dat >>= 1;
    
    if(HAL_GPIO_ReadPin(IO_GPIO_Port, IO_Pin))
      dat |= 0x80;
    else
      dat &= 0x7F;     
  }
  
  return dat; 
}
void DS1302_Write(uint8_t addr, uint8_t dat){
  DS1302_CE_0;
  //Delay_Us(2);
  DS1302_SCLK_0;
  //Delay_Us(2);
  DS1302_CE_1;
  //Delay_Us(2);
  
  DS1302_InputByte(addr);
  DS1302_InputByte(dat);
  
  DS1302_SCLK_1;
  //Delay_Us(2);
  DS1302_CE_0;
  //Delay_Us(2);
}
uint8_t DS1302_Read(uint8_t addr){
  uint8_t dat;
  
  DS1302_CE_0;
  //Delay_Us(2);
  DS1302_SCLK_0;
  //Delay_Us(2);
  DS1302_CE_1;
  //Delay_Us(2);
  
  DS1302_InputByte(addr|0x01);
  dat = DS1302_OutputByte();
  
  DS1302_SCLK_1;
  //Delay_Us(2);
  DS1302_CE_0;
  //Delay_Us(2);
  
  return dat;
}
void DS1302_SetProtect(uint8_t flag){
  if(flag)
    DS1302_Write(0x8E,0x10);  //protect
  else
    DS1302_Write(0x8E,0x00);  //no protect
}
void DS1302_SetTime(uint8_t address, uint8_t value){
  DS1302_SetProtect(0);
  DS1302_Write(address, (value/10)<<4 | (value%10)); 
}
void DS1302_Init(void){
  unsigned char second;
  
  second = DS1302_Read(DS1302_SECOND);
  if(second&0x80)      
    DS1302_SetTime(DS1302_SECOND,second & 0x7f);
}
void DS1302_GetTime(Time* now_time){
  uint8_t ReadValue;
  ReadValue = DS1302_Read(DS1302_SECOND);
  ReadValue = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  now_time->second = ReadValue;
  ReadValue = DS1302_Read(DS1302_MINUTE);
  now_time->minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_Read(DS1302_HOUR);
  now_time->hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_Read(DS1302_DAY);
  now_time->day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);  
  ReadValue = DS1302_Read(DS1302_MONTH);
  now_time->month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_Read(DS1302_YEAR);
  now_time->year = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  
  ReadValue = DS1302_Read(DS1302_WEEK);
  switch(((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F)){
    case SUNDAY:
      sprintf(now_time->week, "Sun.");
      break;
    case MONDAY:
      sprintf(now_time->week, "Mon.");
      break;
    case TUESDAY:
      sprintf(now_time->week, "Tues.");
      break;
    case WEDNESDAY:
      sprintf(now_time->week, "Wed.");
      break;
    case THURSDAY:
      sprintf(now_time->week, "Thur.");
      break;
    case FRIDAY:
      sprintf(now_time->week, "Fri.");
      break;
    case SATURDAY:
      sprintf(now_time->week, "Sat.");
      break;
  }
}
void DS1302_Burst_GetTime(Time* now_time){
  uint8_t ReadValue;
  
  DS1302_CE_0;
  //Delay_Us(2);
  DS1302_SCLK_0;
  //Delay_Us(2);
  DS1302_CE_1;
  //Delay_Us(2);
  
  DS1302_InputByte(0xBF);
  
  ReadValue = DS1302_OutputByte();
  ReadValue = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  now_time->second = ReadValue;
  ReadValue = DS1302_OutputByte();
  now_time->minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_OutputByte();
  now_time->hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_OutputByte();
  now_time->day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);  
  ReadValue = DS1302_OutputByte();
  now_time->month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  ReadValue = DS1302_OutputByte();
  switch(((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F)){
    case SUNDAY:
      sprintf(now_time->week, "Sun.");
      break;
    case MONDAY:
      sprintf(now_time->week, "Mon.");
      break;
    case TUESDAY:
      sprintf(now_time->week, "Tues.");
      break;
    case WEDNESDAY:
      sprintf(now_time->week, "Wed.");
      break;
    case THURSDAY:
      sprintf(now_time->week, "Thur.");
      break;
    case FRIDAY:
      sprintf(now_time->week, "Fri.");
      break;
    case SATURDAY:
      sprintf(now_time->week, "Sat.");
      break;
  }
  ReadValue = DS1302_OutputByte();
  now_time->year = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
  DS1302_OutputByte();
  
  DS1302_SCLK_1;
  //Delay_Us(2);
  DS1302_CE_0;
  //Delay_Us(2);
}
