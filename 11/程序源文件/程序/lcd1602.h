#ifndef __LCD1602_HH_
#define __LCD1602_HH_


void lcd_clear(void);
void lcd_write_com(unsigned char byte);
void lcd_write_data(unsigned char byte);
void lcd_init();
void lcd_write_char(unsigned char x,unsigned char y,unsigned char byte);
void lcd_write_str(unsigned char x, unsigned char y, unsigned char *s);

#endif