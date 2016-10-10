/* reaper7 */
#include "../include/driver/i2c_am2320.h"

#include <errno.h>

#include "driver/i2c.h"

uint16_t crc16(uint8_t *ptr, unsigned int len){

    uint16_t crc =0xFFFF;
    uint8_t	i;

    while(len--) {
    	crc ^= *ptr++;

		for(i=0;i<8;i++) {
			if(crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
    }
    return crc;
}

//Read sensor data
int am2320_read(uint16_t *humidity, uint16_t *temp){
	//Wake sensor, first time it can return NAK or do not respond
    i2c_start();
    i2c_writeByte(AM2320_W);
    i2c_stop();
    os_delay_us(800);

    //Send read command
    i2c_start();
    i2c_writeByte(AM2320_W);//Write device address

    if (!i2c_check_ack()) {
	    i2c_stop();
	    return(ERR_READ);
	}

    i2c_writeByte(0x03);//This is constant
    if (!i2c_check_ack()) {
	    i2c_stop();
	    return(ERR_READ);
	}
    i2c_writeByte(0x00);//first register address
    if (!i2c_check_ack()) {
	    i2c_stop();
	    return(ERR_READ);
	}
    i2c_writeByte(0x04);//Data length
    if (!i2c_check_ack()) {
	    i2c_stop();
	    return(ERR_READ);
	}
    i2c_stop();//end of send command

    //Read data
    os_delay_us(1600);//wait for data to measure

    i2c_start();
    i2c_writeByte(AM2320_R);//Send address for reading
    if (!i2c_check_ack()) {
	    i2c_stop();
	    return(ERR_READ);
	}

    os_delay_us(100);//wait a bit

    struct {
    	uint16_t crc;
    	uint16_t temp;
    	uint16_t humidity;
    	uint8_t len;
    	uint8_t startCode;
    } dta;

    //Input Buffer
    uint8_t *b = (uint8_t *)(&dta);

    uint8_t i;//read answer - 8 bytes
	for(i=0; i<8; i++){
		b[7 - i] = i2c_readByte();
    	i2c_send_ack(i == 7 ? 0 : 1);
    }

	*temp = dta.temp;
	*humidity = dta.humidity;

	//swap lo-hi bytes in crc
	uint8_t t = b[0];
	b[0] = b[1];
	b[1] = t;

	uint16_t crc = dta.crc;

	uint8_t d[6];
	//reorganize to calc CRC (reverse and shift to 0 intex)
    for (i=0; i<6; i++){
    	d[i] = b[7-i];
    }

	if(crc != crc16(d, 6)){
	    return(ERR_CRC);
	}
    return RES_OK;
}

void am2320_init(){
    i2c_init();
}

