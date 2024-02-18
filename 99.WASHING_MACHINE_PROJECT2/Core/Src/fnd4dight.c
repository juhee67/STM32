#include "main.h"
#include "fnd4digit.h"
#include "button.h"

extern volatile int fnd1ms_counter;  // fnd1ms

void fnd4digit_sec_clock(void);

uint16_t FND_digit[4] =
{
	FND_d1, FND_d10, FND_d100, FND_d1000
};

uint32_t FND_font[10] =
{
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f,   // 0
  FND_b|FND_c,                           // 1
  FND_a|FND_b|FND_d|FND_e|FND_g,         // 2
  FND_a|FND_b|FND_c|FND_d|FND_g,         // 3
  FND_b|FND_c|FND_f|FND_g,   // 4
  FND_a|FND_c|FND_d|FND_f|FND_g,  // 5
  FND_a|FND_c|FND_d|FND_e|FND_f|FND_g,  // 6
  FND_a|FND_b|FND_c,      // 7
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f|FND_g,   // 8
  FND_a|FND_b|FND_c|FND_d|FND_f|FND_g   // 9
};

uint16_t FND[4];    // FND에 쓰기 위한 값을 준비하는 변수

void fnd4digit_main(void)									// original code
{
	unsigned int value=0;   // 1초가 되었을때 up count
	unsigned int msec=0;    // ms counter
	static int i=0;     // FND position indicator 	어떤 FND 킬지

	FND4digit_off();

	while(1)
	{
#if 1   // SYSTICK interrupt 방식
		if (fnd1ms_counter >= 2)   // 2ms reached
		{
			fnd1ms_counter=0;
			msec += 2;   // 2ms
			if (msec > 1000)   // 1000ms reached
			{
				msec = 0;		// clear
				value++;       // sec count를 증가
				FND_update(value);		// 2ms 마다 FND 업데이트 하는 함수 호출
			}
			FND4digit_off(); 	//  FND all off
#if 0 // common 애노우드  WCN4-
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_SET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_RESET);
#else // common 캐소우드 CL5642AH30
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
#endif
			i++;   // 다음 display할 FND를 가리킨다.
			i %= 4;
		}
#else   // orginal delay 방식
		msec += 8;   // 8ms
		if (msec > 1000)   // 1000ms reached
		{
			msec = 0;
			value++;       // sec count를 증가
			FND_update(value);
		}
		for (int i=0; i < 4; i++)
		{
			FND4digit_off();
			HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
			HAL_Delay(2);
		}
#endif
	}
}


void fnd4digit_sec_clock(void)						// original code copy 해서 수정 (for fnd 함수로 연동)
{
	static unsigned int value=0;   // 1초가 되었을때 up count
	static unsigned int msec=0;    // ms counter
	static int i=0;     // FND position indicator 	어떤 FND 킬지

//	FND4digit_off();			// clear
	if (fnd1ms_counter >= 2)   // 2ms reached
	{
		fnd1ms_counter=0;
		msec += 2;   // 2ms
		if (msec > 1000)   // 1000ms reached
		{
			msec = 0;		// clear
			value++;       // sec count를 증가
			FND_update(value);		// 2ms 마다 FND 업데이트 하는 함수 호출
		}
		FND4digit_off(); 	//  FND all off
#if 0 // common 애노우드  WCN4-
		HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_SET);
		HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_RESET);
#else // common 캐소우드 CL5642AH30
		HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);
#endif
		i++;   // 다음 display할 FND를 가리킨다.
		i %= 4;
	}
}


void FND_update(unsigned int value)
{
	FND[0] = FND_font[value % 10];
	FND[1] = FND_font[value / 10 % 10];
	FND[2] = FND_font[value / 100 % 10];			// 100단위 10으로 나눠진 나머지 정수로 출력
	FND[3] = FND_font[value / 1000 % 10];

	return;
}
void FND4digit_off(void)
{
// 에노드 캐소드 선택
#if 0 // common 에노우드 WCN4-
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#else // common 캐소우드 CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#endif
	return;
}
void FND4digit_on(void)
{
#if 0  // common 에노우드
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
#else  // CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
#endif
	return;
}













