#ifndef DHT11_H
#define DHT11_H

#include <stdbool.h>
#include "main.h"

/* set timer7 to count per 1us */
extern TIM_HandleTypeDef htim7;

typedef struct{
  uint8_t tempature;
  uint8_t humidity;
} dht11_data;

void dht11_init(void);
bool dht11_read(dht11_data *data);

#endif /* DHT11_H */
