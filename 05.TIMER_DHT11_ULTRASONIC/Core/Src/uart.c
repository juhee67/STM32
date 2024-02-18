#include "uart.h"
#include <string.h>										 	// strncmp, strcpy
#include <stdlib.h>
void pc_command_processing(void);							// 선언
void bt_command_processing(void);							// 선언

// from led.c 함수 -> extern 선언해줌
extern void led_main(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);
extern void led_flower_on(void);
extern void led_flower_off(void);

extern int dht11time;
extern volatile int dht11_on_off_flag;						// aa
extern volatile int ultrasonic_on_off_flag;					// aa

// from main.c -> extern 선언해줌
extern uint8_t rx_data;										// UART3 rx data
extern uint8_t bt_rx_data;									// UART6 bt rx data
extern UART_HandleTypeDef huart3;							// PC
extern UART_HandleTypeDef huart6;							// BT

// H/W와 S/W가 만나는 약속 장소 : call back function
// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
// UART로 1 byte가 수신되면  H/W가 function을 call을 해준다.
// UART RX INT가 발생이 되면 (call back function 으로) 이곳으로 자동적으로 들어온다.
// 숙제 1. 1차원 array 가지고는 overflow가 발생되므로 2차원 array circular queue를 구성하여 overflow가 발생되지 않도록 구현해야 한다.
// ComPort Master 에서
// ledallon\n
// ledalloff\n
// led_toggle\n

#if 0
#define COMMAND_MAX 20
#define COMMAND_LENGTH 40													// 40 byte
volatile unsigned char rx_buff[COMMAND_MAX][COMMAND_LENGTH];	// UART3으로부터 수신된 char 을 (\n 만날 때까지) 저장하는 공간
//volatile int rx_index = 0;									// rx_buff의 save 위치 변수 선언
volatile int rx_index1 = 0;
volatile int rx_index2 = 0;
volatile int newline_detect_flag = 0;							// new line 을 만났을 때의 indicator 예) ledallon\n

#else																												// original
#define COMMAND_LENGTH 40													// 40 byte
volatile unsigned char rx_buff[COMMAND_LENGTH];					// UART3으로부터 수신된 char 을 (\n 만날 때까지) 저장하는 공간
volatile int rx_index = 0;										// rx_buff의 save 위치 변수 선언
volatile int newline_detect_flag = 0;							// new line 을 만났을 때의 indicator 예) ledallon\n
#endif																												// original

volatile unsigned char bt_rx_buff[COMMAND_LENGTH];				// UART6으로부터 수신된 char 을 (\n 만날 때까지) 저장하는 공간
volatile int bt_rx_index = 0;									// bt_rx_buff의 save 위치 변수 선언
volatile int bt_newline_detect_flag = 0;						// new line 을 만났을 때의 indicator 예) ledallon\n

#if 0
// 숙제 ㅁㅁㅁㅁ
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// PC
	if (huart == &huart3)									// comport master 와 연결된 uart
	{
		unsigned char data;
		data = rx_data;

		if (out_rx_index2 % COMMAND_MAX == (rx_index2+1) % COMMAND_MAX)
		{
			printf("Queue Full !!!\n");
		}
		else
		{
			if (data == '\n' || data == '\r')				// \n 만나면
			{
				rx_buff[rx_index2][rx_index1] = '\0'; 		// '\0' 을 넣음
				rx_index1 = 0;
				rx_index2++;
				rx_index2 %= COMMAND_MAX;
			}
			else
			{
				rx_buff[rx_index2][rx_index1++] = data;
			}
		}
		// 주의 : 반드시 HAL_UART_Receive_IT 를 call 해줘야 다음 INT 가 발생된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}
}

#else																					// original
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// PC
	if (huart == &huart3)									// comport master 와 연결된 uart
	{
		if (rx_index < COMMAND_LENGTH)						// 현재까지 들어온 byte 가 40 byte 를 넘지 않으면 save
		{
			if (rx_data == '\n' || rx_data == '\r')
			{
				rx_buff[rx_index] = 0; 						// '\0' 을 넣음
				newline_detect_flag = 1;					//  new line 을 만났다는 flag 를 set 한다.
				rx_index = 0; 								//	다음 message 저장을 위해서 rx_index 값을 0으로 한다.
			}
			else
			{
				rx_buff[rx_index++] = rx_data;				// rx_data 를 save
			}
		}
		else		// 40 byte 를 넘으면
		{
			rx_index = 0;
			printf("Message Overflow !!!\n");
		}
		// 주의 : 반드시 HAL_UART_Receive_IT 를 call 해줘야 다음 INT 가 발생된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}

	// BT
	if (huart == &huart6)									// BT 와 연결된 uart
		{
			if (bt_rx_index < COMMAND_LENGTH)				// 현재까지 들어온 byte 가 40 byte 를 넘지 않으면 save
			{
				if (bt_rx_data == '\n' || bt_rx_data == '\r')
				{
					bt_rx_buff[bt_rx_index] = 0; 			// '\0' 을 넣음
					bt_newline_detect_flag = 1;				//  new line 을 만났다는 flag 를 set 한다.
					bt_rx_index = 0; 						//	다음 message 저장을 위해서 rx_index 값을 0으로 한다.
				}
				else
				{
					bt_rx_buff[bt_rx_index++] = bt_rx_data;	// rx_data 를 save
				}
			}
			else		// 40 byte 를 넘으면
			{
				bt_rx_index = 0;
				printf("BT Message Overflow !!!\n");
			}
			// 주의 : 반드시 HAL_UART_Receive_IT 를 call 해줘야 다음 INT 가 발생된다.
			HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);
		}
}
#endif																					// original


