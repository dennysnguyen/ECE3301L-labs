
#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Main.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"

extern char buffer[35];
extern char *nbr;
extern char *txt;
extern char tempC_Txt[];
extern char tempF_Txt[];
extern char time_Txt[];
extern char date_Txt[];
extern char Fan_Timer_Mode_Txt[];
extern char Fan_Timer_Txt[];

extern char Fan_Set_Temp_Txt[];
extern char Fan_Mode_Txt[];
extern char Fan_SW_Txt[];
 
extern char DC_Txt[];
extern char Volt_Txt[];
extern char RPM_Txt[];
extern char SYSTEM_ON;

extern signed char tempC, tempF;
extern unsigned char second, minute, hour, dow, day, month, year;

extern unsigned char fan_set_temp;
extern char duty_cycle;

extern char FAN, FANMODE , FAN_TIMER_MODE, FAN_TIMER_TIME;

extern int rpm;
extern float volt;
char Timing_Txt[] = "ECE3301L FinFa25/?/??\0";

void Initialize_Screen(void) 
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
        drawtext(1, 1, Timing_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
                                                        // X and Y coordinates of where the text is to be displayed
        strcpy(txt, "Temperature:");
        drawtext(temp_x  , temp_y , txt, ST7735_MAGENTA, ST7735_BLACK, TS_1);      // including text color and the background of it
        strcpy(txt, "C/");
        drawtext(tempc_x , tempc_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2); 
        strcpy(txt, "F");         
        drawtext(tempf_x , tempf_y, txt, ST7735_YELLOW , ST7735_BLACK, TS_2);
        strcpy(txt, "Time");
        drawtext(time_x  , time_y , txt, ST7735_BLUE   , ST7735_BLACK, TS_1);
        strcpy(txt, "Date");
        drawtext(date_x  , date_y , txt, ST7735_RED    , ST7735_BLACK, TS_1);
        strcpy(txt, "Timer Mode");
        drawtext(fan_timer_mode_x  , fan_timer_mode_y , txt, ST7735_YELLOW, ST7735_BLACK, TS_1);  
        strcpy(txt, "Timer Time");
        drawtext(fan_timer_x  , fan_timer_y , txt, ST7735_YELLOW, ST7735_BLACK, TS_1);     
        strcpy(txt, "FAN SetTemp");
        drawtext(fan_set_temp_x, fan_set_temp_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1); 
        strcpy(txt, "Mode");
        drawtext(fan_mode_x, fan_mode_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1);
        strcpy(txt, "SW");
        drawtext(fan_sw_x, fan_sw_y, txt, ST7735_BLUE  , ST7735_BLACK, TS_1);
        strcpy(txt, "DC");
        drawtext(dc_x, dc_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1);
        strcpy(txt, "Volt");
        drawtext(volt_x, volt_y, txt, ST7735_WHITE       , ST7735_BLACK  , TS_1);
        strcpy(txt, "RPM");
        drawtext(rpm_x, rpm_y, txt, ST7735_WHITE       , ST7735_BLACK  , TS_1); 
    }
    else
    {
        strcpy(txt, "SYSTEM OFF");
        drawtext(sysoff_x  , sysoff_y , txt, ST7735_MAGENTA, ST7735_BLACK, TS_2);
    }  
}

