#include "led.h"

void led_main(void);
void led_all_off(void);
void led_on_up();
void led_on_down();
void led_keepon_up();
void led_keepon_down();
void led_flower_on();
void led_flower_off();

void led_main(void)
{
	while(1)
	{
		// Phase #5
		led_flower_on();
		led_flower_off();

		// Phase #4
		// [STM32] led_keepon_up/down
		// 1. source code 2. 동작 영상 cafe 올리기
		// LED 0->1->2->3->4->5->6->7
		// 앞전에 ON 했던 LED는 그대로 유지
//		led_keepon_up();
//		led_all_off();
		// LED 7->6->5->4->3->2->1->0
		// 앞전에 ON 했던 LED는 그대로 유지
//		led_keepon_down();
//		led_all_off();

		// Phase #3
		// LED 0->1->2->3->4->5->6->7
		// 해당되는 bit의 LED만 ON
//		led_on_up();
		// LED 7->6->5->4->3->2->1->0
//		led_on_down();

		// Phase #2
//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_All);
//		HAL_Delay(500);

		// Phase #1
//		led_all_on();
//		HAL_Delay(300);
//		led_all_off();
//		HAL_Delay(300);
	}
}


void led_flower_on()
{
	for (int i=0; i<4; i++)
	{
		int j = 1;
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		HAL_Delay(j*100);
		j++;
	}
}

void led_flower_off()
{
	for (int i=4; i>0; i--)
	{
		int j = 4;
		led_all_on();
		HAL_GPIO_WritePin(GPIOD, 0xF0 << i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x0F >> i, 0);
		HAL_Delay(i*100);
		j--;
	}
	led_all_off();
	HAL_Delay(50);
}


void led_keepon_up()
{
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

void led_keepon_down()
{
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}


// LED 0->1->2->3->4->5->6->7
// 해당되는 bit의 LED만 ON
void led_on_up()
{
	for (int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

// LED 7->6->5->4->3->2->1->0
void led_on_down()
{
	for (int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);		// 오른쪽으로 shift
		HAL_Delay(200);
	}
}


void led_all_on(void)			// led on 함수
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
//			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_all_off(void)			// led off 함수
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
//			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

