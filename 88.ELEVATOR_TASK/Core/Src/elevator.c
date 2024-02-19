#include "main.h"
#include "button.h"
#include "i2c_lcd.h"
#include <stdio.h>

void elevator_main(void);
void elevator_init();
void elevator_run();

extern void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
extern void delay_us(unsigned long us);
extern void stepmotor_drive(int direction);
extern void set_rpm(int rpm);
extern void i2c_lcd_init(void);

//각 기능 module
void button_control();
void stepmotor_elevator();
void i2c_lcd_display();
void fnd_display();
void fnd_current_floor_display();
void display_lcd_fnd();
void cds_led();
void dotmatrix_main();

// 기능 6. dotmatrix -> 화살표 display
extern int dotmatrix_main_up(void);
extern int dotmatrix_main_down(void);
extern void init_dotmatrix_up(void);
extern void init_dotmatrix_down(void);

// 버튼 flag
int btn_flag = 0;

// 기능 1. 버튼 제어 -> 각 버튼 led 연동(on/off check)
int button_1_cnt = 0;
int button_2_cnt = 0;
int button_3_cnt = 0;
int button_4_cnt = 0;
void button_control()
{
		// 버튼 1
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)	// 버튼1 : 1층으로
		{
			btn_flag = 1;

			button_1_cnt++;
			button_1_cnt %= 2;

			if (button_1_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, 1);	// LED0 on
			}
			else if (button_1_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, 0);	// LED0 off
			}
		}

		// 버튼 2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)	// 버튼2 : 2층으로
		{
			btn_flag = 2;

			button_2_cnt++;
			button_2_cnt %= 2;

			if (button_2_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 1);	// LED0 on

			}
			else if (button_2_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 0);	// LED0 off

			}
		}

		// 버튼 3
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)	// 버튼3 : 3층으로
		{
			btn_flag = 3;

			button_3_cnt++;
			button_3_cnt %= 2;

			if (button_3_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 1);	// LED0 on
			}
			else if (button_3_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 0);	// LED0 off
			}
		}

		// 버튼 4
		if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, 4) == BUTTON_PRESS)	// 버튼4 : 4층으로
		{
			btn_flag = 4;

			button_4_cnt++;
			button_4_cnt %= 2;

			if (button_4_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);	// LED0 on
			}
			else if (button_4_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);	// LED0 off
			}
		}
}


// 기능 2. stepmotor -> elevator 동작
int current_floor = 0;
int input_floor = 0;
void stepmotor_elevator()
{
	int direction = 0;
//	int input_floor = 0;

		// 버튼 1
		if (btn_flag == 1)
		{
			if (button_1_cnt == 1)
			{
				input_floor = 1;	// 입력층
				direction = 2;	// 역방향 down
				//if (current_floor == input_floor) direction = 0; // 정지

			}
			else if (button_1_cnt == 0)
			{
				input_floor = 1;	// 입력층
				direction = 0;	// 취소
			}
		}

		// 버튼 2
		if (btn_flag == 2)
		{
			if (button_2_cnt == 1)
			{
				input_floor = 2;	// 입력층
				if (current_floor < 2) direction = 1;	// 정방향 up
				else if (current_floor > 2) direction = 2;	// 역방향 down
				//else if (current_floor == input_floor) direction = 0; // 정지
			}
			else if (button_2_cnt == 0)
			{
				input_floor = 2;	// 입력층
				direction = 0;	// 취소
			}
		}

		// 버튼 3
		if (btn_flag == 3)
		{
			if (button_3_cnt == 1)
			{
				input_floor = 3;	// 입력층
				if (current_floor < 3) direction = 1;	// 정방향 up
				else if (current_floor > 3) direction = 2;	// 역방향 down
				//else if (current_floor == input_floor) direction = 0; // 정지
			}
			else if (button_3_cnt == 0)
			{
				input_floor = 3;	// 입력층
				direction = 0;	// 취소
			}
		}

		// 버튼 4
		if (btn_flag == 4)
		{
			if (button_4_cnt == 1)
			{
				input_floor = 4;	// 입력층
				direction = 1;	// 정방향 up
				//if (current_floor == input_floor) direction = 0; // 정지
			}
			else if (button_4_cnt == 0)
			{
				input_floor = 4;	// 입력층
				direction = 0;	// 취소
			}
		}

	// 포토센서 -> 현재층과 입력층 같으면 정지
	if (current_floor == input_floor) direction = 0; // 정지

	stepmotor_drive(direction);
	set_rpm(13);
}


