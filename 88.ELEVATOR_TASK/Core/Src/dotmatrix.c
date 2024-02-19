#include "main.h"

int dotmatrix_main_up(void);	// 선언
void init_dotmatrix_up(void);
int dotmatrix_main_down(void);	// 선언
void init_dotmatrix_down(void);

GPIO_TypeDef *col_port[] =
{
	COL1_GPIO_Port,COL2_GPIO_Port,COL3_GPIO_Port,COL4_GPIO_Port,
	COL5_GPIO_Port,COL6_GPIO_Port,COL7_GPIO_Port,COL8_GPIO_Port
};

GPIO_TypeDef *row_port[] =
{
	ROW1_GPIO_Port,ROW2_GPIO_Port,ROW3_GPIO_Port,ROW4_GPIO_Port,
	ROW5_GPIO_Port,ROW6_GPIO_Port,ROW7_GPIO_Port,ROW8_GPIO_Port
};

uint16_t row_pin[] =
{
	ROW1_Pin,ROW2_Pin,ROW3_Pin,ROW4_Pin,
	ROW5_Pin,ROW6_Pin,ROW7_Pin,ROW8_Pin
};

uint16_t col_pin[] =
{
	COL1_Pin,COL2_Pin,COL3_Pin,COL4_Pin,
	COL5_Pin,COL6_Pin,COL7_Pin,COL8_Pin
};

unsigned char all_on[] = {		// all on 문자 정의
# if 0
	0b00000000,
	0b11100111,
	0b11111111,
	0b11100111,
	0b00000000,
	0b01000010,
	0b00111100,
	0b00000000
#else
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
#endif
};


const uint8_t blank[8] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};

#if 1

uint8_t number_data_up[2][10] =
{
		{
			0b00000000,		//>
			0b00001000,
			0b00001100,
			0b01111110,
			0b00001100,
			0b00001000,
		    0b00000000,
			0b00000000
		}
};

uint8_t number_data_down[1][10] =
{
		{
			0b00000000,		//<
			0b00010000,
			0b00110000,
			0b01111110,
			0b00110000,
			0b00010000,
		    0b00000000,
			0b00000000
		}
};

#else
uint8_t number_data[20][10] =
{
	{
		0b01110000,	//0
		0b10001000,
		0b10011000,
		0b10101000,
		0b11001000,
		0b10001000,
		0b01110000,
	    0b00000000
	},
	{
		0b01000000,	//1
		0b11000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b11100000,
	    6   // 점 0b00000110
	},
	{
		0b01110000,	//2
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b01000000,
		0b11111000,
	    6
	},
	{
		0b11111000,	//3
	    0b00010000,
		0b00100000,
		0b00010000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00010000,	//4
		0b00110000,
		0b01010000,
		0b10010000,
		0b11111000,
		0b00010000,
		0b00010000,
	    6
	},
	{
		0b11111000,	//5
		0b10000000,
		0b11110000,
		0b00001000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00110000,	//6
		0b01000000,
		0b10000000,
		0b11110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b11111000,	//7
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b00100000,
		0b00100000,
	    6
	},
	{
		0b01110000,	//8
		0b10001000,
		0b10001000,
		0b01110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b01110000,	//9
		0b10001000,
		0b10001000,
		0b01111000,
		0b00001000,
		0b00010000,
		0b01100000,
	    6
	},
	{
		0b00000000,    // heart
		0b01100110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00011000
	}
};

#endif

unsigned char display_data[8];	// 최종 8X8 출력활 데이터
unsigned char scroll_buffer[50][8] = {0};	// 스크롤할 데이터가 들어있는 버퍼 초기화해줌 (8개가 들어가는 방이 50개 있음)
int number_of_character = 1; // 출력할 문자 갯수

// 초기화 작엄
// 1. display_data에 number_data[0]에 있는 내용 복사
// 2. number_data를 scroll_buffer에 복사 (이중 for문 필요)
// 3. dotmatrix의 led를 off

//UP
void init_dotmatrix_up(void)
{
	// 1
	for (int i=0; i<8; i++)
	{
		display_data[i] = number_data_up[0][i];
	}
	// 2
	for (int i=1; i<number_of_character+1; i++)  // i < 2
	{
		for (int j=0; j<8; j++)	//scroll_buffer[0] = blank
		{
			scroll_buffer[i][j] = number_data_up[i-1][j];
		}
	}
	// 3
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(col_port[i], col_pin[i], 1); 	// led all off
	}
}

//DOWN
void init_dotmatrix_down(void)
{
	// 1
	for (int i=0; i<8; i++)
	{
		display_data[i] = number_data_down[0][i];
	}
	// 2
	for (int i=1; i<number_of_character+1; i++)  // i < 2
	{
		for (int j=0; j<8; j++)	//scroll_buffer[0] = blank
		{
			scroll_buffer[i][j] = number_data_down[i-1][j];
		}
	}
	// 3
	for (int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(col_port[i], col_pin[i], 1); 	// led all off
	}
}




