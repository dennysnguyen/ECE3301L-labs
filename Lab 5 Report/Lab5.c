#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

/******************************************************************************/
/* Prototype                                                                  */
/******************************************************************************/
void putch(char c);                                     
void Init_UART(void);                                   
void Init_ADC(void);                                    
void Init_TRIS(void);                                   
void Set_ADCON0(char AN_pin);                           
unsigned int get_full_ADC(void);                        
void Set_D1_Color(int temp_F);                          
void Set_D2_Color(int temp_F);                          
void Set_D3_Color(float voltage);                       
void WAIT_ONE_SEC(void);                                
void Set_D2_Off();                                      
void Set_D2_Red();                                      
void Set_D2_Green();                                    
void Set_D2_Yellow();                                   
void Set_D2_Blue();                                     
void Set_D2_Purple();                                   
void Set_D2_Cyan();                                     
void Set_D2_White();                                    
void Set_D3_White();                                    
void Set_D3_Blue();                                     
void Set_D3_Green();                                    
void Set_D3_Red();                                      
void Set_D3_Off();                                      

/******************************************************************************/
/* Variables                                                                   */
/******************************************************************************/
#define D1_RED      PORTCbits.RC0                       
#define D1_GREEN    PORTCbits.RC1                       
#define D1_BLUE     PORTCbits.RC2                       
#define D2_RED      PORTCbits.RC3                       
#define D2_GREEN    PORTCbits.RC4                       
#define D2_BLUE     PORTCbits.RC5                       
#define D3_RED      PORTEbits.RE0                       
#define D3_GREEN    PORTEbits.RE1                       
#define D3_BLUE     PORTEbits.RE2                       

/******************************************************************************/
/* Seven Segment array                                                        */
/******************************************************************************/
const char LED_7seg[10] = {
    0b1000000,                                          // 0
    0b1111001,                                          // 1
    0b0100100,                                          // 2
    0b0110000,                                          // 3
    0b0011001,                                          // 4
    0b0010010,                                          // 5
    0b0000010,                                          // 6
    0b1111000,                                          // 7
    0b0000000,                                          // 8
    0b0010000                                           // 9
};

void putch(char c)
{
    while (!TRMT);                                      
    TXREG = c;                                          
}

/******************************************************************************/
/* Init_UART(void): Initializes UART                                           */
/******************************************************************************/
void Init_UART(void)
{
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF &     // Configure USART settings
              USART_ASYNCH_MODE & USART_EIGHT_BIT &
              USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x60;                                      // Set oscillator to 4MHz
}

/******************************************************************************/
/* Set_ADCON0(char AN_pin): Selects the analog channel for ADC conversion     */
/******************************************************************************/
void Set_ADCON0(char AN_pin)
{
    ADCON0 = AN_pin * 4 + 1;                            
}

/******************************************************************************/
/* Init_ADC() Initializes the Analog-to-Digital Converter module              */
/******************************************************************************/
void Init_ADC()
{
    ADCON0 = 0x01;                                      // Select channel AN0, turn on ADC
    ADCON1 = 0x1A;                                      // Set AN0-AN4 as analog, VDD/VSS as refs
    ADCON2 = 0xA9;                                      // Right justify, set acquisition and conversion times
}

/******************************************************************************/
/* Init_TRIS(void): Configures the direction of I/O ports                     */
/******************************************************************************/
void Init_TRIS()
{
    TRISA = 0x3F;                                       // Set PORTA pins as inputs (for ADC)
    TRISD = 0x00;                                       // Set PORTD as output (for 7-segment)
    TRISB = 0x00;                                       // Set PORTB as output (for 7-segment)
    TRISE = 0x00;                                       // Set PORTE as output (for D3 LED)
    TRISC = 0x00;                                       // Set PORTC as output (for D1, D2 LEDs)
}

/******************************************************************************/
/* get_full_ADC(void): Performs an ADC conversion and returns the 10-bit result */
/******************************************************************************/
unsigned int get_full_ADC(void)
{
    int result;
    ADCON0bits.GO = 1;                                  // Start ADC conversion
    while(ADCON0bits.DONE == 1);                        // Wait for conversion to complete
    result = (ADRESH * 0x100) + ADRESL;                 // Combine upper and lower bytes for full result
    return result;                                      // Return the 10-bit result
}

