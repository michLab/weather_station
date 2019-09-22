#ifndef __COMMON_H__
#define __COMMON_H__

#include "stm32l4xx_hal.h"

int console_show(UART_HandleTypeDef *huart, char* data);

#endif
