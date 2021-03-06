/*
 ******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2020-11-04

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 ******************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "delay.h"
#include "board.h"
#include "segment.h"
#include "keypad.h"
#include "LCD.h"
#include "ADC.h"
#include "timer.h"
#include "ultrasonic.h"
#include "Servo.h"
#include "uart.h"
#include "spi.h"
#include "EEPROM.h"
#include "flash.h"
#include "systick.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void)
{
	uint8_t key = 0;
	uint16_t adcValue = 0;

	uint8_t eepromData = 0;
	uint8_t data = 0;
	uint32_t timeout = 0;

	/* Clock -> Internal 16 MHz */
	RCC_DeInit();				/* Adapt PLL to the internal 16 MHz RC oscillator */
	SystemCoreClockUpdate();	/* Update SystemCoreClock */

	SysTick_Init(SystemCoreClock/1000);

	Uart_Init(USART1,115200);
	Uart_Init(USART2,115200);

	Uart_SendString(USART2,"OTA Loader started ...\n");
	/* Test Command */
	if(WIFI_SendCmd("AT\r\n","OK",1000)){
		Uart_SendString(USART2,"WIFI is Up\n");
	}else{
		Uart_SendString(USART2,"WIFI is Down\n");
	}
	delayMs(100);
	/* Echo Command */
	if(WIFI_SendCmd("ATE0\r\n","OK",1000)){
		Uart_SendString(USART2,"Echo Disabled\n");
	}else{
		Uart_SendString(USART2,"Echo not Disabled\n");
	}
	delayMs(100);
	/* Station Mode Command */
	if(WIFI_SendCmd("AT+CWMODE=1\r\n","OK",1000)){
		Uart_SendString(USART2,"Station Mode Done\n");
	}else{
		Uart_SendString(USART2,"Station Mode ERROR\n");
	}
	delayMs(100);
	/* WI-FI Network Connection */
	if(WIFI_SendCmd("AT+CWJAP=\"wagnat_plus\",\"wagnat111\"\r\n","OK",10000)){
		Uart_SendString(USART2,"Network Connected\n");
	}else{
		Uart_SendString(USART2,"Network ERROR\n");
	}
	delayMs(100);
	/* TCP Connection */
	if(WIFI_SendCmd("AT+CIPSTART=\"TCP\",\"192.168.1.10\",3000\r\n","OK",10000)){
		Uart_SendString(USART2,"TCP Connected\n");
	}else{
		Uart_SendString(USART2,"TCP ERROR\n");
	}
	delayMs(100);
	/* Send Command for HTTP*/
	if(WIFI_SendCmd("AT+CIPSEND=35\r\n","OK",1000)){
		//FLASH_MainSectorInit();
		delayMs(10);
		Uart_SendString(USART1,"GET /download/firm.bin HTTP/1.1\r\n\r\n");
		/* We are receiving a file */
		FLASH_Unlock();

		uint8_t lines = 0;
		while(1){
			if(Uart_ReceiveByte_Unblock(USART1,&data)){
				if(data == '\n'){
					lines++;
					if(lines == 12){
						break;
					}
				}
			}
		}
		/*while(1){
			if(Uart_ReceiveByte_Unblock(USART1,&data)){
				Uart_SendByte(USART2,data);
			}
		}*/
		/* File starts */
		uint32_t bytes = 0;
		while(1){
			if(Uart_ReceiveByte_Unblock(USART1,&data)){
				FLASH_ProgramByte((0x08060000 + bytes), data);
				bytes++;
				if(bytes == 1276){
					Uart_SendByte(USART2,data);
					Uart_SendString(USART2,"Binary received\n");
					break;
				}
			}
		}
		//void (*pf)(void);
		//typedef void (*PFUN)(void);
		//volatile uint32_t* fAddr = (*((volatile uint32_t*)0x08060004));
		//pf = (PFUN)fAddr;
		asm("const1: .word 0x08060004");

		__set_MSP((*((volatile uint32_t*)0x08060000)));
		//pf();
			asm(" ldr r0, =const1 ");
			asm(" ldr pc, [r0] ");

		//asm("bx 0x");

	}else{
		Uart_SendString(USART2,"Send ERROR\n");
	}


	while(1);



}
