#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 
#define delay 5 

/****************************************************************/
/* Main Program                                                 */
/****************************************************************/
void main(void)
{
    char sw_value;                  // Variable to store the input switch value
    TRISA = 0xFF;                   // Set PORTA as an input port
    TRISC = 0x00;                   // Set PORTC as an output port
    ADCON1 = 0x0F;                  // Configure PORTA pins as digital I/O
    
    while(1)                        // Create an infinite loop
    {
        sw_value = PORTA;           // Read the value from PORTA 
        sw_value = sw_value & 0x07; // Mask the upper bits to isolate RA0, RA1, and RA2
        PORTC = sw_value;           // Write the result to PORTC
    }
}