void Update_Screen(void)
{
    if (SYSTEM_ON == 1)
    {
        if ((tempC & 0x80) == 0x80) 
        {
            tempC_Txt[0] = '-';
            int neg_tempC = -tempC;
            tempC_Txt[1]  = (neg_tempC & 0x7f)/10  + '0';// Tens digit of C
            tempC_Txt[2]  = (neg_tempC & 0x7f)%10  + '0';// Ones digit of C
        }
        else
        {
            tempC_Txt[0] = '+';
            tempC_Txt[1]  = tempC/10  + '0';         // Tens digit of C
            tempC_Txt[2]  = tempC%10  + '0';         // Ones digit of C
        }

        if (tempF < 0) 
        {
            tempF_Txt[0] = '-';
            int neg_tempF = -tempF;
            tempF_Txt[1]  = neg_tempF/100  + '0';         // Hundred digit of F            
            tempF_Txt[2]  = (neg_tempF/10)%10  + '0';    // Tens digit of F
            tempF_Txt[3]  = neg_tempF%10  + '0';          // Ones digit of F
        }
        else
        {
            tempF_Txt[0] = '+';
            tempF_Txt[1]  = tempF/100  + '0';             // Hundred digit of F            
            tempF_Txt[2]  = (tempF/10)%10  + '0';         // Tens digit of F
            tempF_Txt[3]  = tempF%10  + '0';              // Ones digit of F
        }    

        time_Txt[0]  = (hour>>4)  + '0';                    // Hour MSD
        time_Txt[1]  = (hour & 0x0f)+ '0';                  // Hour LSD
        time_Txt[3]  = (minute>>4) + '0';// add code here;               		// Minute MSD
        time_Txt[4]  = (minute & 0x0f) + '0';// add code here;          			// Minute LSD
        time_Txt[6]  = (second>>4) + '0';// add code here;               		// Second MSD
        time_Txt[7]  = (second & 0x0f) + '0';// add code here;          			// Second LSD
        date_Txt[0]  = (month>>4) + '0';// add code here;                 	// Month MSD
        date_Txt[1]  = (month & 0x0f) + '0';// add code here;           			// Month LSD
        date_Txt[3]  = (day>>4) + '0';// add code here;                   	// Day MSD
        date_Txt[4]  = (day & 0x0f) + '0';// add code here;                 	// Day LSD
        date_Txt[6]  = (year>>4) + '0';// add code here;                  	// Year MSD
        date_Txt[7]  = (year & 0x0f) + '0';// add code here;           			// Year LSD

        if (FAN_TIMER_MODE == 0)
        {
            strcpy(Fan_Timer_Mode_Txt, "OFF");
        }  
        else if (FAN_TIMER_MODE == 1)
        {
            strcpy(Fan_Timer_Mode_Txt, "TM1");
        } 
        else if (FAN_TIMER_MODE == 2)
        {
            strcpy(Fan_Timer_Mode_Txt, "TM2");
        }    
        else if (FAN_TIMER_MODE == 3)
        {
            strcpy(Fan_Timer_Mode_Txt, "TM3");
        }    

        Fan_Timer_Txt[0] = FAN_TIMER_TIME/100 + '0';
        Fan_Timer_Txt[1] = FAN_TIMER_TIME/10 + '0';// add code here;
        Fan_Timer_Txt[2] = FAN_TIMER_TIME%10 + '0';// add code here;

        Fan_Set_Temp_Txt[0] = fan_set_temp/100 + '0';
        Fan_Set_Temp_Txt[1] = fan_set_temp/10 + '0';// add code here;
        Fan_Set_Temp_Txt[2] = fan_set_temp%10 + '0';// add code here;

        if (FANMODE == 1)
        {
            strcpy(Fan_Mode_Txt, "AUTO");
        }
        else 
        {
            strcpy(Fan_Mode_Txt, "MANU");
        }

        if (FAN == 1)
        {
            strcpy(Fan_SW_Txt, "ON ");
        }
        else 
        {
            strcpy(Fan_SW_Txt, "OFF");
        }


        DC_Txt[0] = duty_cycle/100  + '0';
        DC_Txt[1] = (duty_cycle/10)%10  + '0'; // add code here; 
        DC_Txt[2] = duty_cycle%10  + '0';// add code here;   

        Volt_Txt[0] = ((int)  volt        ) + '0';
        Volt_Txt[2] = (( (int) (volt*10 ) )%10) + '0';
        Volt_Txt[3] = (( (int) (volt*100) )%10) + '0';

        RPM_Txt[0] = rpm/1000  + '0';
        RPM_Txt[1] = (rpm/100)%10  + '0';  // add code here;  
        RPM_Txt[2] = (rpm/10)%10  + '0'; // add code here; 
        RPM_Txt[3] = rpm%10  + '0';// add code here;           

        drawtext(data_tempc_x, data_tempc_y, tempC_Txt , ST7735_YELLOW , ST7735_BLACK , TS_2);       
        drawtext(data_tempf_x, data_tempf_y, tempF_Txt , ST7735_YELLOW , ST7735_BLACK , TS_2);
        drawtext(data_time_x , data_time_y , time_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_2);
        drawtext(data_date_x , data_date_y , date_Txt  , ST7735_GREEN  , ST7735_BLACK , TS_2);
        drawtext(data_fan_timer_mode_x , data_fan_timer_mode_y , Fan_Timer_Mode_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_1); 
        drawtext(data_fan_timer_x , data_fan_timer_y , Fan_Timer_Txt  , ST7735_CYAN   , ST7735_BLACK , TS_1); 
        drawtext(data_fan_set_temp_x, data_fan_set_temp_y ,Fan_Set_Temp_Txt, ST7735_RED, ST7735_BLACK, TS_1);
        drawtext(data_fan_mode_x, data_fan_mode_y, Fan_Mode_Txt, ST7735_RED    , ST7735_BLACK , TS_1); 
        drawtext(data_fan_sw_x, data_fan_sw_y, Fan_SW_Txt, ST7735_RED    , ST7735_BLACK , TS_1);    
        drawtext(data_dc_x, data_dc_y, DC_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);

        drawtext(data_volt_x, data_volt_y, Volt_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);    
        drawtext(data_rpm_x, data_rpm_y, RPM_Txt, ST7735_GREEN, ST7735_BLACK, TS_1);
    }
        
}


