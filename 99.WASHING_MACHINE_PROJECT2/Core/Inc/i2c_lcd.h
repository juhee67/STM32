/*
 * i2c_lcd.h
 *
 *  Created on: 2019. 9. 4.
 *      Author: k
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#define I2C_LCD_ADDRESS (0x27<<1)
#define BACKLIGHT_ON 0x08

/* LCD command   */
#define DISPLAY_ON 0x0C
#define DISPLAY_OFF 0x08
#define CLEAR_DISPLAY 0x01  //Delay 2msec
#define RETURN_HOME 0x02

void lcd_command(uint8_t command);
void lcd_data(uint8_t data);
void i2c_lcd_init(void);								// main.c 91 에 extern 선언
void lcd_string(uint8_t *str);							// main.c 92 에 extern 선언
void move_cursor(uint8_t row, uint8_t column);			// main.c 93 에 extern 선언

#endif /* INC_I2C_LCD_H_ */
