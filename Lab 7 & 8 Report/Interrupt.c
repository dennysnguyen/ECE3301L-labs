#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"

extern char INT0_Flag;
extern char INT1_Flag;
extern char INT2_Flag;
extern char EW_PED_SW;
extern char NS_PED_SW;
extern char MODE;

extern char Flashing_Request;

void init_INTERRUPT()
{

    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT2IF = 0;

    INTCON2bits.INTEDG0 = 1;
    INTCON2bits.INTEDG1 = 1;
    INTCON2bits.INTEDG2 = 1;

    INTCONbits.INT0IE = 1;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT2IE = 1;

    INTCONbits.GIE = 1;
}

void interrupt  high_priority chkisr()
{
    if (INTCONbits.INT0IF == 1)
    {
        INT0_ISR(); // check if INT0 has occurred		
    }
    if (INTCON3bits.INT1IF == 1)
    {
        INT1_ISR();
    }
    if (INTCON3bits.INT2IF == 1)
    {
        INT2_ISR();
    }	
    
}

void INT0_ISR()
{
    INTCONbits.INT0IF=0; // Clear the interrupt flag
    if (MODE == 1)
    {
        NS_PED_SW = 1;
    }
    else
        NS_PED_SW = 0;
        
}

void INT1_ISR()
{
    INTCON3bits.INT1IF = 0;
    if (MODE == 1)
    {
        EW_PED_SW = 1;
    }
    else
        EW_PED_SW = 0;
}

void INT2_ISR()
{
    INTCON3bits.INT2IF = 0;
    Flashing_Request = 1;
}