/******************************************************************************/
/* Set_D1_Color(int temp_F): Sets the color of LED D1 based on temperature    */
/******************************************************************************/
void Set_D1_Color(int temp_F)
{
    int color;
    color = temp_F / 10;                                // Determine color value based on tens digit of temp
    if (color < 0) color = 0;                           
    if (color > 7) color = 7;                           
    PORTC = color | (PORTC & 0xF8);                     // Set the lower 3 bits of PORTC without changing others
}

/******************************************************************************/
/* Set_D2_Color(int temp_F): Sets LED D2 color based on temperature ranges    */
/******************************************************************************/
void Set_D2_Color(int temp_F)
{
    if (temp_F < 32)                                    // Check if temperature is below 32 F
    {
        Set_D2_Off();                                   // Turn LED off
    }
    else if (temp_F < 42)                               // Check if temperature is between 32 and 41 F
    {
        Set_D2_Red();                                   // Set LED to Red
    }
    else if (temp_F < 52)                               // Check if temperature is between 42 and 51 F
    {
        Set_D2_Green();                                 // Set LED to Green
    }
    else if (temp_F < 62)                               // Check if temperature is between 52 and 61 F
    {
        Set_D2_Yellow();                                // Set LED to Yellow
    }
    else if (temp_F < 72)                               // Check if temperature is between 62 and 71 F
    {
        Set_D2_Blue();                                  // Set LED to Blue
    }
    else if (temp_F < 77)                               // Check if temperature is between 72 and 76 F
    {
        Set_D2_Purple();                                // Set LED to Purple
    }
    else if (temp_F <= 80)                              // Check if temperature is between 77 and 80 F
    {
        Set_D2_Cyan();                                  // Set LED to Cyan
    }
    else                                                // If temperature is above 80 F
    {
        Set_D2_White();                                 // Set LED to White
    }
}

/******************************************************************************/
/* Set_D3_Color(float voltage): Sets D3 color based on photoresistor voltage  */
/******************************************************************************/
void Set_D3_Color(float voltage)
{
    if (voltage < 2.6)                                  // Check if voltage is below 2.6V
    {
        Set_D3_White();                                 // Set LED to White
    }
    else if (voltage < 2.9)                             // Check if voltage is between 2.6V and 2.9V
    {
        Set_D3_Blue();                                  // Set LED to Blue
    }
    else if (voltage < 3.2)                             // Check if voltage is between 2.9V and 3.2V
    {
        Set_D3_Green();                                 // Set LED to Green
    }
    else if (voltage < 3.7)                             // Check if voltage is between 3.2V and 3.7V
    {
        Set_D3_Red();                                   // Set LED to Red
    }
    else                                                // If voltage is above 3.7V
    {
        Set_D3_Off();                                   // Turn LED off
    }
}

/******************************************************************************/
/* WAIT_ONE_SEC(void): A simple software delay loop                           */
/******************************************************************************/
void WAIT_ONE_SEC(void)
{
    for (int i = 0; i < 0xFFFF; i++);                   // Loop to create a delay
}

/******************************************************************************/
/* main(): Main program routine                                               */
/******************************************************************************/
void main()
{
    Init_UART();                                        // Initialize UART 
    Init_ADC();                                         // Initialize the ADC module
    Init_TRIS();                                        // Initialize I/O port 

    printf("Lab 5 Initialized. \r\n\r\n");               
    WAIT_ONE_SEC();                                     // Wait a second

    while(1)                                            // Infinite loop for continuous operation
    {
        // Temperature sensor reading
        Set_ADCON0(0);                                  // Select channel AN0 for the temperature sensor
        unsigned int temp_steps = get_full_ADC();       // Read ADC value
        float temp_voltage = temp_steps * (4.0/1024);   // Convert ADC steps to voltage
        float temp_C = (1.035 - temp_voltage) / .00550; // Convert voltage to Celsius
        float temp_F = ((temp_C * 9.0/5.0) + 32.0);     // Convert Celsius to Fahrenheit
        char temp_F_int = (int)temp_F;                  // Convert Fahrenheit to an integer

        PORTD = LED_7seg[temp_F_int / 10];              // Display the tens digit on the upper 7-segment
        PORTB = LED_7seg[temp_F_int % 10];              // Display the ones digit on the lower 7-segment

        Set_D1_Color(temp_F_int);                       // Set color of LED D1 
        Set_D2_Color(temp_F_int);                       // Set color of LED D2 

        printf("Voltage: %fV, Temp: %dF \r\n", temp_voltage, temp_F_int); // Print data to serial port

        // Photoresistor reading
        Set_ADCON0(4);                                  // Select channel AN4 for the photoresistor
        unsigned int pr_steps = get_full_ADC();         // Read ADC value
        float pr_voltage = pr_steps * (4.0/1024);       // Convert ADC steps to voltage
        printf("Light Voltage: %fV \r\n\r\n", pr_voltage); // Print light sensor voltage
        Set_D3_Color(pr_voltage);                       // Set color of LED D3 based on light level

        WAIT_ONE_SEC();                                 // Wait for one second before the next reading
    }
}

