#include "main.h"
#include "DHT11.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void delay_us (unsigned long us);							// 마이크로세컨드 딜레이 함수 필요 main.c -> extern 선언
extern volatile int TDHT11_timer_counter;
extern volatile int TIM10_10ms_counter; 							// variation from main.c
extern uint8_t lcd_display_mode_flag;					// from button.c 7

extern void lcd_string(uint8_t *str);			//from i2c_lcd.h
extern void move_cursor(uint8_t row, uint8_t column);	//from i2c_lcd.h

void DHT11_main(void);												// 선언 -> main.c 에도 extern 으로 선언해줌 78
void DHT11_Init(void);												// 선언 -> main.c 에도 extern 으로 선언해줌 79
void DHT11_processing(void);										// 선언 -> main.c 에도 extern 으로 선언해줌 80

void DHT11_main(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;

	// HAL_TIM_Base_Start_IT(&htim2);
	DHT11_Init();
printf("DHT11_Init()\n");											// test code

	while(1)
	{
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();
		
		i_RH = DHT11_rx_Data();										// 습도 정수
		d_RH = DHT11_rx_Data();										// 습도 소수
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();
		
		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);

//		FND_Update(i_Tmp*100 + i_RH);
		HAL_Delay(1500);
	}
	
}
// mode select 모드1 : LCD에 온도 습도 정보 출력
#if 1
// dht11time 150 300 200 -> comportmaster
int dht11time = 150;
void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	char lcd_buff[20];						//

	if (TIM10_10ms_counter >= dht11time)			// 1500ms
	{
		TIM10_10ms_counter = 0;
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();

		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();

		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);
		sprintf(lcd_buff, "Tmp:%d Wet:%d", (int)i_Tmp, (int)i_RH);
		if (lcd_display_mode_flag == 1)							// mode select 모드1
		{
			move_cursor(0,0);									// 커서 0line 0col 이동
			lcd_string(lcd_buff); 								// HEX 0x20 미만은 LCD 입력 안됨 (\n 안됨)
		}
	}
}
#endif

#if 0												// original code2
// dht11time 150 300 200 -> comportmaster
int dht11time = 150;
void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	char lcd_buff[20];						//

	if (TIM10_10ms_counter >= dht11time)			// 1500ms
	{
		TIM10_10ms_counter = 0;
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();

		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();

		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);
		sprintf(lcd_buff, "Tmp:%d Wet:%d", (int)i_Tmp, (int)i_RH);
		move_cursor(0,0);					// 커서 0line 0col 이동
		lcd_string(lcd_buff); 				// HEX 0x20 미만은 LCD 입력 안됨 (\n 안됨)
	}
}
#endif												// original code2

#if 0
// 10/12 숙제 ㅁ
int dht11time = 150;
volatile int dht11_on_off_flag;
void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;

	if (dht11_on_off_flag)
	{
		if (TIM10_10ms_counter >= dht11time)			// 1500ms
		{
			TIM10_10ms_counter = 0;
			DHT11_trriger();
			DHT11_DataLine_Input();
			DHT11_dumi_read();

			i_RH = DHT11_rx_Data();
			d_RH = DHT11_rx_Data();
			i_Tmp = DHT11_rx_Data();
			d_Tmp = DHT11_rx_Data();

			DHT11_DataLine_Output();
			HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

			printf("[Tmp]%d\n",(int)i_Tmp);
			printf("[Wet]%d\n",(int)i_RH);
			sprintf(lcd_buff, "[Tmp]%d [Wet]%d", int)i_Tmp, (int)i_RH);
			move_cursor(0,0);			// 커서 0line 0col 이동
			lcd_string(lcd_buff); // HEX 0x20 미만은 LCD 입력 안됨 (\n 안됨)
		}
	}
}
#endif

#if 0											// original code
void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;

	DHT11_trriger();
	DHT11_DataLine_Input();
	DHT11_dumi_read();
	// hand shake end

	i_RH = DHT11_rx_Data();
	d_RH = DHT11_rx_Data();
	i_Tmp = DHT11_rx_Data();
	d_Tmp = DHT11_rx_Data();

	DHT11_DataLine_Output();
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

	// int type 으로 형변환
	printf("[Tmp]%d\n",(int)i_Tmp);
	printf("[Wet]%d\n",(int)i_RH);
//	FND_update(i_Tmp*100 + i_RH);
}
#endif											// original code

void DHT11_Init(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	HAL_Delay(3000);
	return;
}


void DHT11_trriger(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_RESET);
	HAL_Delay(20);
	
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	delay_us(7);
	return;
}

// 데이터 시트보고 오실로스코프 코딩할 줄 알아야
void DHT11_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			//Change Output to Input
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}


void DHT11_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output 
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}


uint8_t DHT11_rx_Data(void)
{
	uint8_t rx_data = 0;							// 8bit
	
	for(int i = 0; i < 8; i++)						// 8bit read -> oscilloscope 8 pulse
	{
		//when Input Data == 0
		while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );		// low일 때 대기, 0일 동안 읽는다 -> high 되면 빠져나옴
#if 1
		delay_us(40);													// high 뛰었을 때 0(28ms), 1(70ms) 구분을 위해 40ms 넣어줌
#else  // org
		delay_us(16);
#endif
		rx_data<<=1;													// data 1bit shift
		
		//when Input Data == 1
		if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN))
		{
			rx_data |= 1;												// 1
		}
		while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	}
	return rx_data;
}


void DHT11_dumi_read(void)											// hand shaking
{
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	return;
}
