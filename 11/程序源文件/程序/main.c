#include "common.h"
#include "DS18B20_One.h"
#include "lcd1602.h"
#include "eeprom52.h"
 
sbit key1 = P1^5;
sbit key2 = P1^4;
sbit key3 = P1^3;

sbit beep = P2^0;

sbit jdq = P3^7;

sbit led1 = P1^0;
sbit led2 = P1^1;

unsigned char temp_hight = 35,temp_low = 10,setn=0;
bit flag = 0;

char check = 111;//����λ

void write_eeprom()//eeprom�洢���ݣ�����ֵ
{
	  SectorErase(0x2000);//���

		byte_write(0x2001, temp_low);
		byte_write(0x2002, temp_hight);
	  byte_write(0x2009, check);
}

void eeprom_init()//eeprom��ȡ����
{
	  check = byte_read(0x2009);
		if(check != 111)//��Ƭ����һ��ʹ��ʱ��checkУ��ֵ�㲻��111����������Ҫ�ѳ�ʼ�������ȴ�һ���ٶ�ȡ
		{
				check = 111;
			  temp_hight = 35;
			  temp_low = 10;
			  write_eeprom();
		}
	  temp_low = byte_read(0x2001);//��ȡ����ֵ
	  temp_hight = byte_read(0x2002);//��ȡ����ֵ
}

void keyscan()
{
 	if(key1 == 0)//�Ӽ�
	{
	   delay_ms(10);//����
	   if(key1 == 0)
	   {
	   	  while(key1 == 0);//�ȴ������ɿ�
			  if(setn == 1)//�����¶�����
				{
						if(temp_hight - temp_low > 1)
						{
							 temp_low++;	
						}	
						lcd_write_char(4,1,temp_low/10+48);
						lcd_write_char(5,1,temp_low%10+48);
						
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
			  }
				if(setn == 2)//�����¶�����
				{
						if(temp_hight < 99)//���99
						{
							 temp_hight++;	
						}	
						lcd_write_char(13,1,temp_hight/10+48);
						lcd_write_char(14,1,temp_hight%10+48);
						
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
			  }
				write_eeprom();//�洢
	   }	
	}
	if(key2 == 0)//����
	{
	   delay_ms(10);
	   if(key2 == 0)
	   {
	   	  while(key2 == 0);
				if(setn == 1)
				{
						if(temp_low > 0)
						{
							 temp_low--;	
						}	
						lcd_write_char(4,1,temp_low/10+48);
						lcd_write_char(5,1,temp_low%10+48);
						
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
			  }
				if(setn == 2)
				{
						if(temp_hight - temp_low > 1)
						{
							 temp_hight--;	
						}	
						lcd_write_char(13,1,temp_hight/10+48);
						lcd_write_char(14,1,temp_hight%10+48);
						
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
			  }
				write_eeprom();
	   }	
	}
	if(key3 == 0)//���ü�
	{
	   delay_ms(10);
	   if(key3 == 0)
	   {
	   	  while(key3 == 0);
			  beep = 1;
        setn++;
			  if(setn > 2)
				{
						setn = 0;
					  lcd_write_com(0x0c);//�رչ��
				}
				if(setn == 1)
				{
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
				}
				if(setn == 2)
				{
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
				}
	   }	
	}
}

void main()
{
    short temperature;//�¶�
    unsigned int count;

	  temperature = DS18B20_TF();
	  delay_ms(1000);
	  eeprom_init();
    lcd_init();//1602Һ����ʼ��
		lcd_write_str(0,0,"Temperature:   C");
		lcd_write_str(0,1,"T-L:00C  T-H:00C");

	  lcd_write_char(4,1,temp_low/10+48);//��ʾʮλ 
    lcd_write_char(5,1,temp_low%10+48);//��ʾ��λ
	
    lcd_write_char(13,1,temp_hight/10+48);
    lcd_write_char(14,1,temp_hight%10+48);
	  delay_ms(1000);
		while(1)
		{
			 keyscan();
			 if(count++ > 200 && setn ==0)
			 {
					count = 0;

					temperature = DS18B20_TF();//��ȡ�¶�
					lcd_write_char(13,0,temperature/10+48);
					lcd_write_char(14,0,temperature%10+48);
           
				  if(temperature <= temp_low){led1=0;jdq=0;}else{led1=1;}
				  if(temperature >= temp_hight){led2=0;jdq=1;}else{led2=1;}
				 
						if(temperature >= temp_hight || temperature <= temp_low)
						{
								beep = ~beep;//����������
							  delay_ms(10);
						}
						else
						{
								beep = 1;//�������ر�
						}
			 }
			 delay_ms(1);
		}
}