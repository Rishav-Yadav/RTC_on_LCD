/*
 * ds1307.c
 *
 *  Created on: 12-May-2026
 *      Author: LENOVO
 */


#include "ds1307.h"
#include <string.h>
#include <stdint.h>
#include "ds1307.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

static void ds1307_write(uint8_t value,uint8_t reg_addr);
static HAL_StatusTypeDef ds1307_read(uint8_t reg_addr, uint8_t *data);
static uint8_t binary_to_bcd(uint8_t value);
static uint8_t bcd_to_binary(uint8_t value);

uint8_t ds1307_init(void){
	if(HAL_I2C_IsDeviceReady(&hi2c1, DS1307_I2C_ADDRESS<<1, 3, 100) != HAL_OK){
		return 1;
	}

	//make clock halt = 0
	ds1307_write(0x00,DS1307_ADD_SEC);

	//read back clock halt
	uint8_t clock_state = 0;
	if(ds1307_read(DS1307_ADD_SEC, &clock_state) != HAL_OK){
		return 1;
	}
	return ((clock_state>>7)&0x1);



}
void ds1307_set_current_time(RTC_time_t* rtc_time){
	uint8_t seconds,hrs;
	seconds = binary_to_bcd(rtc_time->seconds);
	seconds &= ~(1<<7);
	ds1307_write(seconds, DS1307_ADD_SEC);
	ds1307_write(binary_to_bcd(rtc_time->minutes), DS1307_ADD_MIN);
	hrs=binary_to_bcd(rtc_time->hours);
	if(rtc_time->time_format == TIME_FORMAT_24HRS){
		hrs &=~(1<<6);
	}else {
		hrs|=(1<<6);
		hrs = rtc_time->time_format==TIME_FORMAT_12HRS_AM?hrs & (~(1<<5)) : hrs |(1<<5);
	}
	ds1307_write(hrs, DS1307_ADD_HRS);
}
void ds1307_get_current_time(RTC_time_t* rtc_time){
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hrs = 0;
	if(ds1307_read(DS1307_ADD_SEC, &seconds) != HAL_OK){
		return;
	}
	if(ds1307_read(DS1307_ADD_MIN, &minutes) != HAL_OK){
		return;
	}
	if(ds1307_read(DS1307_ADD_HRS, &hrs) != HAL_OK){
		return;
	}
	seconds &= ~ (1<<7);
	rtc_time->seconds = bcd_to_binary(seconds);
	rtc_time->minutes = bcd_to_binary(minutes);
	if((hrs & (1<<6))){
		rtc_time->time_format = hrs & (1<<5)?TIME_FORMAT_12HRS_PM:TIME_FORMAT_12HRS_AM;
	}
	else{
		rtc_time->time_format =TIME_FORMAT_24HRS;
	}
	hrs &= ~(0x3<<5);
	rtc_time->hours = bcd_to_binary(hrs);
}
void ds1307_set_current_date(RTC_date_t* rtc_date){
	ds1307_write(binary_to_bcd(rtc_date->day), DS1307_ADD_DAY);
	ds1307_write(binary_to_bcd(rtc_date->date), DS1307_ADD_DATE);
	ds1307_write(binary_to_bcd(rtc_date->month), DS1307_ADD_MONTH);
	ds1307_write(binary_to_bcd(rtc_date->year), DS1307_ADD_YEAR);
}
void ds1307_get_current_date(RTC_date_t* rtc_date){
	uint8_t day = 0, date = 0, month = 0, year = 0;
	if(ds1307_read(DS1307_ADD_DAY, &day) != HAL_OK){
		return;
	}
	if(ds1307_read(DS1307_ADD_DATE, &date) != HAL_OK){
		return;
	}
	if(ds1307_read(DS1307_ADD_MONTH, &month) != HAL_OK){
		return;
	}
	if(ds1307_read(DS1307_ADD_YEAR, &year) != HAL_OK){
		return;
	}
	rtc_date->day = bcd_to_binary(day);
	rtc_date->date = bcd_to_binary(date);
	rtc_date->month = bcd_to_binary(month);
	rtc_date->year = bcd_to_binary(year);
}

//void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c){
//	if(hi2c->Instance == I2C1){
//	__HAL_RCC_I2C1_CLK_ENABLE();
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	GPIO_InitTypeDef i2c_sda, i2c_scl;
//	memset(&i2c_sda,0,sizeof(i2c_sda));
//	memset(&i2c_scl,0,sizeof(i2c_scl));
//
//	i2c_sda.Alternate = GPIO_AF6_I2C1;
//	i2c_sda.Mode = GPIO_MODE_AF_OD;
//	i2c_sda.Pin = GPIO_PIN_9;
//	i2c_sda.Pull = GPIO_PULLUP;
//	i2c_sda.Speed = GPIO_SPEED_FREQ_MEDIUM;
//	HAL_GPIO_Init(GPIOB, &i2c_sda);
//
//	i2c_scl.Alternate = GPIO_AF6_I2C1;
//	i2c_scl.Mode = GPIO_MODE_AF_OD;
//	i2c_scl.Pin = GPIO_PIN_8;
//	i2c_scl.Pull = GPIO_PULLUP;
//	i2c_scl.Speed = GPIO_SPEED_FREQ_MEDIUM;
//	HAL_GPIO_Init(GPIOB, &i2c_scl );
//	}
//}

static void ds1307_write(uint8_t value,uint8_t reg_addr){
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;
	(void)HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDRESS<<1,tx, sizeof(tx), 100);
}

static uint8_t ds1307_read(uint8_t reg_addr){
	uint8_t data = 0;
	if(HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDRESS<<1,&reg_addr, 1, 100) != HAL_OK){
		return 0;
	}
	if(HAL_I2C_Master_Receive(&hi2c1, DS1307_I2C_ADDRESS<<1, &data, 1, 100) != HAL_OK){
		return 0;
	}
	return data;
}

static uint8_t binary_to_bcd(uint8_t value){
	uint8_t m, n;
	uint8_t bcd;
	bcd = value;
	if(value>=10){
	m=value/10;
	n=value%10;
	bcd = (m<<4)|n;
	}
	return bcd;
}

static uint8_t bcd_to_binary(uint8_t value){
	uint8_t m, n;
	uint8_t binary;
	m = (uint8_t) ((value>>4)*10) ;
	n = value & (uint8_t)0x0F;
	binary = m+n;
	return binary;
}



