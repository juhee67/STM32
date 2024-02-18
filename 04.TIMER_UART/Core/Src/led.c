#include "led.h"
#include "button.h"							// 선언

// #define LED_ON_UP 0
// #define LED_ON_DOWN 1

#define LED_ALL_ON 0
#define LED_ALL_OFF 1
#define LED_FLOWER_ON 2
#define LED_FLOWER_OFF 3
#define LED_KEEPON_UP 4
#define LED_KEEPON_DOWN 5
#define LED_ON_UP 6
#define LED_ON_DOWN 7

void led_main(void);
void led_all_on(void);
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

extern volatile int t1ms_counter;					// main.c 코드 복사 -> 초기값 빼줌

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

/*
// case문 이용
void button0_toggle(void)
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
*/

void (*funcp[])()=
{
		led_all_on,			// 1
		led_all_off,		// 2
		led_flower_on,		// 3
		led_flower_off,		// 4
		led_keepon_up,		// 5
		led_keepon_down,	// 6
		led_on_up,			// 7
		led_on_down			// 8
};

void button0_toggle(void)
{
	static int button0_count = 0;

	if (button0_count >= 0)
	{
		funcp[button0_count]();
	}
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8;
	}
}

int func_index = 0;											// 전역변수

void led_main(void)
{
	while(1)
	{
#if 1
		button0_toggle();
#endif

#if 0
		// Phase #2-6 (delete HAL_Delay)
		// [STM32] circular_function
		// 1 : led_all_on()
		// 2 : led_all_off()
		// 3 : led_flower_on()
		// 4 : led_flower_off()
		// 5 : led_keepon_up()
		// 6 : led_keepon_down()
		// 7 : led_on_up()
		// 8 : led_on_down()

		switch(func_index)
		{
		case LED_ALL_ON:			// 1
			led_all_on();
			break;
		case LED_ALL_OFF:			// 2
			led_all_off();
			break;
		case LED_FLOWER_ON:			// 3
			led_flower_on();
			break;
		case LED_FLOWER_OFF:		// 4
			led_flower_off();
			break;
		case LED_KEEPON_UP:			// 5
			led_keepon_up();
			break;
		case LED_KEEPON_DOWN:		// 6
			led_keepon_down();
			break;
		case LED_ON_UP:				// 7
			led_on_up();
			break;
		case LED_ON_DOWN:			// 8
			led_on_down();
			break;
		default:
			break;
		}
#endif

#if 0
		// Phase #2-4 (delete HAL_Delay)
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0; 							// clear
			// 200ms 마다 toggle
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);		// LED1
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LED2
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);		// LED3
			HAL_Delay(200);								// 200ms delay			// ??
		}
#endif

#if 0
		// 내장 led 200ms 마다 toggle
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);		// LED1
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LED2
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);		// LED3
		HAL_Delay(200);								// 200ms delay
#endif

#if 0
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
		// Phase #3 	// Phase #2-5 (delete HAL_Delay)
		// LED 0->1->2->3->4->5->6->7
		// 해당되는 bit의 LED만 ON
		switch(func_index)
		{
		case LED_ON_UP:
			led_on_up();
			break;
		case LED_ON_DOWN:
			led_on_down();
			break;
		default:
			break;
		}
//		led_on_up();
		// LED 7->6->5->4->3->2->1->0
//		led_on_down();
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
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		i++;
	}
	if (i > 4)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
	}

// circular function
/*
#if 1
	static int i = 0;
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		i++;

		if(i >= 4)
		{
			i = 0;
			func_index = LED_FLOWER_OFF;
		}
	}

#else
	for (int i=0; i<4; i++)							// <original code
	{
		int j = 1;
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		HAL_Delay(j*100);
		j++;
	}												// original code>
#endif
*/
}

void led_flower_off(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		i++;
	}
	if (i > 4)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 1);
	}

// circular function
/*
#if 1
	static int i = 0;
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		i++;

		if (i >= 4)
		{
			i = 0;
			func_index = LED_KEEPON_UP;
		}
	}
#else
	for (int i=4; i>0; i--)							// <original code
	{
		int j = 4;
		led_all_on();
		HAL_GPIO_WritePin(GPIOD, 0xF0 << i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x0F >> i, 0);
		HAL_Delay(i*100);
		j--;
	}
	led_all_off();
	HAL_Delay(50);									// original code>
#endif
*/
}

void led_keepon_up(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		i++;
	}
	if (i > 8)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
	}

// circular function
/*
#if 1
	static int i = 0;
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		i++;

		if (i >= 9)
		{
			i = 0;
			HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
			func_index = LED_KEEPON_DOWN;
		}
	}
#else
	for (int i=0; i<8; i++)							// <original code
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}												// original code>
#endif
*/
}

void led_keepon_down(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
	}
	if (i > 8)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
	}

// circular function
/*
#if 1
	static int i = 0;
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;

		if (i >= 9)
		{
			i = 0;
			HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
			func_index = LED_ON_UP;
		}
	}
#else
	for (int i=0; i<8; i++)							// <original code
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}												// original code>
#endif
*/
}

// LED 0->1->2->3->4->5->6->7		해당되는 bit의 LED만 ON
void led_on_up(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)						// for 문 찢어놓음				// 119 46:00
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		i++;
	}
	if (i > 8)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
	}

// circular function
/*
#if 1
	if (t1ms_counter >= 200)						// for 문 찢어놓음				// 119 46:00
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		i++;

		if ( i >= 8)
		{
			i = 0;
			func_index = LED_ON_DOWN;
		}
	}
#else
	for (int i=0; i<8; i++)							// <original code
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}												// original code>
#endif
*/
}

// LED 7->6->5->4->3->2->1->0		해당되는 bit의 LED만 ON
void led_on_down(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
	}
	if (i > 8)
	{
		i = 0;
		func_index = (func_index + 1) % 8;
	}

// circular function
/*
#if 1
	if (t1ms_counter >= 200)						// for 문 찢어놓음				// 119? 46:00
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;

		if ( i >= 8)
		{
			i = 0;
			func_index = LED_ALL_ON;
		}
	}
#else
	for (int i=0; i<8; i++)							// <original code
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);		// 오른쪽으로 shift
		HAL_Delay(200);
	}												// original code>
#endif
*/
}

void led_all_on(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
//			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 1);
		func_index = LED_ALL_OFF;
	}
}

void led_all_off(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
//			|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;
		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		func_index = LED_FLOWER_ON;
	}
}

