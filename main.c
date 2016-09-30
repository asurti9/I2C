#include <stdint.h>
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "eep.h"

void main_delay(uint32_t count)
{
	volatile uint32_t counter = 0x0000;
	while( counter < count) counter++;
}

void main()
{
	GPIO_InitTypeDef gpio;
	uint8_t data;
	uint8_t written_data = 0x60;
	uint16_t address;
	/* reset rcc */
	RCC_DeInit();
	RCC_HSICmd(ENABLE);

	/* enable clock to GPIOB */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_400KHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &gpio);
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
/*
	for(address = 0x0000; address < 0x1FF; address++)
	{
		written_data++;
		eeprom_write(0x0004, written_data);
		data = eeprom_read(0x0004);

		if(data == written_data)
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			GPIO_SetBits(GPIOB, GPIO_Pin_7);
			main_delay(0xFFF0);
		}
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
		main_delay(0xFFF0);
	}
*/
	eeprom_write(0x0050, written_data);
	data = eeprom_read(0x0050);
	for(;;);
}

void assert_param(uint8_t t)
{
  (void)t;
}
