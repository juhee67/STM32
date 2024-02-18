#include "led.h"
#include "button.h"									// 선언

void led_main(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void led_keepon_up(void);
void led_keepon_down(void);
void led_flower_on(void);
void led_flower_off(void);

void button0_led_all_on_off_toggle(void);
void button0_toggle(void);
void led_bar_up(void);
void led_bar_down(void);


void led_button_demo();


void led_button_demo()
{
	static int button0_count = 0;
	static int button1_count = 0;
	static int button2_count = 0;
	static int button3_count = 0;

	// 버튼을 한 번 눌렀다 뗀 상태라면
	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 2;
		if(button0_count)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
		}
	}

	if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		button1_count++;
		button1_count %= 2;
		if(button1_count)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
		}
	}
	if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		button2_count++;
		button2_count %= 2;
		if(button2_count)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		}
	}
	if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		button3_count++;
		button3_count %= 2;
		if(button3_count)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		}
	}
}


void button0_led_all_on_off_toggle(void)			// 함수 	 Phase #2-1
{
	static int button0_count = 0;					// static으로 선언하면 전역변수처럼 동작한다. (토글하려면 함수 이전상태 변수 있어야)

	// 버튼을 한 번 눌렀다 뗀 상태라면
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)										// main.h에서 BUTTON0_GPIO_Port 복사
	{
		button0_count++;
		button0_count %= 2;
		if (button0_count)							// 나머지가 1이면 led 전체 on
		{
			led_all_on();
		}
		else										// led 전체 on
		{
			led_all_off();
		}
	}
}

void button0_toggle(void)							// Phase #2-2
{
	// 1번 누름 : led_all_on()
	// 2 	  : led_all_off()
	// 3      : led_flower_on()
	// 4      : led_flower_off()
	// 5      : led_keepon_up()
	// 6	  : led_keepon_down()
	// 7 	  : led_on_up()
	// 8 	  : led_on_down()
	static int button0_count = 0;

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8;

		switch (button0_count)
		{
			case 1:
				led_all_on(); break;
			case 2:
				led_all_off(); break;
			case 3:
				led_flower_on(); break;
			case 4:
				led_flower_off(); break;
			case 5:
				led_keepon_up(); break;
			case 6:
				led_keepon_down(); break;
			case 7:
				led_on_up(); break;
			case 0:
				led_on_down(); break;
		}
	}
}


void led_bar_up(void)
{
	static int button0_count = 0;

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8;

		int i = button0_count;
		for (i=0; i<8; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
//			HAL_Delay(200);
		}
	}
}

void led_bar_down(void)
{
	static int button0_count = 0;

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8;

		int i = button0_count;
		for (i=0; i<8; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
//			HAL_Delay(200);
		}
	}

}


void led_main(void)
{
	while(1)
	{

#if 1
		// Phase #2-3
		led_bar_up();
		led_bar_down();
#endif

#if 0
		// Phase #2-2
		// [STM32] button_toggle
		button0_toggle();
#endif

#if 0
		// Phase #2-1
		button0_led_all_on_off_toggle();			// 버튼 한 번 누르면 led 전체 on/off
#endif

#if 0
		// Phase #5
		led_flower_on();
		led_flower_off();
#endif

#if 0
		// Phase #4
		// [STM32] led_keepon_up/down
		// 1. source code 2. 동작 영상 cafe 올리기
		// LED 0->1->2->3->4->5->6->7
		// 앞전에 ON 했던 LED는 그대로 유지
		led_keepon_up();
		led_all_off();
		// LED 7->6->5->4->3->2->1->0
		// 앞전에 ON 했던 LED는 그대로 유지
		led_keepon_down();
		led_all_off();
#endif

#if 0
		// Phase #3
		// LED 0->1->2->3->4->5->6->7
		// 해당되는 bit의 LED만 ON
		led_on_up();
		// LED 7->6->5->4->3->2->1->0
		led_on_down();
#endif

#if 0
		// Phase #2
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_All);
		HAL_Delay(500);
#endif

#if 0
		// Phase #1
		led_all_on();
		HAL_Delay(300);
		led_all_off();
		HAL_Delay(300);
#endif

	}
}


void led_flower_on(void)
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

void led_flower_off(void)
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


void led_keepon_up(void)
{
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

void led_keepon_down(void)
{
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}


// LED 0->1->2->3->4->5->6->7
// 해당되는 bit의 LED만 ON
void led_on_up(void)
{
	for (int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
}

// LED 7->6->5->4->3->2->1->0
void led_on_down(void)
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

