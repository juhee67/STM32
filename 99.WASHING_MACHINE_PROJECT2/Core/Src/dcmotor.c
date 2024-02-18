#include "main.h"
#include "button.h"
#include "dcmotor.h"
#include "i2c_lcd.h"
#include "led.h"

extern TIM_HandleTypeDef htim4;					// from main.c 54
extern uint8_t lcd_display_mode_flag;
extern volatile int TIM10_10ms_counter_pwm;
extern volatile int TIM10_10ms_toggle;
extern unsigned int school_bell[];


//extern TIM_HandleTypeDef htim2;

extern void lcd_string(uint8_t *str);			//from i2c_lcd.h
extern void move_cursor(uint8_t row, uint8_t column);	//from i2c_lcd.h
extern void servo_open_hole(void);
extern void buzzer_finish();
extern void buzzer_stop();
void noTone();

//extern volatile int TIM10_10ms_counter_led;			//a

void dcmotor_pwm_control(void);					// declaration -> main.c 96 에도 extern 으로 선언

// BUTTON0 : start/stop : LED1 (toggle) <- 가시적 정보
// BUTTON1 : SPEED-UP   : LED2 (toggle)
// BUTTON2 : SPEED-DOWN : LED3 (toggle)
uint8_t pwm_start_flag = 0;						// dcmotor start/stop indicator flag	버튼 눌렀는지 알려주는 변수 선언
uint8_t pwm_forward_backward_dcmotor = 0;
uint16_t CCR_UP_Value = 0;						// PWM UP control
uint16_t CCR_DOWN_Value = 0;					// PWM DOWN control		-> channel control register 선언

uint8_t wash_start_press_btn2=0;
uint8_t wash_finish_flag=0;
//uint16_t CCR_UP_CCR_Value = 0;  // PWM UP control
//uint16_t CCR_DOWN_CCR_Value = 0;  // PWM DOWN control


// 세탁기
#if 2
void dcmotor_pwm_control(void)					// main.c 에서 호출해서 계속 돌릴 함수
{
	uint8_t lcd_buff[40];

	// Mode_0 Menu
	if (lcd_display_mode_flag == 0)
	{
		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode Select!");
		lcd_string(lcd_buff);

		move_cursor(1,0);
		sprintf((char *) lcd_buff, "- Push button1 -");
		lcd_string(lcd_buff);
	}

	// Mode_1 Wash : button1
	if (lcd_display_mode_flag == 1)
	{
		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);				// LD1 (PB0)

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode1 : Wash");
		lcd_string(lcd_buff);


		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD
			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Supply Water");		// 1. 급수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		// if (wash_start_press_btn2)
		{

			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 2. 급수
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )				// 2. 급수
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 세탁
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Washing");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}
			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 탈수
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dehydration");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}

			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 900)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_finish();

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish Washing!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				wash_finish_flag=1;

				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Running!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				wash_finish_flag=1;

				lcd_display_mode_flag = 1;
			}
		}
	}

	// Mode_2 Dry : button1
	if (lcd_display_mode_flag == 2)
	{
		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LD2

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode2 : Dry");
		lcd_string(lcd_buff);

		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD
			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Drain Water");		// 1. 배수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;

			// Servo
			servo_open_hole();
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		{
			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 1. 배수 토글
			{
				if (TIM10_10ms_toggle >= 5)					// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);	// LED Yellow
				}

				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 건조 정방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry forward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}


			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 건조 역방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry backward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}
			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 900)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish Drying!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				// Buzzer
				buzzer_finish();

				wash_finish_flag=1;


				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Drying!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				lcd_display_mode_flag = 2;
			}

		}
	}

	// Mode_3 Wash & Dry: button1
	if (lcd_display_mode_flag == 3)
	{
		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);				// LD1 (PB0)

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "Mode3 : Wash&Dry");
		lcd_string(lcd_buff);


		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD

			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Supply Water");		// 1. 급수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		// if (wash_start_press_btn2)
		{

			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 1. 급수 토글
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 세탁
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Washing");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}
			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 탈수
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dehydration");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}

			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// Dry
			if (TIM10_10ms_counter_pwm  == 900)
			{
				i2c_lcd_init();
				move_cursor(0,0);
				sprintf((char *) lcd_buff, "Mode3 : Wash&Dry");
				lcd_string(lcd_buff);

				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Drain Water");	// 4. 배수
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Servo
				servo_open_hole();
			}
			if (TIM10_10ms_counter_pwm > 900 && TIM10_10ms_counter_pwm < 1200)
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
			}

			// DC
			if (TIM10_10ms_counter_pwm  == 1200)				// 5. 건조 정방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry forward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}


			if (TIM10_10ms_counter_pwm > 1200 && TIM10_10ms_counter_pwm < 1500)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 1500)				// 6. 건조 역방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry backward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}
			if (TIM10_10ms_counter_pwm > 1500 && TIM10_10ms_counter_pwm < 1800)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 1800)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				// Buzzer
				buzzer_finish();

				wash_finish_flag=1;

				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Running!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				lcd_display_mode_flag = 3;
			}

		}
	}
}
#endif



