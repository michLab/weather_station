/*
 * common.c
 *
 *  Created on: Sep 22, 2019
 *      Author: michal
 */
#include <common.h>

int console_show(UART_HandleTypeDef *huart, char* data) {
	char uart_tx_data[300];
	int uart_tx_data_size = sprintf(uart_tx_data, "%s\n\r", data);
	HAL_UART_Transmit(huart, uart_tx_data, uart_tx_data_size, 10);
}

