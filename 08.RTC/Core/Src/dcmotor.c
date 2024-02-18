#include "button.h"
#include "dcmotor.h"
#include "led.h"

extern TIM_HandleTypeDef htim4;					// from main.c 54
//extern volatile int TIM10_10ms_counter_led;			//a

void dcmotor_pwm_control(void);					// declaration -> main.c 96 에도 extern 으로 선언

// BUTTON0 : start/stop : LED1 (toggle) <- 가시적 정보
// BUTTON1 : SPEED-UP   : LED2 (toggle)
// BUTTON2 : SPEED-DOWN : LED3 (toggle)
uint8_t pwm_start_flag = 0;						// dcmotor start/stop indicator flag	버튼 눌렀는지 알려주는 변수 선언
uint8_t pwm_forward_backward_dcmotor = 0;
uint16_t CCR_UP_Value = 0;						// PWM UP control
uint16_t CCR_DOWN_Value = 0;					// PWM DOWN control		-> channel control register 선언

// 10/13 숙제 ㅁㅁㅁ: 기능추가 speed-up-down -> led_bar_up_down, forword_backword_dcmotor -> LED(PE13) 50ms/100ms 마다 toggle
#if 0
void dcmotor_pwm_control(void)					// main.c 에서 호출해서 계속 돌릴 함수
{
	// start/stop : button0
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)		// from led.c line-38
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);								// LD1 (PB0)

//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);			// test code 정회전
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);			// test code

		if (!pwm_start_flag)									// 처음 상태
		{
			pwm_start_flag = 1;

			if (!pwm_forward_backward_dcmotor)					// forward 일 때
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 1);
			}
			else												// backward 일 때
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
				CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				for (int i = 0; i < CCR_UP_Value/10-2; i++)
					HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);

			}
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
		}
	}

	// speed-up : button1
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);								// LED2
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);		// 현재 PWM 읽어옴
		CCR_UP_Value += 10;
		if (CCR_UP_Value > 100) CCR_UP_Value = 100;							// -> 오실로스코프에서 버튼1 3번 누르면 듀티 100까지 증가
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);			// H/W에 PWM 업데이트

//		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		for (int i=0; i <= CCR_UP_Value/10-2 ; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		}
	}

	// speed-down : button2
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);								// LED3
		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);		// PWM 읽어옴
		CCR_DOWN_Value -= 10;
		if (CCR_DOWN_Value < 60) CCR_DOWN_Value = 60;						// 모터 속도 디폴트 70 -> 오실로스코프에서 버튼2 누르면 60까지 감소 가능
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);

		HAL_GPIO_WritePin(GPIOD, 0xFF, 0);
		for (int i=0; i <= CCR_DOWN_Value/10-2 ; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		}
//		HAL_GPIO_WritePin(GPIOD, 0x01 << (CCR_DOWN_Value/10)-1, 0);
	}

	// forword_backword_dcmotor : button3
	if (get_button(BUTTON3_GPIO_Port, BUTTON2_Pin, 3) == BUTTON_PRESS)
	{
		if (!pwm_forward_backward_dcmotor)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			pwm_forward_backward_dcmotor = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			pwm_forward_backward_dcmotor = 0;
		}
	}

	if (pwm_start_flag)
	{
		if (!pwm_forward_backward_dcmotor)
		{
			if (TIM10_10ms_counter_led >= 5)
			{
				TIM10_10ms_counter_led = 0;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
		}
		else
		{
			if (TIM10_10ms_counter_led >= 10)
			{
				TIM10_10ms_counter_led = 0;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
		}
	}
}
#endif

// 버튼 012 -> start/stop, speed-up, speed-down
#if 1																		// original code
void dcmotor_pwm_control(void)					// main.c 에서 호출해서 계속 돌릴 함수
{
	// start/stop : button0
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)		// from led.c line-38
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);								// LD1 (PB0)

//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);			// test code 정회전 	-> 동작 x...
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);			// test code		-> 동작 x...

		if (!pwm_start_flag)												// 처음 상태
		{
			pwm_start_flag = 1;

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);			// test code
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);			// test code

			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);			// test code
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);			// test code

		}
	}

	// speed-up : button1
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);								// LED2
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);		// PWM 읽어옴
		CCR_UP_Value += 10;
		if (CCR_UP_Value > 100) CCR_UP_Value = 100;							// -> 오실로스코프에서 버튼1 3번 누르면 듀티 100까지 증가
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);			// H/W에 PWM 업데이트
	}

	// speed-down : button2
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);								// LED3
		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);		// PWM 읽어옴
		CCR_DOWN_Value -= 10;
		if (CCR_DOWN_Value < 60) CCR_DOWN_Value = 60;						// 모터 속도 디폴트 70 -> 오실로스코프에서 버튼2 누르면 60까지 감소 가능
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
	}
}
#endif																		// original code