// 세탁기1
#if 0
void dcmotor_pwm_control(void)					// main.c 에서 호출해서 계속 돌릴 함수
{
	uint8_t lcd_buff[40];
	noTone();

	// Mode_0 Menu
	if (lcd_display_mode_flag == 0)
	{
		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode Select!");
		lcd_string(lcd_buff);

		move_cursor(1,0);
		sprintf((char *) lcd_buff, "- Push button1 -");
		lcd_string(lcd_buff);
	}

	// Mode_1 Wash : button1
	if (lcd_display_mode_flag == 1)
	{
		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);				// LD1 (PB0)

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode1 : Wash");
		lcd_string(lcd_buff);


		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD
			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Supply Water");		// 1. 급수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		// if (wash_start_press_btn2)
		{

			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 1. 급수
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )				// 1. 급수
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 세탁
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Washing");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}
			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 탈수
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dehydration");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}

			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 900)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_finish();

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish Washing!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				wash_finish_flag=1;

				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Washing!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				wash_finish_flag=1;

				lcd_display_mode_flag = 1;
			}
		}
	}

	// Mode_2 Dry : button1
	if (lcd_display_mode_flag == 2)
	{

		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LD2

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "  Mode2 : Dry");
		lcd_string(lcd_buff);

		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD
			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Drain Water");		// 1. 배수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;

			// Servo
			servo_open_hole();
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		{
			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 1. 배수 토글
			{
				if (TIM10_10ms_toggle >= 5)					// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);	// LED Yellow
				}

				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 건조 정방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry forward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}


			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 건조 역방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry backward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}
			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 900)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_finish();

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish Drying!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				wash_finish_flag=1;


				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Drying!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				lcd_display_mode_flag = 2;
			}

		}
	}

	// Mode_3 Wash & Dry: button1
	if (lcd_display_mode_flag == 3)
	{
		// LED
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);				// LD1 (PB0)

		move_cursor(0,0);
		sprintf((char *) lcd_buff, "Mode3 : Wash&Dry");
		lcd_string(lcd_buff);


		// Start : button2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			wash_start_press_btn2=1;
			wash_finish_flag=0;

			// LCD

			move_cursor(1,0);
			sprintf((char *) lcd_buff, "Run:Supply Water");		// 1. 급수
			lcd_string(lcd_buff);

			TIM10_10ms_counter_pwm=0;
		}

		if (wash_start_press_btn2 && !wash_finish_flag)
		// if (wash_start_press_btn2)
		{

			// DC
			if (TIM10_10ms_counter_pwm < 200 )				// 1. 급수 토글
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
				return;
			}
			if (TIM10_10ms_counter_pwm == 200 )
			{
				TIM10_10ms_toggle=0;
			}
			// DC
			if (TIM10_10ms_counter_pwm  == 300)				// 2. 세탁
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Washing");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}
			if (TIM10_10ms_counter_pwm > 300 && TIM10_10ms_counter_pwm < 600)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 600)				// 3. 탈수
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dehydration");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}

			if (TIM10_10ms_counter_pwm > 600 && TIM10_10ms_counter_pwm < 900)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// Dry
			if (TIM10_10ms_counter_pwm  == 900)
			{
				i2c_lcd_init();
				move_cursor(0,0);
				sprintf((char *) lcd_buff, "Mode3 : Wash&Dry");
				lcd_string(lcd_buff);

				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Drain Water");	// 4. 배수
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Servo
				servo_open_hole();
			}
			if (TIM10_10ms_counter_pwm > 900 && TIM10_10ms_counter_pwm < 1200)
			{
				if (TIM10_10ms_toggle >= 5)							// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_4);			// LED Yellow
				}
			}

			// DC
			if (TIM10_10ms_counter_pwm  == 1200)				// 5. 건조 정방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry forward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}


			if (TIM10_10ms_counter_pwm > 1200 && TIM10_10ms_counter_pwm < 1500)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);		// LED Green
				}
			}
			if (TIM10_10ms_counter_pwm  == 1500)				// 6. 건조 역방향
			{
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Run:Dry backward");
				lcd_string(lcd_buff);

				HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);		// 역회전
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			}
			if (TIM10_10ms_counter_pwm > 1500 && TIM10_10ms_counter_pwm < 1800)
			{
				if (TIM10_10ms_toggle >= 5)						// 50ms 마다
				{
					TIM10_10ms_toggle = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6);		// LED Green
				}
			}

			// finish
			if (TIM10_10ms_counter_pwm == 1800)				// 4. 완료
			{
				TIM10_10ms_counter_pwm=0;

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_finish();

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Finish!");
				lcd_string(lcd_buff);
				HAL_Delay(3000);

				wash_finish_flag=1;

				lcd_display_mode_flag = 0;
			}


			// Stop : button3
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 일시 정지
			{

				// LCD
				i2c_lcd_init();
				move_cursor(1,0);
				sprintf((char *) lcd_buff, "Stop Running!");
				lcd_string(lcd_buff);

				// DC
				HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);		// 정지
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

				// Buzzer
				buzzer_stop();

				lcd_display_mode_flag = 3;
			}

		}
	}
}
#endif


