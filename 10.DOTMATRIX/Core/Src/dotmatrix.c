#include "main.h"

int dotmatrix_main_test(void);

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

uint8_t smile[] = {			// 스마일 문자 정의
		0b00111100,
		0b01000010,
		0b10010101,
		0b10100001,
		0b10100001,
		0b10010101,
		0b01000010,
		0b00111100
};


void write_column_data(int col)
{
	for (int i=0; i < 8; i++)
	{
		if (i == col)
			HAL_GPIO_WritePin(col_port[i], col_pin[i], 0);  // on
		else HAL_GPIO_WritePin(col_port[i], col_pin[i], 1);  // off
	}
}
// 0b00111110
void write_row_data(unsigned char data)
{
	unsigned char d;

	d = data;

	for(int i=0; i < 8; i++)
	{
		if ( d & (1 << i))   // 1인경우
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 1);
		else
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 0);
	}
}

int dotmatrix_main_test(void)
{
	while(1)
	{
		for (int i=0; i < 8; i++)
		{
			// 공통 양극 방식
			// column에는 0을 ROW에는 1을 출력해야 해당 LED가 on된다.
			write_column_data(i);
			write_row_data(smile[i]);
			HAL_Delay(1);
		}
	}
	return 0;
}