/******************************************************************************/
/* Set_D2_Off(): Turns off LED D2                                             */
/******************************************************************************/
void Set_D2_Off()
{
    D2_RED = 0;                                         // Turn off red component
    D2_GREEN = 0;                                       // Turn off green component
    D2_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D2_Red(): Sets LED D2 to red                                           */
/******************************************************************************/
void Set_D2_Red()
{
    D2_RED = 1;                                         // Turn on red component
    D2_GREEN = 0;                                       // Turn off green component
    D2_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D2_Green(): Sets LED D2 to green                                       */
/******************************************************************************/
void Set_D2_Green()
{
    D2_RED = 0;                                         // Turn off red component
    D2_GREEN = 1;                                       // Turn on green component
    D2_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D2_Yellow(): Sets LED D2 to yellow                                     */
/******************************************************************************/
void Set_D2_Yellow()
{
    D2_RED = 1;                                         // Turn on red component
    D2_GREEN = 1;                                       // Turn on green component
    D2_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D2_Blue(): Sets LED D2 to blue                                         */
/******************************************************************************/
void Set_D2_Blue()
{
    D2_RED = 0;                                         // Turn off red component
    D2_GREEN = 0;                                       // Turn off green component
    D2_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D2_Purple(): Sets LED D2 to purple                                     */
/******************************************************************************/
void Set_D2_Purple()
{
    D2_RED = 1;                                         // Turn on red component
    D2_GREEN = 0;                                       // Turn off green component
    D2_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D2_Cyan(): Sets LED D2 to cyan                                         */
/******************************************************************************/
void Set_D2_Cyan()
{
    D2_RED = 0;                                         // Turn off red component
    D2_GREEN = 1;                                       // Turn on green component
    D2_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D2_White(): Sets LED D2 to white                                       */
/******************************************************************************/
void Set_D2_White()
{
    D2_RED = 1;                                         // Turn on red component
    D2_GREEN = 1;                                       // Turn on green component
    D2_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D3_White(): Sets LED D3 to white                                       */
/******************************************************************************/
void Set_D3_White()
{
    D3_RED = 1;                                         // Turn on red component
    D3_GREEN = 1;                                       // Turn on green component
    D3_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D3_Blue(): Sets LED D3 to blue                                         */
/******************************************************************************/
void Set_D3_Blue()
{
    D3_RED = 0;                                         // Turn off red component
    D3_GREEN = 0;                                       // Turn off green component
    D3_BLUE = 1;                                        // Turn on blue component
}

/******************************************************************************/
/* Set_D3_Green(): Sets LED D3 to green                                       */
/******************************************************************************/
void Set_D3_Green()
{
    D3_RED = 0;                                         // Turn off red component
    D3_GREEN = 1;                                       // Turn on green component
    D3_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D3_Red(): Sets LED D3 to red                                           */
/******************************************************************************/
void Set_D3_Red()
{
    D3_RED = 1;                                         // Turn on red component
    D3_GREEN = 0;                                       // Turn off green component
    D3_BLUE = 0;                                        // Turn off blue component
}

/******************************************************************************/
/* Set_D3_Off(): Turns off LED D3                                             */
/******************************************************************************/
void Set_D3_Off()
{
    D3_RED = 0;                                         // Turn off red component
    D3_GREEN = 0;                                       // Turn off green component
    D3_BLUE = 0;                                        // Turn off blue component
}