#if 0												// original
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
#endif






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

// 숙제 10/13 <코드 참고1> dcmotor pwm 제어...
#if 0
void dcmotor_pwm_control(void)
{
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);	// LED1

		if (pwm_start_flag == 0)
		{

			pwm_start_flag = 1;
			if (pwm_forward_backward_dcmotor == 0)		//forward
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
				CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				for (int i = 0; i < CCR_UP_Value/10-2; i++)
					HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
			}
			else								//backward
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
				CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				for (int i = 0; i < CCR_UP_Value/10-2; i++)
					HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
			}
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
		}
	}
	// speed-up : button1
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_Value += 10;
		if (CCR_UP_Value > 100) CCR_UP_Value = 100;
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
		for (int i = 0; i < CCR_UP_Value/10-2; i++)
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
	}
	// speed-down : button2
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_Value -= 10;
		if (CCR_UP_Value < 30) CCR_UP_Value = 30;
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
		for (int i = 0; i < CCR_UP_Value/10-2; i++)
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
	}
	if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		if (pwm_forward_backward_dcmotor == 0)	// forward -> backward
		{
			pwm_forward_backward_dcmotor = 1;
		}
		else							// backward -> forward
		{
			pwm_forward_backward_dcmotor = 0;
		}
	}
	if (pwm_start_flag == 1)
	{
		if (pwm_forward_backward_dcmotor == 0)		//forward
		{
			if (t1ms_counter >= 50)
			{
				t1ms_counter = 0;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
		}
		else
		{
			if (t1ms_counter >= 100)		//backward
			{
				t1ms_counter = 0;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
		}
	}
}
#endif

// 숙제 10/13 <코드 참고2> dcmotor pwm 제어...

#if 0
void dcmotor_pwm_control(void)
{
	// start/stop : button0
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);  // LD1

		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

		if (!pwm_start_flag)
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 1);
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
			led_all_off();
		}
	}

	// speed-up : button1
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);  // LD2
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_Value += 10;
		CCR_UP_CCR_Value = CCR_UP_Value;
		if (CCR_UP_Value > 100)
		{
			CCR_UP_Value = 100;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
		for (int i=1; i<=(CCR_UP_CCR_Value/10)-2; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		}
	}

	// speed-down : button2
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);  // LD3
		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_DOWN_Value -= 10;
		CCR_DOWN_CCR_Value = CCR_DOWN_Value;
		if (CCR_DOWN_Value < 30)
		{
			CCR_DOWN_Value = 30;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
		HAL_GPIO_WritePin(GPIOD, 0x01 << (CCR_DOWN_CCR_Value/10)-1, 0);
	}

	// 정,역회전 : button3
	if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		if (pwm_forward_backward_dcmotor == 0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			if (t1ms_counter >= 200)
			{
				t1ms_counter = 0;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
			pwm_forward_backward_dcmotor = 1;

		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			if (t1ms_counter >= 50)
			{
				t1ms_counter = 1;
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
			}
			pwm_forward_backward_dcmotor = 0;

		}
	}

	if (pwm_start_flag)  // LED
		{
			if (pwm_forward_backward_dcmotor == 0)
			{
				if (t1ms_counter >= 50)
				{
					t1ms_counter = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
				}
			}
			else
			{
				if (t1ms_counter >= 100)
				{
					t1ms_counter = 0;
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
				}
			}
		}
}

#endif

// 버튼 012 -> start/stop, speed-up, speed-down
#if 0																		// original code
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

