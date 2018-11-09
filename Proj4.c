#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <plib.h>
#include <xc.h>
//Include all needed libraries
#include "adc.h"
#include "config.h"
#include "lcd.h"   
//#include "led.h"  
#include "rgbled.h"
#include "utils.h"
#include "uart.h"
#include "btn.h"
#include "ssd.h"
#include "swt.h"
#include "pmods.h"
#include "ultr.h"

#pragma config JTAGEN = OFF     // Turn off JTAG - required to use Pin RA0 as IO
#pragma config FNOSC = PRIPLL   //configure system clock 80 MHz
#pragma config FSOSCEN = OFF    // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT     // Primary Oscillator Configuration (XT osc mode)
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_2   //configure peripheral bus clock to 40 MHz

#define SYS_FREQ        (80000000L)     // 80MHz system clock (PB clock = 40MHz)
#define INT_SEC         10              // interrupt at 10th of a second
#define CORE_TICK_RATE  (SYS_FREQ/2/INT_SEC) //Using CoreTimer for 10th of a sec
#define Min_ms          20             //minimum update/shift time in ms
#define Max_ms          2000            //maximum update/shift time in ms

#define _SUPPRESS_PLIB_WARNING
_SUPPRESS_PLIB_WARNING

int angle;
int dutyCycle;
int stepSize = 2;
char displayText[80];
int btnlck = 0;
int turnClockwise = 0;

int clockwise = 1600;
int counterCwise = 1400;

int ultDist;
float distance;
char msg[80];

void findAngle(){
    dutyCycle = SRV_GetPulse(1,1);
    angle=(int)(((dutyCycle-29) * 360)/(971-29+1));
}

void findAngle();
//void delay_ms(int ms);
void update_SSD(int value);
void readUltr();

int main(void){
    BTN_Init();
    SWT_Init();
    SRV_Init();
    LCD_Init();
    RGBLED_Init();
    ULTR_Init(0,1,0,2);
    SSD_Init();  
    
    SRV_SetPulseMicroseconds1(1500);
    while(angle != 90){
            findAngle();
            SRV_SetPulseMicroseconds1(1700);
        }
    while(1){
        sprintf(displayText,"Tm:%2d,Step:%2ddeg", 1, stepSize);
        LCD_WriteStringAtPos(displayText, 0, 0);
        if(SWT_GetValue(7)){
            findAngle();
            if(!turnClockwise){
                if(angle % stepSize < 1){
                    SRV_SetPulseMicroseconds1(1500);
                    //start
                    readUltr();
                    //end
                    SRV_SetPulseMicroseconds1(clockwise);
                    delay_ms(100);
                    findAngle();
                }
                SRV_SetPulseMicroseconds1(clockwise);
            }
            else{
                if(angle % stepSize < 1){
                    SRV_SetPulseMicroseconds1(1500);
                    //start
                    readUltr();
                    //end
                    SRV_SetPulseMicroseconds1(counterCwise);
                    findAngle();
                }
                 SRV_SetPulseMicroseconds1(counterCwise);
            }
            if(angle < 95 && angle > 88){
                SRV_SetPulseMicroseconds1(clockwise);
                turnClockwise = 0;
            }
            if(angle < 275 && angle > 268){
                SRV_SetPulseMicroseconds1(counterCwise);
                turnClockwise = 1;
            }
        }
        else {
            SRV_SetPulseMicroseconds1(1500);
        }
        
        if(BTN_GetValue('U') && !btnlck){
            btnlck = 1;
            if(stepSize < 20){
                if(stepSize == 2){
                    stepSize = 5;
                }
                else {
                    stepSize *=2;
                }
            }
        }
        
        if(BTN_GetValue('C') && !btnlck){
            btnlck = 1;
            if(stepSize > 2){
                if(stepSize == 5){
                    stepSize = 2;
                }
                else {
                    stepSize /=2;
                }
            }
        }
        
        if(!BTN_GetValue('U') && !BTN_GetValue('C')){
            btnlck = 0;
        }
    }
}

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 300; i++) {
        } //software delay ~1 millisec 
    }
}

void update_SSD(int value) {
    int hunds, tens, ones, tenths;
    int dec1, dec2;
    char SSD1 = 0b0000000; //SSD setting for 1st SSD (LSD)
    char SSD2 = 0b0000000; //SSD setting for 2nd SSD
    char SSD3 = 0b0000000; //SSD setting for 3rd SSD
    char SSD4 = 0b0000000; //SSD setting for 4th SSD (MSD)
    if (value < 0) {
        SSD4 = 17;
        value = -1 * value;
        dec1 = 0;
        dec2 = 1;
    } else {
        dec1 = 1;
        dec2 = 0;
        hunds = floor(value / 1000);
        if (hunds > 0)
            SSD4 = hunds; //SSD4 = display_char[thous];
        else
            SSD4 = 0;
    }
    tens = floor((value % 1000) / 100);
    if (hunds == 0 && tens == 0)
        SSD3 = 0;
    else
        SSD3 = tens;
    SSD2 = ones = floor(value % 100 / 10);
    SSD1 = tenths = floor(value % 10);
    SSD_WriteDigits(SSD1, SSD2, SSD3, SSD4, 0, 0, dec2, dec1);
}


void readUltr(){
	ultDist = ULTR_MeasureDist();
	update_SSD(ultDist);
	if(SWT_GetValue(0)){
		distance = (ultDist*13503.9)*0.0000005;
		if(distance>=0 && distance<2)
			RGBLED_SetValue(255,0,0);
		else if(distance>=2 && distance<4)
			RGBLED_SetValue(255,255,0);
		else if(distance>=4 && distance<20)
			RGBLED_SetValue(0,255,0);
		else
			RGBLED_SetValue(0,0,255);
		
		if(distance<0)
			sprintf(msg,"Range: %.2f in\r\n",0);
		else
			sprintf(msg,"Range: %.2f in\r\n", distance);
	}
	else{
		distance=(ultDist*34300)*0.0000005;
		if(distance>=0 && distance<5)
			RGBLED_SetValue(255,0,0);
		else if(distance>=5 && distance<10)
			RGBLED_SetValue(255,255,0);
		else if(distance>=10 && distance<50)
			RGBLED_SetValue(0,255,0);
		else
			RGBLED_SetValue(0,0,255);
		
		if(distance<0)
			sprintf(msg,"Range: %.2f cm\r\n",0);
		else
			sprintf(msg,"Range: %.2f cm\r\n",distance);
	}
	LCD_WriteStringAtPos(msg,1,0);
	
	if(SWT_GetValue(1))
		UART_PutString(msg);
}