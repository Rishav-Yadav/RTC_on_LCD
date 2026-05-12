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
static uint8_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t value);
static uint8_t bcd_to_binary(uint8_t value);

uint8_t ds1307_init(void){

	//make clock halt = 0
	ds1307_write(0x00,DS1307_ADD_SEC);

	//read back clock halt
	uint8_t clock_state = ds1307_read(DS1307_ADD_SEC);
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
	uint8_t seconds = ds1307_read(DS1307_ADD_SEC);
	seconds &= ~ (1<<7);
	rtc_time->seconds = bcd_to_binary(seconds);
	rtc_time->minutes = bcd_to_binary(ds1307_read(DS1307_ADD_MIN));
	uint8_t hrs = ds1307_read(DS1307_ADD_HRS);
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
	rtc_date->day = bcd_to_binary(ds1307_read(DS1307_ADD_DAY));
	rtc_date->date = bcd_to_binary(ds1307_read(DS1307_ADD_DATE));
	rtc_date->month = bcd_to_binary(ds1307_read(DS1307_ADD_MONTH));
	rtc_date->year = bcd_to_binary(ds1307_read(DS1307_ADD_YEAR));
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
	HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDRESS<<1,tx, sizeof(tx), HAL_MAX_DELAY);
}

static uint8_t ds1307_read(uint8_t reg_addr){
	uint8_t data;
	HAL_I2C_Master_Transmit(&hi2c1, DS1307_I2C_ADDRESS<<1,&reg_addr, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, DS1307_I2C_ADDRESS<<1, &data, 1, HAL_MAX_DELAY);
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



