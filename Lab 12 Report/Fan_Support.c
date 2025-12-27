#include <p18f4620.h>
#include "main.h"
#include "Fan_Support.h"
#include "stdio.h"
#include "utils.h"

extern char FAN;
extern char duty_cycle;


int get_RPM()
{
    int RPS = TMR3L / 2; 
                       // then RPS = count /2
    TMR3L = 0;         // clear out the count
    return (RPS * 60); // return RPM = 65 * RPS
} 


void Toggle_Fan()
{
    if (FAN ==0)
    {
        Turn_On_Fan();
    }
    else
    {
        Turn_Off_Fan();
    }
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FANON_LED = 0;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FANON_LED = 1;
}

void Increase_Speed()
{
    if (duty_cycle >= 100)
    {
        Do_Beep();
        duty_cycle = 100;
    }
    else
    {
        duty_cycle = duty_cycle + 5;
    }

    do_update_pwm(duty_cycle);

}

void Decrease_Speed()
{
    if (duty_cycle <= 0)
    {
        Do_Beep();
        duty_cycle = 0;  
    }
    else
    {
        duty_cycle = duty_cycle - 5;
    }
    
    do_update_pwm(duty_cycle);
}

void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ; // set the frequency for 25 Khz
    T2CON = 0b00000111 ; //
    dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz
                                        // signal
    dc_I = (int) dc_f; // get the integer part
    if (dc_I > duty_cycle) dc_I++; // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2; 
}

void Set_DC_RGB(char duty_cycle)
{
    char PORTA_DC[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    
    int i = duty_cycle / 10;
    if (i >= 7)
    {
        i = 7;
    }
    PORTA = (PORTA & 0xF8) | PORTA_DC[i];
}

void Set_RPM_RGB(int rpm)
{
    char PORTA_RPM[8] = {0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};
    
    int a = rpm / 500;
    a = (rpm > 0) * (a + 1);//plus 1 if over 0 rpm
    if (a >= 7)
    {
        a = 7;
    }
    PORTA = (PORTA & 0xC7) | PORTA_RPM[a];
}


