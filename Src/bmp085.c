/*
 * bmp085.c
 *
 *  Created on: Sep 22, 2019
 *      Author: michal
 */
#include "bmp085.h"
#include <math.h>

int16_t ac1 = 0;
int16_t ac2 = 0;
int16_t ac3 = 0;
uint16_t ac4 = 0;
uint16_t ac5 = 0;
uint16_t ac6 = 0;

int16_t b1 = 0;
int16_t b2 = 0;
int32_t b5 = 0;

int16_t mb = 0;
int16_t mc = 0;
int16_t md = 0;


void bmp085_calibration(I2C_HandleTypeDef *hi2c) {

	uint8_t id = 0;
	uint8_t ac1_msb, ac1_lsb;
	id = bmp085_read_1B(hi2c, BMP085_ADDRESS, BMP085_ID, 10);

	ac1 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC1, 10);
	ac2 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC2, 10);
	ac3 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC3, 10);
	ac4 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC4, 10);
	ac5 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC5, 10);
	ac6 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_AC6, 10);

	b1 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_B1, 10);
	b2 = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_B2, 10);

	mb = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_MB, 10);
	mc = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_MC, 10);
	md = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_CAL_MD, 10);
}

int32_t bmp085_get_ut(I2C_HandleTypeDef *hi2c) {
	uint8_t command = BMP085_READTEMPCMD;
	HAL_I2C_Mem_Write(hi2c, BMP085_ADDRESS, BMP085_CONTROL, 1, &command, 1, 10);
	HAL_Delay(10);
	uint16_t data = bmp085_read_2B(hi2c, BMP085_ADDRESS, BMP085_TEMPDATA, 10);
	return (int32_t)data;
}

int32_t bmp085_get_up(I2C_HandleTypeDef *hi2c, int8_t oversampling) {

	uint32_t delay_ms = 0;
	switch (oversampling) {
	case 0:
		delay_ms = 8;
		break;
	case 1:
		delay_ms = 10;
		break;
	case 2:
		delay_ms = 16;
		break;
	case 3:
		delay_ms = 30;
		break;
	default:
		oversampling = 0;
		break;
	}

	uint8_t command = BMP085_READPRESSURECMD + (oversampling << 6);
	HAL_I2C_Mem_Write(hi2c, BMP085_ADDRESS, BMP085_CONTROL, 1, &command, 1, 10);
	HAL_Delay(delay_ms);

	uint8_t msb = 0;
	uint8_t lsb = 0;
	uint8_t xlsb = 0;

	msb = bmp085_read_1B(hi2c, BMP085_ADDRESS, BMP085_PRESSUREDATA, 10);
	lsb = bmp085_read_1B(hi2c, BMP085_ADDRESS, BMP085_PRESSUREDATA+1, 10);
	xlsb = bmp085_read_1B(hi2c, BMP085_ADDRESS, BMP085_PRESSUREDATA+2, 10);

	int32_t res = ( (((int32_t)msb) << 16) + (((int32_t)lsb)<< 8) + ((int32_t)xlsb) ) >> (8 - oversampling);
	return res;
}

int32_t bmp085_get_pressure(I2C_HandleTypeDef *hi2c, int8_t oversampling)
{
	int32_t up = bmp085_get_up(hi2c, oversampling);
	int32_t b6 = b5 - 4000;
	int32_t x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
	int32_t x2 = (ac2 * b6) >> 11;
	int32_t x3 = x1 + x2;
	int32_t b3 = ((((int32_t)ac1*4 + x3) << oversampling) + 2) / 4;

	 x1 = ((int32_t)ac3 * b6) >> 13;
	 x2 = ((int32_t)b1 * ((b6 * b6) >> 12)) >> 16;
	 x3 = ((x1 + x2) + 2) >> 2;
	 uint32_t b4 = ((uint32_t)ac4 * (uint32_t)(x3 + 32768)) >> 15;
	 uint32_t b7 = ((uint32_t)up - b3) * (uint32_t)( 50000UL >> oversampling );

	 int32_t p = 0;
	 if (b7 < 0x80000000) {
	   p = (b7 * 2) / b4;
	 } else {
	   p = (b7 / b4) * 2;
	 }
	 x1 = (p >> 8) * (p >> 8);
	 x1 = (x1 * 3038) >> 16;
	 x2 = (-7357 * p) >> 16;
	 p = p + ((x1 + x2 + (int32_t)3791)>>4);
	 return p;

}

float bmp085_get_temperature(I2C_HandleTypeDef *hi2c) {
	int32_t ut = bmp085_get_ut(hi2c);
	int32_t x1 = ((ut - (int32_t)ac6) * (int32_t)ac5) >> 15;
	int32_t x2 = ((int32_t)mc << 11) / (x1 + (int32_t)md);
	b5 = x1 + x2;
	return ((float)((b5 + (int32_t)8) >> 4)) / 10.0f;
}



uint8_t bmp085_read_1B(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint32_t Timeout)
{
	uint16_t MemAddSize = 1;
	uint16_t Size = 1;
	uint8_t data;
	HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, &data, Size, Timeout);
	return data;
}

uint16_t bmp085_read_2B(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint32_t Timeout)
{
	uint16_t MemAddSize = 1;
	uint8_t data_msb;
	uint8_t data_lsb;

	HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, &data_msb, 1, Timeout);
	HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress+1, MemAddSize, &data_lsb, 1, Timeout);

	int16_t res = ((int16_t)data_msb) << 8;
	res += (int16_t)data_lsb;

	return res;
}





