#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
//#pragma config CCP2MX = PORTBE

#include "ST7735_TFT.h"
#include "main.h"
#include "utils.h"
#include "Main_Screen.h"
#include "Interrupt.h"
#include "I2C.h"
#include "I2C_Support.h"

#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz

char tempSecond = 0xff;
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

short Nec_OK = 0;
char Nec_Button;
extern unsigned long long Nec_code;



char array1[21]={0xA2, 0x62, 0xE2, 0x22, 0x02, 0xC2, 0xE0, 0xA8, 0x90, 0x68,
0x98, 0xB0, 0x30, 0x18, 0x7A, 0x10, 0x38, 0x5A, 0x42, 0x4A, 0x52};
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0",
                   "PRV\0", "NEX\0", "PLY\0",
                   "VL-\0", "VL+\0", "EQ \0",
                   " 0 \0", "100\0", "200\0",
                   " 1 \0", " 2 \0", " 3 \0",
                   " 4 \0", " 5 \0", " 6 \0",
                   " 7 \0", " 8 \0", " 9 \0"};

int color[21]={RD, RD, RD,
               CY, CY, GR,
               BU, BU, MA,
               BK, BK, BK,
               BK, BK, BK,
               BK, BK, BK,
               BK, BK, BK};

//array for PORTA (LED D1 andd D2)
char PORTA_LED[21] = {
    0x01, 0x01, 0x01,  //bit0-2 RD,RD,RD,CY,CY,GR,BU
    0x06, 0x06, 0x02,  //bit3-5 BU,BU,BK,BK,BK,BK,BK
    0x04, 0x20, 0x28,  //00101000
    0x38, 0x38, 0x38,  
    0x38, 0x38, 0x00,  
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00   
};

// array for PORTB (LED D3)
char PORTB_LED[21] = {
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x38,  
    0x38, 0x38, 0x38,  
    0x38, 0x38, 0x38
};

// array for PORTC (LED D4)
char PORTC_LED[21] = {
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00,  
    0x00, 0x00, 0x00,  
    0x04, 0x00, 0x00,  
    0x04, 0x04, 0x04,  
    0x04, 0x04, 0x04,  
    0x04, 0x04, 0x04
};

void main()
{
    
    Init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0;                              // Enable PORTB internal pull up resistor
    TRISB = 0x01;
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;
    ADCON1 = 0x0F;                          //
    TRISA = 0x00;
    TRISE = 0x00;
 
    Initialize_LCD_Screen();
    Init_Interrupt();
    I2C_Init(100000);
    DS1621_Init();

    Nec_code = 0x0;                         // Clear code
    char found;
    
    //part 3
//    DS3231_Setup_Time();
    
    while(1)
    {
        Deactivate_Buzzer();
        
        //part 1
//        signed char tempC = DS1621_Read_Temp();
//        char tempC1 = DS1621_Read_Temp_Bad();
//        signed char tempF = (tempC * 9 / 5) + 32;
//        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
//        Wait_One_Sec_Soft();
        
        //part 2
        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();
            signed char tempF = (tempC * 9 / 5) + 32;
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
        }
        
        
        if (Nec_OK == 1)
        {
            Nec_OK = 0;
            Enable_INT_Interrupt();
            printf ("NEC_Button = %x \r\n", Nec_Button);  

            //part 4
            if (Nec_Button == 0xA2)
            {
                DS3231_Setup_Time();
                printf("Time has been reset\r\n");
            }
            
            found = 0xff;
            
            // add code here to look for code using a FOR loop
            for (char i = 0; i< 21; i++)
            {
                if (Nec_Button == array1[i])
                {
                    found = i;
                }
            }
            
            if (found != 0xff) 
            {
				printf ("Key Location = %d \r\n\n", found);      
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
            
			// add code to output color for the RGB LEDS
                
                //just maskk bit
                PORTA = (PORTA & 0xC0) | PORTA_LED[found];
                
                //PORTB mask bits 3-5
                PORTB = (PORTB & 0xC6) | PORTB_LED[found];
                
                //PORTC mask bit 2
                PORTC = (PORTC & 0xFB) | PORTC_LED[found];
			
			// add code to handle the KEY_PRESSED LED and do the buzzer sound
                KEY_PRESSED = 1;
                Activate_Buzzer();
                Wait_One_Sec();
                Deactivate_Buzzer();
                KEY_PRESSED = 0;
            }
                
        }
    }
}


