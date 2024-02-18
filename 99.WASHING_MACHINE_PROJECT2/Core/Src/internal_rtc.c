#include "main.h"		// for GPIO & HAL
#include "i2c_lcd.h"
#include "button.h"

void get_rtc(void);									// 시간 설정
void set_rtc(char *date_time);						// 시간 보정
void set_time_button_ui(void);						// main.c 에도 extern 선언

extern RTC_HandleTypeDef hrtc;						// from main.c 53 extern 선언
extern uint8_t lcd_display_mode_flag;				// from button.c 7
extern RTC_TimeTypeDef mTime; 						// from button.c 8

RTC_TimeTypeDef sTime = {0};		// time information 구조체
RTC_DateTypeDef sDate = {0};		// date information 구조체

// 예)23년이 save 된 binary format
// 7654 3210 (high/low nibble)
// 0010 0011
// STM32의 RTC로부터 날짜 & 시각 정보를 읽어 오는 함수 (1초에 한 번씩 초가 바뀔 때만 가져오게)

// BIN -> DEC conversion
unsigned char bin2dec(unsigned char byte)			// argument 를 call by value 로 받음
{
	unsigned char high, low;

	low = byte & 0x0f;					// 하위 4 bit (low nibble) 저장
	high = (byte >> 4)*10;				// 상위 4 bit (high nibble) 저장

	return high + low;
}

// decimal --> BCD 예) 23 -> 001 0111
unsigned char dec2bin(unsigned char byte)
{
	unsigned char high, low;

	high = (byte / 10) << 4; 						// 4 로 shift
	low = byte % 10;								// 나머지

	return high + low;
}
// YYYY-MM-DD HH:mm:ss	<- comportmaster 에 출력됨 (lcd 에는 온습도, 거리 출력됨)
void get_rtc(void)
{
	static RTC_TimeTypeDef oldTime;	// 이전 시각 정보를 가지고 있기 위함이다. (이전에 업데이트된 상태 유지하기 위해 static 변수 씀)
	char lcd_buff[40];								//
	// 날짜, 시각 정보 가져옴
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	// 정보 뿌려줌
	if (oldTime.Seconds != sTime.Seconds)			// 비교
	{
		// YYYY-MM-DD HH:mm:ss	<- comportmaster & lcd 출력
		printf("%04d-%02d-%02d %02d:%02d:%02d\n",
				bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date), 			// bin2dec : BIN -> DEC conversion 함수
				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
		// lcd 뿌리면서 one button 처리
		if (lcd_display_mode_flag == 0)
		{
			// sprintf 로 분리
			sprintf(lcd_buff, "DATE:%04d-%02d-%02d",
					bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date)); 			// bin2dec : BIN -> DEC conversion 함수
			move_cursor(0,0);
			lcd_string(lcd_buff);
			sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
					bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
			move_cursor(1,0);
			lcd_string(lcd_buff);
		}
	}
	oldTime.Seconds = sTime.Seconds; 				// 현재 시간 업데이트
}

// setrtc231016103800 <- comportmaster 의 Send 로 설정하기, Send 누르면 시간 보정된다.
//       678901234567 (6~17번방)
void set_rtc(char *date_time)						// 시간 보정 기능
{
	// 정보 저장할 아스키 어레이 필요
	char yy[4], mm[4], dd[4];		// date
	char hh[4], min[4], ss[4];		// time

	strncpy(yy, date_time+6, 2);	// yy[0]='2' yy[1]='3' yy[2] = 0; (6번째 offset 에서 2byte 자름)
	strncpy(mm, date_time+8, 2);
	strncpy(dd, date_time+10, 2);

	strncpy(hh, date_time+12, 2);
	strncpy(min, date_time+14, 2);
	strncpy(ss, date_time+16, 2);

	// ascii --> int --> bcd 변환해서 RTC 칩에 write
	sDate.Year = dec2bin(atoi(yy));
	sDate.Month = dec2bin(atoi(mm));
	sDate.Date = dec2bin(atoi(dd));

	sTime.Hours = dec2bin(atoi(hh));
	sTime.Minutes = dec2bin(atoi(min));
	sTime.Seconds = dec2bin(atoi(ss));

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

// 세탁기
#if 1
void set_time_button_ui(void)
{

	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		if (lcd_display_mode_flag == 1)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

			move_cursor(0,0);
			lcd_string("Mode_1 : Wash");

		}
	}



#if 0
	char lcd_buff[40];

	if (lcd_display_mode_flag == 3)
	{
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)				// from led.c 38
		{
			// 시간정보 modify
			mTime.Hours++;
			mTime.Hours %= 24;
		}
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			// 분정보 modify
			mTime.Minutes++;
			mTime.Minutes %= 60;

		}
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			// 초정보 modify
			mTime.Seconds++;
			mTime.Seconds %= 60;

		}

		sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
				bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
		move_cursor(0,0);
		lcd_string(lcd_buff);

		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			// 완료 버튼
			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
			mTime.Hours = 0;
			mTime.Minutes = 0;
			mTime.Seconds = 0;
			lcd_display_mode_flag = 0;
/*
			// 부저 test code2
			for (int i=0; i < 5; i++)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
				HAL_Delay(200);
			}
*/
/*
			// 부저 test code1
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
			HAL_Delay(1000);								// 1초
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
*/
		}
	}
#endif

}
#endif





#if 0							// original
// button0 : 시간 정보를 변경하는 버튼. 00~23시 (up counter)
// button1 : 분을 변경하는 버튼. 00~59분 (up counter)
// button2 : 초를 변경하는 버튼. 00~59초 (up counter)
// button3 : 변경 완료. 현재까지 변경된 내용을 저장 (-> HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD); 를 실행)
void set_time_button_ui(void)
{

	if (lcd_display_mode_flag == 3)
	{
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)				// from led.c 38
		{
			// 시간정보 modify

		}
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			// 분정보 modify

		}
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			// 초정보 modify

		}
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			// 완료 버튼
//			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

			// 부저 test code2 -> 부저 소리 조절
			for (int i=0; i < 5; i++)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
				HAL_Delay(200);
			}
/*
			// 부저 test code1
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
			HAL_Delay(1000);								// 1초
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
*/
		}
	}
}
#endif


// 모드 셀렉트 모드 추가 과제 ㅁ시간 보정 저장이...
#if 0
void set_time_button_ui(void)
{
	char lcd_buff[40];

	if (lcd_display_mode_flag == 3)
	{
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)				// from led.c 38
		{
			// 시간정보 modify
			mTime.Hours++;
			mTime.Hours %= 24;
		}
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			// 분정보 modify
			mTime.Minutes++;
			mTime.Minutes %= 60;

		}
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			// 초정보 modify
			mTime.Seconds++;
			mTime.Seconds %= 60;

		}

		sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
				bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
		move_cursor(0,0);
		lcd_string(lcd_buff);

		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			// 완료 버튼
			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
			mTime.Hours = 0;
			mTime.Minutes = 0;
			mTime.Seconds = 0;
			lcd_display_mode_flag = 0;
/*
			// 부저 test code2
			for (int i=0; i < 5; i++)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
				HAL_Delay(200);
			}
*/
/*
			// 부저 test code1
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);		// high
			HAL_Delay(1000);								// 1초
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);		// low
*/
		}
	}
}
#endif
