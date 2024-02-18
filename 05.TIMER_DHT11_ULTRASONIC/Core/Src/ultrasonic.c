#include "ultrasonic.h"

extern volatile int TIM10_10ms_ultrasonic;						// from main.c
void ultrasonic_processing(void);								// declaration -> main.c 87 에도 extern 선언

extern void lcd_string(uint8_t *str);					//from i2c_lcd.h
extern void move_cursor(uint8_t row, uint8_t column);	//from i2c_lcd.h

volatile int distance; 											// 거리를 측정한 펄스 개수를 저장하는 변수 선언
volatile int ic_cpt_finish_flag = 0;							// 초음파 거리 측정 완료 indicator 변수 선언
// Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c 에 가서
// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)을 잘라내서 이곳으로 가져온다.
// 2. 초음파 센서의 ECHO 핀의 상승 엣지와 하강 엣지 발생 시 이곳으로 들어온다.
volatile uint8_t is_first_capture = 0;							// 0:상승엣지 1:하강엣지 (for rising, falling edge 구분을 위한 변수)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3)												//
	{
		if ( is_first_capture == 0)											// 상승엣지
		{
			__HAL_TIM_SET_COUNTER(htim, 0);									// clear H/W counter (변수 초기화 필요, 0 넣어줌)
			is_first_capture = 1;											// 상승엣지를 만났다는 flag 변수 indicator 를 set
		}
		else if (is_first_capture == 1)										// 하강엣지를 만나면
		{
			is_first_capture = 0;											// 다음 echo 펄스를 count 하기 위해 변수 초기화 필요
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);			// 현재까지 count 한 펄스 수를 읽어온다. -> 변수 선언 필요 (line-3)
			ic_cpt_finish_flag = 1; 										// 초음파 거리 측정 완료 -> 변수 선언 필요 (line-4)
		}
	}
}

#if 1										// original
void ultrasonic_processing(void)								// main.c 에서 계속 부름
{
	int dis;		// 거리를 측정한 펄스 개수를 저장하는 변수 선언 -> 지역변수
	char lcd_buff[20];						//

	if (TIM10_10ms_ultrasonic >= 100)				// 1초
	{
		TIM10_10ms_ultrasonic = 0; 					// 다음 1초를 위해 clear
		make_trigger();								// 호출
		if (ic_cpt_finish_flag) 					// 초음파 센서 측정 완료되었으면		if (ic_cpt_finish_flag >= 1) 코드와 동일
		{
			ic_cpt_finish_flag = 0;
			dis = distance;										// 전역 변수를 복사한 값 사용 -> dis 지역변수 선언 필요
			dis = dis * 0.034 / 2;					// 1us 가 034cm 를 이동
			// 왕복 거리를 리턴해주기 때문에 나누기2 해주는 것이다.
			printf("dis : %dcm", dis);
			sprintf(lcd_buff, "dis:%d", dis);
			move_cursor(1,0);			// 커서 1line 0col 이동
			lcd_string(lcd_buff); 		// HEX 0x20 미만은 LCD 입력 안됨 (\n 안됨)
		}
	}
}
#endif										// original

#if 0
// 10/12 숙제 comportmaster -> ultra_on/off
volatile int ultrasonic_on_off_flag;
void ultrasonic_processing(void)							// main.c 에서 계속 부름
{
	if (ultrasonic_on_off_flag)								// 거리를 측정한 펄스 개수를 저장하는 변수 선언 -> 지역변수
	{
		int dis;

		if (TIM10_10ms_ultrasonic >= 100)				// 1초
		{
			TIM10_10ms_ultrasonic = 0; 					// 다음 1초를 위해 clear
			make_trigger();								// 호출

			if (ic_cpt_finish_flag) 					// 초음파 센서 측정 완료되었으면		if (ic_cpt_finish_flag >= 1) 코드와 동일
			{
				ic_cpt_finish_flag = 0;
				dis = distance;									// 전역 변수를 복사한 값 사용 -> dis 지역변수 선언 필요
				dis = dis * 0.034 / 2;					// 1us 가 034cm 를 이동
				// 왕복 거리를 리턴해주기 때문에 나누기2 해주는 것이다.
				printf("dis : %dcm\n", dis);
			}
		}
	}
}
#endif

void make_trigger(void)											// trigger 구형파 완성 (Trigger Input to module - square pulse)
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);		// from main.h line-82, 83
	delay_us(2);				// 2us 로 low
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 1);
	delay_us(10);				// high 로 10us 준다
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
}

