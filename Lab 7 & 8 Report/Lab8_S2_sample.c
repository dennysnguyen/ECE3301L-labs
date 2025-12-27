#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"
#include "Interrupt.h"


void Set_EW(char color);
void Set_EW_LT(char color);
void Set_NS(char color);
void Set_NS_LT(char color);
void PED_Control( char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep();
void Wait_One_Second();
void Do_Flashing();


#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4        // Timer Clock 2 Mhz

//interrupt flags
char INT0_Flag;
char INT1_Flag;
char INT2_Flag; 

char NS_PED_SW = 0;
char EW_PED_SW = 0;

char Flashing_Request = 0;
char Flashing_Status = 0; 


char dir;
char Count;                             
char PED_Count;                         

char  MODE;
char direction;
float volt;

extern char Light_Sensor;

void main(void)
{
    OSCCON = 0x70;                      // set the system clock to be 4MHz
    TRISA = 0x1F;                       //inputs RA0/AN0-RA4 inputs,RA5 output
    TRISB = 0x07;                       // outputs
    TRISC = 0x00;                       
    TRISD = 0x00;                       
    TRISE = 0x00;                       
    Init_ADC();
    Init_UART();
    //PORTCbits.RC1 = 0                   //keep buzzer from staying on at the start of the program
    Initialize_LCD_Screen();                 // Initialize the TFT screen
    RBPU = 0;
    init_INTERRUPT();

//    while (1)
//    { // Do nothing,
//        if (INT0_Flag == 1)
//        {
//            INT0_Flag = 0; // clear the flag
//            printf("INT0 interrupt pin detected \r\n");
//                            // print a message that INT0 has
//                            // occurred
//        }
//        if (INT1_Flag == 1)
//        {
//            INT1_Flag = 0; // clear the flag
//            printf("INT1 interrupt pin detected \r\n");
//                            // print a message that INT1 has
//                            // occurred
//        }
//        if (INT2_Flag == 1)
//        {
//            INT2_Flag = 0; // clear the flag
//            printf("INT2 interrupt pin detected \r\n");
//                            // print a message that INT2 has
//                            // occurred
//        }
//    }
    
    
//    while (1)
//    {
//        Rcmd2red();
//        delay_ms(2);
//        
//    }

    volt = Read_Volt(0);                        

    Light_Sensor = volt < 2.5 ? 1:0;                 // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                    // forever loop
    {
        volt = Read_Volt(0);
        Light_Sensor = volt < 2.5 ? 1:0;
        
        if (Light_Sensor == 1)    
        {
            MODE_LED = 1;
            Day_Mode();                         // calls Day_Mode() function
        }
        else
        {
            MODE_LED = 0;
            Night_Mode();                       // calls Night_Mode() function
        }
        if (Flashing_Request == 1)
        {
            Flashing_Request = 0;
            Flashing_Status = 1;
            Do_Flashing();
        } 
    } 
}

void Set_EW(char color)
{
    direction = EW;
    update_LCD_color(direction,color);
    switch (color)
    {
        case OFF:    EW_RED = 0; EW_GREEN = 0; break;    // Turn off EW light
        case RED:    EW_RED = 1; EW_GREEN = 0; break;    // Set EW light to red
        case GREEN:  EW_RED = 0; EW_GREEN = 1; break;    // Set EW light to green
        case YELLOW: EW_RED = 1; EW_GREEN = 1; break;    // Set EW light to yellow
    }
}

void Set_EW_LT(char color)
{
    direction = EWLT;
    update_LCD_color(direction,color);
    switch (color)
    {
        case OFF:    EWLT_RED = 0; EWLT_GREEN = 0; break;// Turn off EWLT light
        case RED:    EWLT_RED = 1; EWLT_GREEN = 0; break;// Set EWLT light to red
        case GREEN:  EWLT_RED = 0; EWLT_GREEN = 1; break;// Set EWLT light to green
        case YELLOW: EWLT_RED = 1; EWLT_GREEN = 1; break;// Set EWLT light to yellow
    }
}

void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction,color);
    switch (color)
    {
        case OFF:    NS_RED = 0; NS_GREEN = 0; break;    // Turn off NS light
        case RED:    NS_RED = 1; NS_GREEN = 0; break;    // Set NS light to red
        case GREEN:  NS_RED = 0; NS_GREEN = 1; break;    // Set NS light to green
        case YELLOW: NS_RED = 1; NS_GREEN = 1; break;    // Set NS light to yellow
    }
}

