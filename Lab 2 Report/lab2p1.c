#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 

/****************************************************************/
/* Main Program                                                 */
/****************************************************************/
void main(void)
{
    char in_sw;                     // Variable to store the input switch value
    TRISA = 0xFF;                   // Set PORTA to all inputs
    TRISB = 0x00;                   // Set PORTB to all outputs
    TRISC = 0x00;                   // Set PORTC to all outputs
    TRISD = 0x00;                   // Set PORTD to all outputs
    ADCON1 = 0x0F;                  // Configure PORTA pins as digital I/O
    
    while(1)                        // Create an infinite loop
    {
        in_sw = PORTA;              // Read the value from PORTA
        in_sw = in_sw & 0x07;       // Mask the upper bits to isolate RA0, RA1, and RA2
        PORTB = in_sw;              // Write the result to PORTB
    }
}