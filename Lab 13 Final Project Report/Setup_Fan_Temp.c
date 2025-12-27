#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Main.h"
#include "I2C_Support.h"
#include "Setup_Fan_Temp.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"
#include "utils.h"

extern char setup_fan_set_text[];
unsigned char setup_fan_set_temp;

extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;

char Select_Fan_Temp_Field;
char In_Fan_Temp_Setup;

extern char *txt;
extern char found;
extern unsigned char fan_set_temp;
extern short Nec_ok;
extern char Timing_Txt[];
extern char SYSTEM_ON;
extern char tempSecond;
extern int rpm;
extern float volt;
extern signed char tempC, tempF;
extern char duty_cycle;
extern char light_flag;
extern char FAN_TIMER_TIME;

void Setup_Fan_Temp(void)
{

    In_Fan_Temp_Setup = 1;
    setup_fan_set_temp = fan_set_temp;
    Initialize_Setup_Fan_Screen();                  // Initialize setup heater screen
    Update_Setup_Fan_Screen();                      // Update the screen with latest info

    while (In_Fan_Temp_Setup == 1)
    {   
        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            rpm = get_RPM();            
 
            volt = Read_Volt(0);
            if (volt > 3.000) light_flag = 1;
            tempC = DS1621_Read_Temp();
            tempF = (tempC * 9 / 5) + 32;
            Set_D1_RGB(duty_cycle);
            Set_D2_RGB(rpm);
			Set_D3_RGB(FAN_TIMER_TIME);
            
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F", tempC, tempF);
            printf ("Light Sensor = %f V", volt);
            printf (" RPM = %d  dc = %d ls = %d\r\n", rpm, duty_cycle, light_flag);
        }
        if (check_for_button_input() == 1)
        {
             Nec_ok = 0;
             if (found >= Play_Pause && found <= EQ)
             {
                Do_Beep_Good(); 
                if (found == Play_Pause) Do_Save_New_Fan_Temp();
                
				// add if code to handle  'EQ', 'Plus', 'Minus' - See routines below
                if (found == EQ)
                    Exit_Setup_Fan_Temp();
                if (found == Plus)
                    Increase_Fan_Temp();
                if (found == Minus)
                    Decrease_Fan_Temp();

                found = 0xff;
             }
             else
             {
                Do_Beep_Bad();
             }             
        }                   
    }
}

void  Increase_Fan_Temp()		// routine for 'Plus' key
{
	// add code to increase setup_fan_set_temp by 1
	// then check if it is greater than 110. If so, limit to 110
    setup_fan_set_temp++;
    if (setup_fan_set_temp > 110)
        setup_fan_set_temp = 110;

    Update_Setup_Fan_Screen();              		// Update screen with latest info

}

void  Decrease_Fan_Temp()   						// routine for 'Minus' key     
{
	// add code to decrease setup_fan_set_temp by 1
	// then check if it is less than 50. If so, limit to 50
    setup_fan_set_temp--;
    if (setup_fan_set_temp < 50)
        setup_fan_set_temp = 50;

    Update_Setup_Fan_Screen();                      // Update screen with latest info

        
}

void Do_Save_New_Fan_Temp()							// routine for 'Play_Pause' key  
{
    fan_set_temp = setup_fan_set_temp;
    DS3231_Read_Time();                             // Read time
    Initialize_Screen();                            // Initialize the screen before returning  
    In_Fan_Temp_Setup = 0;    
}
    
void Exit_Setup_Fan_Temp()							// routine for 'EQ' key  
{
    DS3231_Read_Time();                             // Read time
    Initialize_Screen();                            // Initialize the screen before returning  
    In_Fan_Temp_Setup = 0;                                   
}


void Initialize_Setup_Fan_Screen(void) 
{ 
    LCD_Reset();                                    // Screen reset
    TFT_GreenTab_Initialize();         
    fillScreen(ST7735_BLACK);                       // Fills background of screen with color passed to it
    if (Semester == 0)
    {
        Timing_Txt[12] = 'S';  
        Timing_Txt[13] = 'P';         
    }
    else 
    {
        Timing_Txt[12] = 'F';   
        Timing_Txt[13] = 'A';       
    }
    Timing_Txt[14] = Year/10 + '0';
    Timing_Txt[15] = Year%10 + '0';
    Timing_Txt[17] = Session_Number%10 + '0';  
    Timing_Txt[19] = Table_Number/10 + '0';
    Timing_Txt[20] = Table_Number%10 + '0';   
 
    if (SYSTEM_ON == 1)
    {
        drawtext(start_x , start_y, Timing_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    

        strcpy(txt, " Fan Setup\0");                                       // Text displayed 
        drawtext(start_x , start_y+25, txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     

        strcpy(txt, "  Set Fan Temp");
        drawtext(setup_fan_x , setup_fan_y, txt, ST7735_CYAN  , ST7735_BLACK, TS_1);
    }
}

void Update_Setup_Fan_Screen(void)
{
    char dc_char1 = (setup_fan_set_temp/100);
    char dc_char2 = (setup_fan_set_temp/10)%10;// add code here temp ten ;									
    char dc_char3 = (setup_fan_set_temp%10);// add code here temp one ;
    
 
    setup_fan_set_text[0] = dc_char1 + '0';
    setup_fan_set_text[1] = dc_char2 + '0';
    setup_fan_set_text[2] = dc_char3 + '0'; 
    
    drawtext(setup_data_fan_x, setup_data_fan_y ,setup_fan_set_text, ST7735_RED, ST7735_BLACK, TS_2);
}