// col 컨트롤 함수
void write_column_data(int col)
{
	// for문 8번 돌려서 해당 col 찾아서
	for (int i=0; i<8; i++)
	{
		if (i == col)
			HAL_GPIO_WritePin(col_port[i], col_pin[i], 0);	// on		// pin state = 0으로 해서 불켜지지 않게?
		else HAL_GPIO_WritePin(col_port[i], col_pin[i], 1);	// off
	}
}

// 0b00111100 이라는 데이터 찍는다고 할 때 로직 어떻게?
void write_row_data(unsigned char data)	// origianl data = data
{
	unsigned char d; 	// 이미지를 copy 하기 위한 임시 버퍼 (한 bit씩 write 하기 위해)

	d = data;
	for (int i=0; i<8; i++)
	{
		//6번째 0을 write 한다면
		if ( d & (1 << i))		// 1인 경우
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 1);
		else
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 0);
	}
}

#if 1

// scroll 문자 출력 program
//UP
int dotmatrix_main_up(void)
{
	static int count = 0;	// column count		지역변수를 전역 변수처럼 쓰려면?
	static int index = 0; 	// scroll_buffer의 2차원 index 값
	static uint32_t past_time = 0; 	// 이전 Tick 값을 저장

	init_dotmatrix_up();

	while(1)
	{
		uint32_t now = HAL_GetTick();	// 1ms 타임값마다 값을 읽어옴
		// 처음 시작시 past_time = 0;	now:500 --> past_time=500
		if (now - past_time >= 200)	// 500ms마다 scroll
		{
			past_time = now;
			for (int i=0; i<8; i++)
			{
				display_data[i] = (scroll_buffer[index][i] >> count)|(scroll_buffer[index+1][i] << (8 - count)); 	// shift
			}
			if (++count == 8)	// 8 column을 다 처리했으면 다음 scroll_buffer로 이동
			{
				count = 0;
				index++;	// 다음 scroll_buffer로 이동
				if (index == number_of_character+1) index = 0;	// 11개의 문자를 다 처리했으면 0번 scroll_buffer를 처리하기 위해 이동
			}
		}
		for (int i=0; i<8; i++)		// 8줄
		{
			// 공통 양글 방식 common anode
			// column에는 0을 row에는 1을 출력해야 해당 LED가 on
			write_column_data(i);
			write_row_data(display_data[i]);	// i를 넘겨준다

			HAL_Delay(1);
		}
	}
	return 0;
}



//DOWN
int dotmatrix_main_down(void)
{
	static int count = 0;	// column count		지역변수를 전역 변수처럼 쓰려면?
	static int index = 0; 	// scroll_buffer의 2차원 index 값
	static uint32_t past_time = 0; 	// 이전 Tick 값을 저장

	init_dotmatrix_down();

	while(1)
	{
		uint32_t now = HAL_GetTick();	// 1ms 타임값마다 값을 읽어옴
		// 처음 시작시 past_time = 0;	now:500 --> past_time=500
		if (now - past_time >= 200)	// 500ms마다 scroll
		{
			past_time = now;
			for (int i=0; i<8; i++)
			{
				display_data[i] = (scroll_buffer[index][i] << count)|(scroll_buffer[index+1][i] >> (8 - count)); 	// shift
			}
			if (++count == 8)	// 8 column을 다 처리했으면 다음 scroll_buffer로 이동
			{
				count = 0;
				index++;	// 다음 scroll_buffer로 이동
				if (index == number_of_character+1) index = 0;	// 11개의 문자를 다 처리했으면 0번 scroll_buffer를 처리하기 위해 이동
			}
		}
		for (int i=0; i<8; i++)		// 8줄
		{
			// 공통 양글 방식 common anode
			// column에는 0을 row에는 1을 출력해야 해당 LED가 on
			write_column_data(i);
			write_row_data(display_data[i]);	// i를 넘겨준다

			HAL_Delay(1);
		}
	}
	return 0;
}



#else

// 고정 문자 출력 demo program
int dotmatrix_main_test(void)
{
	while(1)
	{
		for (int i=0; i<8; i++)		// 8줄
		{
			// 공통 양글 방식 common anode
			// column에는 0을 row에는 1을 출력해야 해당 LED가 on
			write_column_data(i);
			write_row_data(all_on[i]);	// i를 넘겨준다

			HAL_Delay(1);
		}
	}
	return 0;
}
#endif







/*
uint8_t number_data[5][10] =
{
		{
			0b01111000,		//J
			0b00010000,
			0b00010000,
			0b00010000,
			0b00010000,
			0b10010000,
		    0b01100000,
			0b00000000
		},
		{
			0b01111000,		//J
			0b00010000,
			0b00010000,
			0b00010000,
			0b00010000,
			0b10010000,
		    0b01100000,
			0b00000000
		},
		{
			0b10001000,		//H
			0b10001000,
			0b10001000,
			0b11111000,
			0b10001000,
			0b10001000,
			0b10001000,
			0b00000000
		},
		{
			0b00100000,		//^
			0b01110000,
			0b10101000,
			0b00100000,
			0b00100000,
			0b00100000,
			0b00100000,
			0b00000000
		},
		{
			0b01000000,		//1
			0b11000000,
			0b01000000,
			0b01000000,
			0b01000000,
			0b01000000,
			0b11100000
		}
};
*/
