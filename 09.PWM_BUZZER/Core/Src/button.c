#include "button.h"
#include "i2c_lcd.h"
// 초기 버튼 상태 table
char button_status[BUTTON_NUMBER] =
{BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};				// 버튼 하나 추가해서 5개로 늘림 (BUTTON_RELEASE 추가)

extern RTC_HandleTypeDef hrtc;											// from internal_rtc.c 7 extern 선언
RTC_TimeTypeDef mTime;													// from internal_rtc.c 10 extern 선언하지 않고 별도로 mTime 선언

// LCD 출력 >> 모드0:시간 정보 / 모드1:온습도 정보 / 모드2:거리 정보 / 모드3:시각 보정 기능
uint8_t lcd_display_mode_flag = 0;								// 0번 : 시계 정보를 lcd 에 display   ->  internal_rtc.c & DHT11.c 에 extern 선언
void lcd_display_mode_select(void)								// main.c 105 에 extern 선언 후 while 에서 호출시킴
{
	char lcd_buff[40];

	if (get_button(GPIOC, GPIO_PIN_13, 4) == BUTTON_PRESS)		// from main.h 62 63
	{
		lcd_command(CLEAR_DISPLAY);								// from i2c_lcd.c 94 for clear
		lcd_display_mode_flag++;
		lcd_display_mode_flag %= 4;								// 모드 개수 만큼 돌아감

		if (lcd_display_mode_flag == 3)							// 기능 추가
		{
			HAL_RTC_GetTime(&hrtc, &mTime, RTC_FORMAT_BCD);				// from internal_rtc.c 46
			sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
					bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
			move_cursor(1,0);									// 커서 0line 0col 이동
			lcd_string(lcd_buff); 										// HEX 0x20 미만은 LCD 입력 안됨 (\n 안됨)
			move_cursor(1,6);									// 시간 정보 fieid(6번 col) 로 커서 이동
		}

	}
}

// get_button(gpio, pin, button 번호)
// 완전히 눌렀다 떼면 BUTTON_RELEASE(1)을 return
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number)
{
	unsigned char curr_state;																				// 리턴값을 담음

	curr_state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN);		// 0, 1

	// 버튼이 눌려졌으나 처음 상태
	if (curr_state == BUTTON_PRESS && button_status[button_number] == BUTTON_RELEASE)
	{
		HAL_Delay(80);									// noise 지나가길 기다림
		button_status[button_number] = BUTTON_PRESS;					// 현재의 버튼 상태를 저장
		return BUTTON_RELEASE; 							// 버튼이 눌려진 상태나 아직은 noise 상태로 인정
	}
	else if (curr_state == BUTTON_RELEASE && button_status[button_number] == BUTTON_PRESS)
	{
		// 1. 이전에 버튼이 눌려진 상태였고 지금은 버튼을 뗀 상태이면 		(버튼 눌렸다 뗀 상태)
		button_status[button_number] = BUTTON_RELEASE;	// 2. button_status table 을 초기화 하고
		HAL_Delay(30);
		return BUTTON_PRESS;							// 3. 버튼을 1번 눌렀다 뗀 것으로 인정한다.
	}
	return BUTTON_RELEASE; 								// 나머지는 버튼 누르지 않은 상태
}

