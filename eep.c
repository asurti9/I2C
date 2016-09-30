#include "stm32l1xx_gpio.h"
#include <stdbool.h>
#include "stm32l1xx_rcc.h"
#include "eep.h"

// Define the pins
// SDA
// SCLK

#define GPIO_MODE_INPUT(pinNumber)        (0U << ((pinNumber) * 2))
#define GPIO_MODE_OUTPUT(pinNumber)       (1U << ((pinNumber) * 2))
#define GPIO_MODE_ALTERNATE(pinNumber)    (2U << ((pinNumber) * 2))
#define GPIO_MODE_ANALOG(pinNumber)       (3U << ((pinNumber) * 2))

#define SDA(x) x == 0 ? GPIO_ResetBits(GPIOB, GPIO_Pin_8): GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define SCLK(x) x == 0 ? GPIO_ResetBits(GPIOB, GPIO_Pin_9): GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define  SDA_DATA(x)  SCLK(0); \
                      SDA(x); \
                      SCLK(1); \
                      delay(); \
                      SCLK(0)

#define SDA_IN()   GPIOB->MODER &= ~(0x00000003 << (GPIO_PinSource8 * 2)); GPIOB->MODER |= GPIO_MODE_INPUT(GPIO_PinSource8)
#define SDA_OUT()  GPIOB->MODER &= ~(0x00000003 << (GPIO_PinSource8 * 2)); GPIOB->MODER |= GPIO_MODE_OUTPUT(GPIO_PinSource8)

#define DEVICE_SELECT_READ     0b10100001
#define DEVICE_SELECT_WRITE    0b10100000
/*void SDA_IN()
{
	//pin init
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_IN ;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_Speed = GPIO_Speed_400KHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);
}*/

/*void SDA_OUT()
{
	//pin init
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_OUT ;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_Speed = GPIO_Speed_400KHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &gpio);
}*/
void delay()
{
	volatile uint8_t counter = 0x00;
	while( counter < 0x0F) counter++;
}

void i2c_start()
{
	SDA(1);
	delay();
	SCLK(1);
	delay();
	SDA(0);
	delay();
	SCLK(0);
}

void i2c_stop()
{
	SDA(0);
	delay();
	SCLK(1);
	delay();
	SDA(1);
	delay();
}

bool i2c_data_out(uint8_t data)
{
	// Write a loop to put on line.
	uint8_t i;
	bool ack;
	for(i=0; i<8; i++)
	{
		if(data & 0x80)
		{
			SDA_DATA(1);
		}
		else
		{
		   SDA_DATA(0);
		}
		data = data << 1;
	}
	SDA(1);
	SCLK(0);
	// Make GPIO pin Input
	SDA_IN();
	SCLK(1);
	delay();
	// Read ACK
	ack = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8) ? false : true;
	SCLK(0);
	SDA_OUT();
	// Return.
	return ack;
}

uint8_t i2c_data_in(bool ack)
{
	volatile uint8_t data = 0;
	uint8_t i = 0;
	SDA(1);
	//make the data line input
	SDA_IN();
	//loop to get the data
	//SCLK(0);
	delay();
	SCLK(1);
	delay();
	data |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
	SCLK(0);
	for(i=0; i<7; i++)
	{
		data = data << 1;
		SCLK(1);
		delay();
		data |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
		SCLK(0);
	}
	//data line output high
	SDA_OUT();
	SCLK(1);
	//send the ack
	ack ? SDA(0) : SDA(1);
	delay();
	SCLK(0);
	//return the data
	return data;
}

void eeprom_write(uint16_t address, uint8_t data)
{
	i2c_start();
	while(!i2c_data_out(DEVICE_SELECT_WRITE))
	{
		i2c_start();
	}
	while(!i2c_data_out(address & 0x00FF));
	while(!i2c_data_out(data));
	i2c_stop();
}

uint8_t eeprom_read(uint16_t address)
{
	uint8_t data;
	i2c_start();
	while(!i2c_data_out(DEVICE_SELECT_WRITE)){
		i2c_start();
	};

	while(!i2c_data_out(address & 0x00FF));

	i2c_start();
	while(!i2c_data_out(DEVICE_SELECT_READ)){
		i2c_start();
	};
	data = i2c_data_in(true);
	i2c_stop();

	return data;
}
