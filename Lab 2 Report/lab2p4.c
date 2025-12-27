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
    for (int i=0; i<delay; i++);    // Loop to create a  delay
}

/****************************************************************/
/* Main Program                                                 */
/****************************************************************/
void main()
{
    char colord1[8] = {7, 5, 6, 3, 2, 0, 4, 1}; // Array for PORTC output pattern
    char colord2[8] = {6, 7, 1, 3, 4, 0, 2, 5}; // Array for PORTD output pattern
    TRISA = 0xFF;                   // Set PORTA to all inputs
    TRISC = 0x00;                   // Set PORTC to all outputs
    TRISD = 0x00;                   // Set PORTD to all outputs
    ADCON1 = 0x0F;                  // Configure PORTA pins as digital I/O
    int i;                          // Loop counter variable
    
    while(1)                        // Create an infinite loop
    {
        for(i=0; i<=7; i++)         // Loop through count from 0 to 7
        {
            PORTC = colord1[i];     // Output the next pattern to PORTC
            PORTD = colord2[i] << 5; // Output the next pattern to PORTD, shifted left by 5 bits
            
            Delay_One_Sec();        // Call the one-second delay function
        }
    }
}
