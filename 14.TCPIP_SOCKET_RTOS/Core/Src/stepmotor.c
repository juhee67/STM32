#include "main.h"
#include "button.h"

extern void delay_us(unsigned long us);
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);

void set_rpm(int rpm);
void stepmotor_main_test(void);
void stepmotor_drive(int step);

// RPM(Revolution per Minutes) : 분딩 회전수
// 1분 : 60sec : 1,000,000us(1초) X 60 = 60,000,000us
// 1,000,000us(1초)
// --> 1초(1000ms) ==> 1ms(1000us) X 1000ms ==> 1,000,000us
// 4096스텝 : 1바퀴(4096스텝 이동)
// -- 1바퀴 도는데 필요한 총 스텝 수 : 4096
// 4096 / 8(0.7) ==> 512 sequence : 360도
// 1 sequence(8 step) : 0.70312도
// 0.70312도 X 512 sequence = 360도

//------ set_rpm(13) 함수 분당 13번으로 지정시의 동작 상황 ------//
// 60,000,000us = 1분 / 4096스텝 돌아감 / rpm
// 1126us(1스텝 idle타임) X 4096 = 4,612,096us = 4612ms = 4.6초(1바퀴 도는 시간)
// 1분에 몇 바퀴? 60초 / 4.6초(1회전 소요시간) = 13회전
void set_rpm(int rpm)	// rpm 1~13
{
	delay_us(60000000/4096/rpm);
	// 최대 speed 기준(13) : delay_us(1126);
}

// 시계방향으로 1회전 <--> 반시계방향으로 1회전
void stepmotor_main_test(void)
{
#if 0
// 스텝모터 버튼 제어 코드 작성

	int btn0_count = 0;
	int btn1_count = 0;

	int direction = 0;

	direction %= 2;
	direction++;

	while(1)
	{
		if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			btn0_count++;

		}
		if(get_button(BUTTON0_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			btn1_count++;

		}
	}


#else
	// original code // 시계방향으로 1회전 <--> 반시계방향으로 1회전
//	while(1)
//	{
		// 시계방향
		for(int i=0; i<512; i++)	// 시계방향으로 1회전
		{
			for (int j=0; j<8; j++)	// 1sequence가 8번 돈다
			{
				stepmotor_drive(j);
				set_rpm(13);
			}
		}

		// 반시계방향(8step 반대로)
		for(int i=0; i<512; i++)	// 반시계방향으로 1회전
		{
			for (int j=7; j>=0; j--) // 1sequence가 8번 돈다
			{
				stepmotor_drive(j);
				set_rpm(13);	// rpm 값만큼 wait
			}
		}
//	}
#endif
}

#if 0
// 스텝모터 버튼 제어 코드 작성

// direction : 1 == 시계방향 회전
// 			   2 == 반시계
// 			   0 : idle
void stepmotor_drive(int direction)
{
	static int step = 0;	// static 을 쓰면 전역변수처럼 동작

	switch(step){
	case 0:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);		// 스텝모터 PPT 표 참고해서 작성
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	}

	// 스텝모터 버튼 제어 코드 작성
	if (direction == 1)	// 정회전
	{
		step++;	// for (step=0; step<8; step++)
		step %= 8;	// 다음 진행할 step 준비
		if (step >= 8) step = 0;
	}
	else if (direction == 2)	// 역회전
	{
		step--;
		if (step < 0) step = 7;
	}

}


#else	// original code
void stepmotor_drive(int step)
{
	switch(step){
	case 0:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);	// 스텝모터 PPT 표 참고해서 작성
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	}
}
#endif
