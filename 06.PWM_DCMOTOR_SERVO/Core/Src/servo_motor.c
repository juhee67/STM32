#include "main.h"

void servo_motor_test_main(void);		// 선언, main.c에도 extern 선언

extern TIM_HandleTypeDef htim2;			// from main.c 53

// 84000000HZ/1680 = 50000HZ = 50kHZ
// T = 1/f = 1/50000HZ = 0.00002s = 20us
// 2ms (180도) : 0.00002 x 100개
// 1.5ms (90도) : 0.00002 x 75개
// 1ms (0도) : 0.00002 x 50개
void servo_motor_test_main(void)
{
	while (1)													// test code		10/13 숙제 2-2
	{
		// 180도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 130);		// 180도
		HAL_Delay(1000);

		// 90도
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90);		// 90도
		HAL_Delay(1000);

		// 0도
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 50);		// 0도
		HAL_Delay(1000);
	}
}

