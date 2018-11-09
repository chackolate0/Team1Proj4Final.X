/* ************************************************************************** */
/** Descriptive File Name

  @Company
 CPEG 222 Team 1
 * Alex Chacko & Matias Saavedra

  @File Name
 Proj4.c

  @Summary
    Brief description of the file.

  @Description
 Project 4 Final Stage
 */


// PIC32MX370F512L Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // Shadow Register Set Priority Select (SRS Priority 7)
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_1       // System PLL Output Clock Divider (PLL Divide by 256)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config FSOSCEN = OFF             // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = XT            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is Disabled)
#pragma config JTAGEN = OFF              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include <xc.h>
#include "adc.h"
#include "btn.h"
#include "config.h"
#include "lcd.h"
#include <math.h>
#include "rgbled.h"
#include "srv.h"
#include "ssd.h"
#include <stdio.h>
#include "swt.h"
#include "uart.h"
#include "ultr.h"
#include "utils.h"

/* TODO:  Include other files here if needed. */

void delay_ms(int ms);
void update_SSD(int value);

int main(void){
    BTN_Init();
    delay_ms(100);
    LCD_Init();
    delay_ms(100);
    //RGBLED_Init();
    delay_ms(100);
    SSD_Init();
    delay_ms(100);
    SWT_Init();
    delay_ms(100);
    UART_Init(9600);
    delay_ms(100);
    ULTR_Init(0,1,0,2);//Echo A1, Trigger A2
    delay_ms(100);
    float distance;
    char msg[80];
    int ultDist;
    LCD_WriteStringAtPos("Team: 1",0,0);
    while(1){
        delay_ms(100);
        ultDist = ULTR_MeasureDist();
        update_SSD(ultDist);
        SRV_SetPulseMicroseconds0(1500);
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

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 1300; i++) {
        } //software delay ~1 millisec
    }
}