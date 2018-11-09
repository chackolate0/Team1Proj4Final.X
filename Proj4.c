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
#include "utils.h"
#include "srv.h"
#include <stdio.h>

#pragma config JTAGEN = OFF     // Turn off JTAG - required to use Pin RA0 as IO
#pragma config FNOSC = PRIPLL   //configure system clock 80 MHz
#pragma config FSOSCEN = OFF    // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT     // Primary Oscillator Configuration (XT osc mode)
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_2   //configure peripheral bus clock to 40 MHz

/*#define SYS_FREQ        (80000000L)     // 80MHz system clock (PB clock = 40MHz)
#define INT_SEC         10              // interrupt at 10th of a second
#define CORE_TICK_RATE  (SYS_FREQ/2/INT_SEC) //Using CoreTimer for 10th of a sec
#define Min_ms          20             //minimum update/shift time in ms
#define Max_ms          2000            //maximum update/shift time in ms*/

#define _SUPPRESS_PLIB_WARNING
_SUPPRESS_PLIB_WARNING

int angle;
int dutyCycle;
int stepSize = 2;
char displayText[80];
int btnlck = 0;
int turnClockwise = 0;

int clockwise = 1550;
int counterCwise = 1450;

void findAngle(){
    dutyCycle = SRV_GetPulse(1,1);
    angle=(int)(((dutyCycle-29) * 360)/(971-29+1));
}

int main(void){
    BTN_Init();
    SWT_Init();
    SRV_Init();
    LCD_Init();
    
    SRV_SetPulseMicroseconds1(1500);
    while(angle != 90){
            findAngle();
            SRV_SetPulseMicroseconds1(1600);
        }
    while(1){
        sprintf(displayText,"Tm:%2d,Step:%2ddeg", 0, stepSize);
        LCD_WriteStringAtPos(displayText, 0, 0);
        if(SWT_GetValue(7)){
            findAngle();
            if(!turnClockwise){
                SRV_SetPulseMicroseconds1(clockwise);
                while((angle - 90) % stepSize < 1){
                    SRV_SetPulseMicroseconds1(1500);
                    //delay_ms(500);
                    SRV_SetPulseMicroseconds1(clockwise);
                    findAngle();
                }
            }
            else{
                SRV_SetPulseMicroseconds1(counterCwise);
                while((angle - 90) % stepSize < 1){
                    SRV_SetPulseMicroseconds1(1500);
                    //delay_ms(500);
                    SRV_SetPulseMicroseconds1(counterCwise);
                    findAngle();
                }
            }
            if(angle < 93 && angle > 87){
                SRV_SetPulseMicroseconds1(clockwise);
                turnClockwise = 0;
            }
            if(angle < 273 && angle > 267){
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
    delay_ms(100);
    ADC_Init();
    
    char servo[80];
    LCD_WriteStringAtPos("Team: 1", 0, 0);
    while(1){
        sprintf(servo,"%d",SRV_GetPulse(0,4));
        LCD_WriteStringAtPos(servo,1,0);
        SRV_SetPulseMicroseconds0(1500);
    }
}

void update_SSD(int value){
    int first,second,third,fourth,decPt1,decPt2,decPt3,decPt4;
    char SSD1 = 0b0000000; //SSD setting for 1st SSD (LSD)
    char SSD2 = 0b0000000; //SSD setting for 2nd SSD
    char SSD3 = 0b0000000; //SSD setting for 3rd SSD
    char SSD4 = 0b0000000; //SSD setting for 4th SSD (MSD)
    if (value>9999){
        first=floor(value/10000);
        second=floor((value%10000)/1000);
        third=floor((value%1000)/100);
        fourth=floor((value%100)/10);
        decPt1=0;
        decPt2=0;
        decPt3=1;
        decPt4=0;
    }
    else if (value>999){
        first=floor((value%10000)/1000);
        second=floor((value%1000)/100);
        third=floor((value%100)/10);
        fourth=floor((value%10));
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=1;
    }
    else{
        first=0;
        second=floor((value%1000)/100);
        third=floor((value%100)/10);
        fourth=floor((value%10));
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=1;
    }
    if(value<0){                          //Display nothing if object is out of range
        first=18;
        second=18;
        third=18;
        fourth=18;
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=0;
    }
}

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 300; i++) {
        } //software delay ~1 millisec 
    }
}