#include <stdio.h> 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h> 
 
#pragma config OSC = INTIO67 
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF 


#define delay 17960                   // Define a delay constant for ~1 second

/****************************************************************/
/* Delay_One_Sec() : Function to generate a 1-second delay      */
/****************************************************************/
void Delay_One_Sec()
{
    for (int i=0; i<delay; i++);    // Loop to create a delay
}

/****************************************************************/
/* Main Program                                                 */
/****************************************************************/
void main()
{
    char sw_value;                  // Variable to hold the count value
    TRISA = 0xFF;                   // Set PORTA to all inputs
    TRISB = 0x00;                   // Set PORTB to all outputs
    TRISC = 0x00;                   // Set PORTC to all outputs
    TRISD = 0x00;                   // Set PORTD to all outputs
    ADCON1 = 0x0F;                  // Configure PORTA pins as digital I/O
    
    while(1)                        // Create an infinite loop
    {
        for(sw_value=0; sw_value<=7; sw_value++) // Loop to count from 0 to 7
        {
            PORTC = sw_value;       // Output the current count to PORTC
            Delay_One_Sec();        // Call the one-second delay function
        }
    }
}





