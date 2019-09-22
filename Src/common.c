/*
 * \file common.c
 * \brief Common functions definitions
 *
 * \author	Michal Labowski
 */

#include <common.h>

/*
 * \fn console_show
 * \brief Puts data to the serial console
 * \param[in] huart	A pointer to the UART handle (HAL)
 * \param[in] data A pointer to data to show
 */
int console_show(UART_HandleTypeDef *huart, char* data) {
	char uart_tx_data[300];
	int uart_tx_data_size = sprintf(uart_tx_data, "%s\n\r", data);
	HAL_UART_Transmit(huart, uart_tx_data, uart_tx_data_size, 10);
}