// 기능 3. i2c_lcd -> 동작상태 display
void i2c_lcd_display()
{
	char lcd_buff[40];

		//버튼 1
	if (btn_flag == 1)
	{
		if (button_1_cnt == 1)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "   Move to 1F");
			lcd_string(lcd_buff);
		}
		else if (button_1_cnt == 0)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "     Cancel  ");
			lcd_string(lcd_buff);
		}
	}


		//버튼 2
	if (btn_flag == 2)
	{
		if (button_2_cnt == 1)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "   Move to 2F");
			lcd_string(lcd_buff);
		}
		else if (button_2_cnt == 0)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "     Cancel  ");
			lcd_string(lcd_buff);
		}
	}


		//버튼 3
	if (btn_flag == 3)
	{
		if (button_3_cnt == 1)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "   Move to 3F");
			lcd_string(lcd_buff);
		}
		else if (button_3_cnt == 0)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "     Cancel  ");
			lcd_string(lcd_buff);
		}
	}


		//버튼 4
	if (btn_flag == 4)
	{
		if (button_4_cnt == 1)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "   Move to 4F");
			lcd_string(lcd_buff);
		}
		else if (button_4_cnt == 0)
		{
			move_cursor(1,0);
			sprintf(lcd_buff, "     Cancel  ");
			lcd_string(lcd_buff);
		}
	}
}


// 기능 4. FND -> 층수 display
void fnd_display()
{
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);

		// 버튼 1
		if (btn_flag == 1)
		{
			if (button_1_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_6, GPIO_PIN_RESET);	//on
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);	//off
			}
			else if (button_1_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}


		// 버튼 2
		if (btn_flag == 2)
		{
			if (button_2_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_2_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}


		// 버튼 3
		if (btn_flag == 3)
		{
			if (button_3_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_3_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}


		// 버튼 4
		if (btn_flag == 4)
		{
			if (button_4_cnt == 1)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_4_cnt == 0)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}
}


// 기능 4-2. FND "current floor" display
void fnd_current_floor_display()
{

	if (current_floor == 1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_6, GPIO_PIN_RESET);	//on
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);	//off
	}
	else if (current_floor == 2)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_11, GPIO_PIN_SET);
	}
	else if (current_floor == 3)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
	}
	else if (current_floor == 4)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
	}

}


// 기능 3+4. FND + LCD display
void display_lcd_fnd()
{
	char lcd_buff[40];

		// 버튼 1
		if (btn_flag == 1)
		{
			if (button_1_cnt == 1)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 1F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_6, GPIO_PIN_RESET);	//on
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);	//off
			}
			else if (button_1_cnt == 0)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}

		// 버튼 2
		if (btn_flag == 2)
		{
			if (button_2_cnt == 1)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 2F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_2_cnt == 0)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}


		// 버튼 3
		if (btn_flag == 3)
		{
			if (button_3_cnt == 1)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 3F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_3_cnt == 0)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}

		// 버튼 4
		if (btn_flag == 4)
		{
			if (button_4_cnt == 1)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 4F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_4_cnt == 0)
			{
				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}
}



// 기능 5. 조도센서 ->  LED 제어
extern uint16_t adcValue[4];
void cds_led()
{
		printf("cds sensor : %d\n", adcValue[0]);
		if (adcValue[0] < 480 )
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 1);	// LED0 on
		}
		else
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 0);	// LED0 off
		}
		HAL_Delay(50); 	//50ms
}

