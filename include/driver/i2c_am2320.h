#ifndef __I2C_AM2320_H
#define	__I2C_AM2320_H

#define AM2320_W					0xb8 //write address [xxxxxxx0]
#define AM2320_R					0xb9 //read address [xxxxxxx1]
										 // where xxxxxxx = 0xc5

#define ERR_CRC						-2
#define ERR_READ					-1
#define RES_OK						0

#include "c_types.h"
#include "ets_sys.h"
#include "osapi.h"

static uint16_t crc16(uint8_t *ptr, unsigned int len);

//Reads data from sensor
int am2320_read(uint16_t *humidity, uint16_t *temp);

void am2320_init();

#endif
