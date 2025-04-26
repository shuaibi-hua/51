
#ifndef __COMMON_H_
#define __COMMON_H_

#include <reg52.h>
#include <intrins.h>
#include<stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MAIN_Fosc		12000000L	//定义主时钟
#define MAIN_Fosc		11059200L	//定义主时钟

#define uchar unsigned char 
#define ushort unsigned short 
#define uint unsigned int 


void delay_ms(uchar ms);
void delay_us(uint n_us);
//void itoa(long value,char* str, int base);

#endif 

