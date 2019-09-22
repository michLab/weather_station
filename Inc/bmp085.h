#ifndef __BMP_085_H__
#define __BMP_085_H__

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define BMP085_ID		0xD0
#define BMP085_ADDRESS	(0xEE)

#define BMP085_CAL_AC1           0xAA  // R   Calibration data (16 bits)
#define BMP085_CAL_AC2           0xAC  // R   Calibration data (16 bits)
#define BMP085_CAL_AC3           0xAE  // R   Calibration data (16 bits)
#define BMP085_CAL_AC4           0xB0  // R   Calibration data (16 bits)
#define BMP085_CAL_AC5           0xB2  // R   Calibration data (16 bits)
#define BMP085_CAL_AC6           0xB4  // R   Calibration data (16 bits)
#define BMP085_CAL_B1            0xB6  // R   Calibration data (16 bits)
#define BMP085_CAL_B2            0xB8  // R   Calibration data (16 bits)
#define BMP085_CAL_MB            0xBA  // R   Calibration data (16 bits)
#define BMP085_CAL_MC            0xBC  // R   Calibration data (16 bits)
#define BMP085_CAL_MD            0xBE  // R   Calibration data (16 bits)

#define BMP085_CONTROL           0xF4
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD       0x2E
#define BMP085_READPRESSURECMD   0x34


void bmp085_calibration(I2C_HandleTypeDef *hi2c);
int32_t bmp085_get_ut(I2C_HandleTypeDef *hi2c);
int32_t bmp085_get_up(I2C_HandleTypeDef *hi2c, int8_t oversampling);
float bmp085_get_temperature(I2C_HandleTypeDef *hi2c);
int32_t bmp085_get_pressure(I2C_HandleTypeDef *hi2c, int8_t oversampling);
uint8_t bmp085_read_1B(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint32_t Timeout);
uint16_t bmp085_read_2B(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint32_t Timeout);

#endif