void Set_NS_LT(char color)
{
    direction = NSLT;
    update_LCD_color(direction,color);
    switch (color)
    {
        case OFF:    NSLT_RED = 0; NSLT_GREEN = 0; break;// Turn off NSLT light
        case RED:    NSLT_RED = 1; NSLT_GREEN = 0; break;// Set NSLT light to red
        case GREEN:  NSLT_RED = 0; NSLT_GREEN = 1; break;// Set NSLT light to green
        case YELLOW: NSLT_RED = 1; NSLT_GREEN = 1; break;// Set NSLT light to yellow
    }
}


void PED_Control( char direction, char Num_Sec)
{ 
    char count;
    for (count = Num_Sec - 1; count > 0; count--)       // Loop for countdown from (Num_Sec - 1)
    {
        update_LCD_PED_Count(direction, count);
        Wait_One_Second_With_Beep();                    // Wait one second with a beep
    }
    update_LCD_PED_Count(direction, 0);
    if (direction == NS)
    {
        NS_PED_SW = 0;
    }
    if (direction == EW)
    {
        EW_PED_SW = 0;
    }
    Wait_One_Second_With_Beep();                        // one second wait
}

void Day_Mode()
{
    MODE = 1;
    printf("DAY MODE \r\n");                            
    printf("Step 1 \r\n");                              
    Set_NS(RED);                                        // Set North-South light to Red
    Set_NS_LT(RED);                                      // Set North-South Left-Turn light to Red
    Set_EW_LT(RED);                                      // Set East-West Left-Turn light to Red
    Set_EW(GREEN);                                      // Set East-West light to Green
    printf("Step 2 \r\n");                              
    printf("EWPED_SW = %d \r\n", EW_PED_SW);             
    if (EW_PED_SW == 1)                                  // Check if the East-West Pedestrian switch is pressed
    {
        printf("Step 3 \r\n");                          
        PED_Control(EW, PEDESTRIAN_EW_WAIT);                              // Start pedestrian countdown for East-West direction
    }
    printf("Step 4 \r\n");                              
    Wait_N_Seconds(EW_WAIT);                                  // Wait for 6 seconds
    printf("Step 5 \r\n");                              
    Set_EW(YELLOW);                                     // Set East-West light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 6 \r\n");                              
    Set_EW(RED);                                        // Set East-West light to Red
    printf("Step 7 \r\n");                              
    printf("NSLT_SW = %d \r\n", NS_LT_SW);               
    if (NS_LT_SW == 1)                                   // Check if the North-South Left-Turn switch is pressed
    {
        printf("Step 8 \r\n");                          
        Set_NS_LT(GREEN);                                // Set North-South Left-Turn light to Green
        Wait_N_Seconds(NS_LT_WAIT);                              // Wait for 7 seconds
        printf("Step 9 \r\n");                          
        Set_NS_LT(YELLOW);                               // Set North-South Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 10 \r\n");                         
        Set_NS_LT(RED);                                  // Set North-South Left-Turn light to Red
    }
    printf("Step 11 \r\n");                             
    Set_NS(GREEN);                                      // Set North-South light to Green
    printf("Step 12 \r\n");                             
    printf("NSPED = %d \r\n", NS_PED_SW);                
    if (NS_PED_SW == 1)                                  // Check if the North-South Pedestrian switch is pressed
    {
        printf("Step 13 \r\n");                         
        PED_Control(NS, PEDESTRIAN_NS_WAIT);                              // Start pedestrian countdown for North-South direction
    }
    printf("Step 14 \r\n");                             
    Wait_N_Seconds(NS_WAIT);                                  // Wait for 5 seconds
    printf("Step 15 \r\n");                             
    Set_NS(YELLOW);                                     // Set North-South light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 16 \r\n");                             
    Set_NS(RED);                                        // Set North-South light to Red
    printf("Step 17 \r\n");                             
    printf("EWLT = %d \r\n", EW_LT_SW);                  
    if (EW_LT_SW == 1)                                   // Check if the East-West Left-Turn switch is pressed
    {
        printf("Step 18 \r\n");                         
        Set_EW_LT(GREEN);                                // Set East-West Left-Turn light to Green
        Wait_N_Seconds(EW_LT_WAIT);                              // Wait for 6 seconds
        printf("Step 19 \r\n");                         
        Set_EW_LT(YELLOW);                               // Set East-West Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("STEP 20 \r\n");                         
        Set_EW_LT(RED);                                  // Set East-West Left-Turn light to Red
    }
    printf("STEP 21 \r\n");                             
    printf("DAY MODE COMPLETED \r\n");                  

}