void dotmatrix_main()
{
	if (btn_flag != 0)
	{
		if (current_floor < input_floor)
		{
			dotmatrix_main_up();
		}
		else if (current_floor > input_floor)
		{
			dotmatrix_main_down();
		}
		else if (current_floor == input_floor)
		{
			init_dotmatrix_up();
			init_dotmatrix_down();
		}
	}
}



// elevator 초기화 함수
void elevator_init()
{
	i2c_lcd_init();
	init_dotmatrix_up();
	init_dotmatrix_down();

	// elevator 내부 조명
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 0);	// LED0 off

	//초기화
	char lcd_buff[40];
	move_cursor(0,0);
	sprintf(lcd_buff, "=== ELEVATOR ===");
	lcd_string(lcd_buff);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
}



// elevator 동작 함수 -> 모듈통합 : 초기화 + 동작
void elevator_run()
{
	while(1)
	{
		button_control();
		stepmotor_elevator();
		i2c_lcd_display();
		fnd_display();
//		fnd_current_floor_display();
//		display_lcd_fnd();
		cds_led();
		dotmatrix_main();
	}
}


#if 1
void elevator_main(void)
{
	//초기화
	elevator_init();

	//동작
	void elevator_run();
}



#else	// module 진행 전 코드
// elevator 메인 함수 -> 모듈화 필요
//int current_floor = 0;
void elevator_main(void)
{
	int direction = 0;
	int input_floor = 0;
	char lcd_buff[40];

//	i2c_lcd_init();
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 1);	// LED0 on

	move_cursor(0,0);
	sprintf(lcd_buff, "=== ELEVATOR ===");
	lcd_string(lcd_buff);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);

	while (1)
	{
		// 버튼 1
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)	// 버튼1 : 1층으로
		{
			button_1_cnt++;
			button_1_cnt %= 2;

			input_floor = 1;	// 입력층

			if (button_1_cnt == 1)
			{
				direction = 2;	// 역방향 down

				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, 1);	// LED0 on

				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 1F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_6, GPIO_PIN_RESET);	//on
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);	//off
			}
			else if (button_1_cnt == 0)
			{
				direction = 0;	// 취소

				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_9, 0);	// LED0 off

				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}

		// 버튼 2
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)	// 버튼2 : 2층으로
		{
			button_2_cnt++;
			button_2_cnt %= 2;

			input_floor = 2;	// 입력층

			if (button_2_cnt == 1)
			{
				if (current_floor < 2) direction = 1;	// 정방향 up
				else if (current_floor > 2) direction = 2;	// 역방향 down

				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 1);	// LED0 on

				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 2F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_2_cnt == 0)
			{
				direction = 0;	// 취소

				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, 0);	// LED0 off

				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}

		}


		// 버튼 3
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS) 	// 버튼3 : 3층으로
		{
			button_3_cnt++;
			button_3_cnt %= 2;

			input_floor = 3;

			if (button_3_cnt == 1)
			{
				if (current_floor < 3) direction = 1;	// 정방향 up
				else if (current_floor > 3) direction = 2;	// 역방향 down

				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 1);	// LED0 on

				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 3F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_3_cnt == 0)
			{
				direction = 0;	// 취소

				HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, 0);	// LED0 off

				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}

		// 버튼 4
		if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, 4) == BUTTON_PRESS)	// 버튼4 : 4층으로
		{
			button_4_cnt++;
			button_4_cnt %= 2;

			input_floor = 4;

			if (button_4_cnt == 1)
			{
				direction = 1;	// 정방향 up

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);	// LED0 on

				move_cursor(1,0);
				sprintf(lcd_buff, "   Move to 4F");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11, GPIO_PIN_SET);
			}
			else if (button_4_cnt == 0)
			{
				direction = 0;	// 취소

				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);	// LED0 on

				move_cursor(1,0);
				sprintf(lcd_buff, "     Cancel  ");
				lcd_string(lcd_buff);

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
			}
		}

		// 포토센서 -> 현재층과 입력층 같으면 정지
		if (current_floor == input_floor) direction = 0; // 정지

		stepmotor_drive(direction);
		set_rpm(13);

	}
}
#endif