#if 0
// PC
void pc_command_processing(void)										// 정의 (newline 만날 때 까지 계속 돌아감)
{
	if (newline_detect_flag)											// comport master 로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);

		if (!strncmp(rx_buff, "led_all_on", strlen("led_all_on")))		// if ( strncmp(rx_buff, "led_all_on", strlen("led_all_on") == 0 )
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 1);
			led_all_on();
			return;														// 불러준 곳으로 다시 리턴
		}
		if (!strncmp(rx_buff, "led_all_off", strlen("led_all_off")))
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			led_all_off();
			return;
		}
		if (!strncmp(rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}

	}
}
#endif

#if 1																					// original
// PC
void pc_command_processing(void)										// 정의 (newline 만날 때 까지 계속 돌아감)
{
	if (newline_detect_flag)											// comport master 로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);

		if (!strncmp(rx_buff, "led_all_on", strlen("led_all_on")))		// if ( strncmp(rx_buff, "led_all_on", strlen("led_all_on") == 0 )
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 1);
			led_all_on();
			return;														// 불러준 곳으로 다시 리턴
		}
		if (!strncmp(rx_buff, "led_all_off", strlen("led_all_off")))
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			led_all_off();
			return;
		}
		if (!strncmp(rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}
		// dht11time150 DHT11
		if (!strncmp(rx_buff, "dht11time", strlen("dht11time")))
		{
			dht11time = atoi(rx_buff + 9);								// ascii -> int
			return;
		}
/*
// 10/12 숙제 comportmaster -> dht11_on/off, ultra_on/off
		if (!strncmp(rx_buff, "dht11_on", strlen("dht11_on")))
		{
			dht11_on_off_flag = 1;
			return;
		}
		if (!strncmp(rx_buff, "dht11_off", strlen("dht11_off")))
		{
			dht11_on_off_flag = 0;
			return;
		}
		if (!strncmp(rx_buff, "ultra_on", strlen("ultra_on")))
		{
			ultrasonic_on_off_flag = 1;
			return;
		}
		if (!strncmp(rx_buff, "ultra_off", strlen("ultra_off")))
		{
			ultrasonic_on_off_flag = 0;
			return;
		}
*/
	}
}

#endif

// BT
void bt_command_processing(void)										// 정의 (newline 만날 때 까지 계속 돌아감)
{
	if (bt_newline_detect_flag)											// comport master 로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);

		if (!strncmp(bt_rx_buff, "led_all_on", strlen("led_all_on")))		// if ( strncmp(rx_buff, "led_all_on", strlen("led_all_on") == 0 )
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 1);
			led_all_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_all_off", strlen("led_all_off")))
		{
//			HAL_GPIO_WritePin(GPIOD, 0xff, 0);
			led_all_off();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}
	}
}
																					// original


//========================================================
// comport master 에서 led_all_on / led_all_off 기능 확인
//========================================================
/*
#include "uart.h"
#include <string.h>
void pc_command_processing(void);

extern uint8_t rx_data;
extern UART_HandleTypeDef huart3;
//when we do not make circular queue, there will be a problem.
//HW와 SW의 만나는 약속 장소 : call back function.
//move from HAL_UART_RxCpltCallback of stm324xx_hal_uart to here
//UART로 1 byte가 수신되면 H/W가 call 해준다.
//UART RX INT가 발생 되면 이곳으로 자동적으로 들어온다.
//comport mastart
//ledallon\n
//ledalloff\n
//led_toggle\n
#define COMMAND_LENGTH 40
volatile unsigned char rx_buff[COMMAND_LENGTH]; //UART3 으로부터 수신된 char를 저장하는 공간. (\n을 만날 때까지)
volatile int rx_index = 0; // rx_buff의 save 위치
volatile int newline_detect_flag = 0; // new line을 만났을 때의 indicator 예) ledallon\n
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   if(huart == &huart3)   //comport master와 연결된 uart
   {
      if(rx_index < COMMAND_LENGTH)   //현재까지 들어온 byte가 20byte를 넘지 않으면
      {
         if(rx_data=='\n' || rx_data == '\r')
         {
            rx_buff[rx_index] = 0;       // '\0'
            newline_detect_flag = 1;   // newline 을 만났다는 flag를 set한다.
            rx_index = 0;            //다음 message 저장을 위해 rx_index값을 0으로 한다.
         }
         else
         {
            rx_buff[rx_index++]=rx_data;
         }
      }
      else
      {
         rx_index=0;
         printf("Message Overflow !!!!\n");
      }
      //주의 : 반드시 HAL_UART_Receive_IT를 call 해줘야 다음 INT가 발생한다.
      HAL_UART_Receive_IT(&huart3, &rx_data, 1);
   }
}


void pc_command_processing(void)
{
   if(newline_detect_flag)      //comport master로부터 완전한 문장이 들어 오면 (\n을 만나면)
   {
      newline_detect_flag = 0;
      printf("%s\n", rx_buff);
      if(!strncmp(rx_buff, "ledallon",strlen("ledallon")))   //if(strncmp(rx_buff, "ledallon",strlen("ledallon")) == NULL)
      {
         HAL_GPIO_WritePin(GPIOD, 0xff, 1);
      }
      if(!strncmp(rx_buff, "ledalloff",strlen("ledalloff")))   //if(strncmp(rx_buff, "ledalloff",strlen("ledalloff")) == NULL)
      {
         HAL_GPIO_WritePin(GPIOD, 0xff, 0);
      }
   }
}
*/