void Night_Mode()
{ 
    EW_PED_SW = 0;
    NS_PED_SW = 0;
    MODE = 0;
    printf("NIGHT MODE \r\n");                          
    printf("Step 1 \r\n");                              
    Set_NS(RED);                                        // Set North-South light to Red
    Set_NS_LT(RED);                                      // Set North-South Left-Turn light to Red
    Set_EW_LT(RED);                                      // Set East-West Left-Turn light to Red
    Set_EW(GREEN);                                      // Set East-West light to Green
    printf("Step 2 \r\n");                              
    Wait_N_Seconds(NIGHT_EW_WAIT);                                  // Wait for 6 seconds
    printf("Step 3 \r\n");                              
    Set_EW(YELLOW);                                     // Set East-West light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 4 \r\n");                              
    Set_EW(RED);                                        // Set East-West light to Red
    printf("Step 5 \r\n");                              
    printf("NSLT_SW = %d \r\n", NS_LT_SW);               
    if (NS_LT_SW == 1)                                   // Check if the North-South Left-Turn switch is pressed
    {
        printf("Step 6 \r\n");                          
        Set_NS_LT(GREEN);                                // Set North-South Left-Turn light to Green
        Wait_N_Seconds(NIGHT_NS_LT_WAIT);                              // Wait for 6 seconds
        printf("Step 7 \r\n");                          
        Set_NS_LT(YELLOW);                               // Set North-South Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 8 \r\n");                          
        Set_NS_LT(RED);                                  // Set North-South Left-Turn light to Red
    }
    printf("Step 9 \r\n");                              
    Set_NS(GREEN);                                      // Set North-South light to Green
    Wait_N_Seconds(NIGHT_NS_WAIT);                                  // Wait for 7 seconds
    printf("Step 10 \r\n");                             
    Set_NS(YELLOW);                                     // Set North-South light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 11 \r\n");                             
    Set_NS(RED);                                        // Set North-South light to Red
    printf("Step 12 \r\n");                             
    printf("EWLT_SW = %d \r\n", EW_LT_SW);               
    if (EW_LT_SW == 1)                                   // Check if the East-West Left-Turn switch is pressed
    {
        printf("Step 13 \r\n");                         
        Set_EW_LT(GREEN);                                // Set East-West Left-Turn light to Green
        Wait_N_Seconds(NIGHT_EW_LT_WAIT);                              // Wait for 5 seconds
        printf("Step 14 \r\n");                         
        Set_EW_LT(YELLOW);                               // Set East-West Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 15 \r\n");                         
        Set_EW_LT(RED);                                  // Set East-West Left-Turn light to Red
    }
    printf("Step 16 \r\n");                             
    printf("NIGHT MODE COMPLETED \r\n");
}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Draw_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Erase_Star();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
	update_LCD_misc();
}

void Wait_One_Second_With_Beep()				//creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    Draw_Star();
    Activate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    Erase_Star();
    Deactivate_Buzzer();
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
	update_LCD_misc();
}
 

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
		update_LCD_count(direction, I);
        Wait_One_Second();          			// calls Wait_One_Second for x number of times
        
    }
	update_LCD_count(direction, 0);
}

void Do_Flashing()
{
    while (Flashing_Status == 1)
    {
        if (Flashing_Request == 0)
        {
            Set_NS_LT(RED);
            Set_NS(RED);
            Set_EW_LT(RED);
            Set_EW(RED);
            Wait_One_Second();
            
            Set_NS_LT(OFF);
            Set_NS(OFF);
            Set_EW_LT(OFF);
            Set_EW(OFF);
            Wait_One_Second();
        }
        else
        {
            Flashing_Request = 0;
            Flashing_Status = 0;
        }
    }
}
