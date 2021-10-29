#include "dht11.h"

void delay_us(uint16_t time)
{
  __HAL_TIM_SET_COUNTER(&htim7, 0);
  while(__HAL_TIM_GET_COUNTER(&htim7) < time);
}

void set_pin_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void set_pin_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void dht11_start(void)
{
  set_pin_output(DATA_GPIO_Port, DATA_Pin);  // set the pin as output
  HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_SET);
  delay_us(20);
  HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_RESET);   // pull the pin low
  delay_us(18000);   // wait for 18ms
  HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_SET);   // pull the pin high
  delay_us(30);   // wait for 20us
  set_pin_input(DATA_GPIO_Port, DATA_Pin);    // set as input
}

bool dht11_check_response(void)
{
  bool response = false;
  
  delay_us(40);
  if(HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin) == GPIO_PIN_RESET)
  {
    delay_us(80);
    if(HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin) == GPIO_PIN_SET)
      response = true;
    else
      response = false;
  }
  while(HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin));   // wait for the pin to go low

  return response;
}

uint8_t dht11_read_byte(void)
{
  uint8_t i,j;
  
  for(j=0;j<8;j++)
  {
    while(!(HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin)));   // wait for the pin to go high
    delay_us(40);   // wait for 40 us
    /* 
     * MSB first receive
     * if the pin is low, write 0 and pin is high, write 1
     */
    if(!(HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin)))
    {
      i &= ~(1 << (7 - j));
    }
    else
    {
      i |= (1 << (7 - j));
    }
    while((HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin)));  // wait for the pin to go low
  }
  
  return i;
}

void dht11_init(void)
{
  HAL_TIM_Base_Start(&htim7);
  HAL_Delay(1500); // wait for stable device 
}

bool dht11_read(dht11_data *data)
{
  uint8_t b0, b1, b2, b3;
  uint16_t checksum, tmp;

  dht11_start();
    
  if(dht11_check_response())
    return false;
  
  b0 = dht11_read_byte();
  b1 = dht11_read_byte();
  b2 = dht11_read_byte();
  b3 = dht11_read_byte();
  checksum = dht11_read_byte();
  tmp = b0 + b1 + b2 + b3;
  tmp = tmp & 0x00FF; // get lower 8 bits
  if(tmp == checksum)
  {
    /* DHT11 dose not support decimal */
    data->humidity = b0;
    data->tempature = b2;
  }
  else
  {
    return false;
  }
  
  return true;
}
