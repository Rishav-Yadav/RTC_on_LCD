/*
 * ds1307.h
 *
 *  Created on: 12-May-2026
 *      Author: LENOVO
 */

#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#include<stdint.h>

#define DS1307_ADD_SEC 0x00
#define DS1307_ADD_MIN 0x01
#define DS1307_ADD_HRS 0x02
#define DS1307_ADD_DAY 0x03
#define DS1307_ADD_DATE 0x04
#define DS1307_ADD_MONTH 0x05
#define DS1307_ADD_YEAR 0x06


#define TIME_FORMAT_12HRS_AM  0
#define TIME_FORMAT_12HRS_PM  1
#define TIME_FORMAT_24HRS     2


#define DS1307_I2C_ADDRESS    0x68



#define DS1307_I2C              0x40005400U
#define DS1307_I2C_GPIO_PORT    GPIOB
#define DS1307_I2C_SDA_PIN      GPIO_PIN_9
#define DS1307_I2C_SCL_PIN      GPIO_PIN_8
#define DS1307_I2C_SPEED          0
#define DS1307_I2C_PUPD           1

#define SUNDAY   1
#define MONDAY   2
#define TUESDAY  3
#define WEDNESDAY 4
#define THURSDAY  5
#define FRIDAY    6
#define SATURDAY  7



typedef struct {
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;

}RTC_date_t;

typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;

typedef struct{
	volatile uint32_t MODER;  //GPIO port mode register
	volatile uint32_t OTYPER;  //GPIO port output type register
	volatile uint32_t OSPEEDR; //GPIO port output speed register
	volatile uint32_t PUPDR;   //GPIO port pull-up/pull-down register
	volatile uint32_t IDR;     //GPIO port input data register
	volatile uint32_t ODR;     //GPIO port Output data register
	volatile uint32_t BSRR;    //GPIO port bit set/reset register
	volatile uint32_t LCKR;    //GPIO port configuration lock register
	volatile uint32_t AFR[2];    //GPIO alternate function low and High register
	volatile uint32_t BRR;     //GPIO port bit reset register

}GPIO_RegDef_t;


uint8_t ds1307_init(void);
void ds1307_set_current_time(RTC_time_t*);
void ds1307_get_current_time(RTC_time_t*);
void ds1307_set_current_date(RTC_date_t*);
void ds1307_get_current_date(RTC_date_t*);





#endif /* INC_DS1307_H_ */
