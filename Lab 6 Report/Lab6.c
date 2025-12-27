#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

/******************************************************************************/
/* Prototype Area                                                             */
/******************************************************************************/
void Init_UART(void);
void Init_ADC(void);
void Init_TRIS(void);
void putch(char c);
void Set_ADCON(char AN_pin);
unsigned int get_full_ADC(void);
void Wait_Half_Second(void);
void Wait_One_Second(void);
void Wait_N_Seconds(char seconds);
void Set_EW(char color);
void Set_EWLT(char color);
void Set_NS(char color);
void Set_NSLT(char color);
void Activate_Buzzer(void);
void Deactivate_Buzzer(void);
void Wait_One_Second_With_Beep(void);
void PED_Control(char Direction, char Num_Sec);
void Night_Mode(void);
void Day_Mode(void);

/******************************************************************************/
/* Variables                                                                  */
/******************************************************************************/
// Color definitions
#define OFF             0                               
#define RED             1                               
#define GREEN           2                               
#define YELLOW          3                               

// RGB LED definitions for traffic lights
#define EW_RED          PORTAbits.RA5                   
#define EW_GREEN        PORTCbits.RC0                   
#define EWLT_RED        PORTCbits.RC2                   
#define EWLT_GREEN      PORTCbits.RC3                   
#define NS_RED          PORTCbits.RC4                   
#define NS_GREEN        PORTCbits.RC5                   
#define NSLT_RED        PORTEbits.RE0                   
#define NSLT_GREEN      PORTEbits.RE1                   

// Indicator LED definitions
#define SEC_LED         PORTDbits.RD7                   
#define MODE_LED        PORTEbits.RE2                   

// Input switch definitions
#define NSLT_SW         PORTAbits.RA1                   
#define NSPED_SW        PORTAbits.RA2                   
#define EWPED_SW        PORTAbits.RA3                   
#define EWLT_SW         PORTAbits.RA4                   

/******************************************************************************/
/* Seven Segment array                                                        */
/******************************************************************************/
const char Seven_Seg[10] = {
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

/******************************************************************************/
/* Set_ADCON(char AN_pin): Selects the analog channel for ADC conversion      */
/******************************************************************************/
void Set_ADCON(char AN_pin)
{
    ADCON0 = AN_pin * 4 + 1;                            
}

/******************************************************************************/
/* get_full_ADC(void): Performs an ADC conversion and returns the result      */
/******************************************************************************/
unsigned int get_full_ADC(void)
{
    int result;
    ADCON0bits.GO = 1;                                  // Start ADC conversion
    while(ADCON0bits.DONE == 1);                        // Wait for conversion to complete
    result = (ADRESH * 0x100) + ADRESL;                 // Combine upper and lower bytes for full result
    return result;                                      // Return the 10-bit result
}

void putch(char c)
{
    while (!TRMT);                                      
    TXREG = c;                                          
}

/******************************************************************************/
/* Init_UART(void): Initializes the UART for communication                    */
/******************************************************************************/
void Init_UART()
{
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF &     // Configure USART settings
              USART_ASYNCH_MODE & USART_EIGHT_BIT &
              USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;                                      // Set oscillator to 8MHz
}

/******************************************************************************/
/* Init_ADC(void): Initializes the Analog-to-Digital Converter module         */
/******************************************************************************/
void Init_ADC()
{
    ADCON0 = 0x01;                                      // Select channel AN0, turn on ADC
    ADCON1 = 0x0E;                                      // Set AN0 as analog, VDD/VSS as refs
    ADCON2 = 0xA9;                                      
}

/******************************************************************************/
/* Init_TRIS(void): Configures the direction of I/O ports and initial state  */
/******************************************************************************/
void Init_TRIS()
{
    TRISA = 0x1F;                                       // Set RA0-RA4 as inputs (switches/ADC)
    TRISD = 0x00;                                       // Set PORTD as output
    TRISB = 0x00;                                       // Set PORTB as output
    TRISE = 0x00;                                       // Set PORTE as output
    TRISC = 0x00;                                       // Set PORTC as output

    PORTA = 0x00;                                       // Clear PORTA initial state
    PORTB = 0xFF;                                       // Turn off 7-segment on PORTB
    PORTC = 0x00;                                       // Clear PORTC initial state
    PORTD = 0xFF;                                       // Turn off 7-segment on PORTD
    PORTE = 0x00;                                       // Clear PORTE initial state
}

/******************************************************************************/
/* Wait_Half_Second(void): Creates a 500ms delay                              */
/******************************************************************************/
void Wait_Half_Second()
{
    T0CON = 0x03;                                       // Timer0, 16-bit mode, prescaler 1:16
    TMR0L = 0x51;                                       // Set the lower byte of TMR0
    TMR0H = 0x0A;                                       // Set the upper byte of TMR0
    INTCONbits.TMR0IF = 0;                              // Clear the Timer0 interrupt flag
    T0CONbits.TMR0ON = 1;                               // Turn on Timer0
    while (INTCONbits.TMR0IF == 0);                     // Wait for the Timer0 flag to be set
    T0CONbits.TMR0ON = 0;                               // Turn off Timer0
}

/******************************************************************************/
/* Wait_One_Second(void): Creates a 1 second delay and blinking SEC_LED       */
/******************************************************************************/
void Wait_One_Second()
{
    SEC_LED = 1;                                        // Turn on the second indicator LED
    Wait_Half_Second();                                 // Wait for 500ms
    SEC_LED = 0;                                        // Turn off the second indicator LED
    Wait_Half_Second();                                 // Wait for another 500ms
}

/******************************************************************************/
/* Wait_N_Seconds(char seconds): Loops to create a delay of N seconds         */
/******************************************************************************/
void Wait_N_Seconds(char seconds)
{
    char I;
    for (I = 0; I < seconds; I++)                       // Loop for the specified number of seconds
    {
        Wait_One_Second();                              // Call the 1-second wait function
    }
}

/******************************************************************************/
/* Set_EW(char color): Sets the color of the East-West traffic light          */
/******************************************************************************/
void Set_EW(char color)
{
    switch (color)
    {
        case OFF:    EW_RED = 0; EW_GREEN = 0; break;    // Turn off EW light
        case RED:    EW_RED = 1; EW_GREEN = 0; break;    // Set EW light to red
        case GREEN:  EW_RED = 0; EW_GREEN = 1; break;    // Set EW light to green
        case YELLOW: EW_RED = 1; EW_GREEN = 1; break;    // Set EW light to yellow
    }
}

/******************************************************************************/
/* Set_EWLT(char color): Sets the color of the E-W Left-Turn light            */
/******************************************************************************/
void Set_EWLT(char color)
{
    switch (color)
    {
        case OFF:    EWLT_RED = 0; EWLT_GREEN = 0; break;// Turn off EWLT light
        case RED:    EWLT_RED = 1; EWLT_GREEN = 0; break;// Set EWLT light to red
        case GREEN:  EWLT_RED = 0; EWLT_GREEN = 1; break;// Set EWLT light to green
        case YELLOW: EWLT_RED = 1; EWLT_GREEN = 1; break;// Set EWLT light to yellow
    }
}

/******************************************************************************/
/* Set_NS(char color): Sets the color of the North-South traffic light        */
/******************************************************************************/
void Set_NS(char color)
{
    switch (color)
    {
        case OFF:    NS_RED = 0; NS_GREEN = 0; break;    // Turn off NS light
        case RED:    NS_RED = 1; NS_GREEN = 0; break;    // Set NS light to red
        case GREEN:  NS_RED = 0; NS_GREEN = 1; break;    // Set NS light to green
        case YELLOW: NS_RED = 1; NS_GREEN = 1; break;    // Set NS light to yellow
    }
}

/******************************************************************************/
/* Set_NSLT(char color): Sets the color of the N-S Left-Turn light            */
/******************************************************************************/
void Set_NSLT(char color)
{
    switch (color)
    {
        case OFF:    NSLT_RED = 0; NSLT_GREEN = 0; break;// Turn off NSLT light
        case RED:    NSLT_RED = 1; NSLT_GREEN = 0; break;// Set NSLT light to red
        case GREEN:  NSLT_RED = 0; NSLT_GREEN = 1; break;// Set NSLT light to green
        case YELLOW: NSLT_RED = 1; NSLT_GREEN = 1; break;// Set NSLT light to yellow
    }
}

/******************************************************************************/
/* Activate_Buzzer(void): Turn on the buzzer                                  */
/******************************************************************************/
void Activate_Buzzer()
{
    PR2 = 0b11111001;                                   // Set period register for PWM
    T2CON = 0b00000101;                                 // Timer2 on, prescaler 1:4
    CCPR2L = 0b01001010;                                // Set duty cycle
    CCP2CON = 0b00111100;                               // Configure CCP2 for PWM mode
}

/******************************************************************************/
/* Deactivate_Buzzer(void): Turns off the buzzer                              */
/******************************************************************************/
void Deactivate_Buzzer()
{
    CCP2CON = 0x00;                                     // Disable PWM on CCP2
    PORTCbits.RC1 = 0;                                  // Ensure the pin is low
}

/******************************************************************************/
/* Wait_One_Second_With_Beep(void): Waits 1 sec while activating the buzzer   */
/******************************************************************************/
void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;                                        // Turn on the second indicator LED
    Activate_Buzzer();                                  // Activate the buzzer
    Wait_Half_Second();                                 // Wait for 500ms
    SEC_LED = 0;                                        // Turn off the second indicator LED
    Deactivate_Buzzer();                                // Deactivate the buzzer
    Wait_Half_Second();                                 // Wait for another 500ms
}

/******************************************************************************/
/* PED_Control(char, char): Manages pedestrian countdown                      */
/******************************************************************************/
void PED_Control(char Direction, char Num_Sec)
{
    char count;
    for (count = Num_Sec - 1; count > 0; count--)       // Loop for countdown from (Num_Sec - 1)
    {
        if (Direction == 0)                             // Check if it's the East-West direction
        {
            PORTD = (Seven_Seg[count] & 0x7F);          // Display count on PORTD 7-segment
            PORTB = 0xFF;                               // Turn off PORTB 7-segment
        }
        else                                            // Else, it's the NS direction
        {
            PORTB = Seven_Seg[count];                   // Display count on PORTB 7-segment
            PORTD = 0x7F;                               // Turn off PORTD 7-segment
        }
        Wait_One_Second_With_Beep();                    // Wait one second with a beep
    }
    PORTD = 0x7F;                                       // Turn off PORTD 7-segment display
    PORTB = 0xFF;                                       // Turn off PORTB 7-segment display
    Wait_One_Second_With_Beep();                        // one second wait
}

/******************************************************************************/
/* Night_Mode(void): Traffic light control sequence for nighttime             */
/******************************************************************************/
void Night_Mode()
{
    printf("NIGHT MODE \r\n");                          
    printf("Step 1 \r\n");                              
    Set_NS(RED);                                        // Set North-South light to Red
    Set_NSLT(RED);                                      // Set North-South Left-Turn light to Red
    Set_EWLT(RED);                                      // Set East-West Left-Turn light to Red
    Set_EW(GREEN);                                      // Set East-West light to Green
    printf("Step 2 \r\n");                              
    Wait_N_Seconds(6);                                  // Wait for 6 seconds
    printf("Step 3 \r\n");                              
    Set_EW(YELLOW);                                     // Set East-West light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 4 \r\n");                              
    Set_EW(RED);                                        // Set East-West light to Red
    printf("Step 5 \r\n");                              
    printf("NSLT_SW = %d \r\n", NSLT_SW);               
    if (NSLT_SW == 1)                                   // Check if the North-South Left-Turn switch is pressed
    {
        printf("Step 6 \r\n");                          
        Set_NSLT(GREEN);                                // Set North-South Left-Turn light to Green
        Wait_N_Seconds(6);                              // Wait for 6 seconds
        printf("Step 7 \r\n");                          
        Set_NSLT(YELLOW);                               // Set North-South Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 8 \r\n");                          
        Set_NSLT(RED);                                  // Set North-South Left-Turn light to Red
    }
    printf("Step 9 \r\n");                              
    Set_NS(GREEN);                                      // Set North-South light to Green
    Wait_N_Seconds(7);                                  // Wait for 7 seconds
    printf("Step 10 \r\n");                             
    Set_NS(YELLOW);                                     // Set North-South light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 11 \r\n");                             
    Set_NS(RED);                                        // Set North-South light to Red
    printf("Step 12 \r\n");                             
    printf("EWLT_SW = %d \r\n", EWLT_SW);               
    if (EWLT_SW == 1)                                   // Check if the East-West Left-Turn switch is pressed
    {
        printf("Step 13 \r\n");                         
        Set_EWLT(GREEN);                                // Set East-West Left-Turn light to Green
        Wait_N_Seconds(5);                              // Wait for 5 seconds
        printf("Step 14 \r\n");                         
        Set_EWLT(YELLOW);                               // Set East-West Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 15 \r\n");                         
        Set_EWLT(RED);                                  // Set East-West Left-Turn light to Red
    }
    printf("Step 16 \r\n");                             
    printf("NIGHT MODE COMPLETED \r\n");                
}

/******************************************************************************/
/* Day_Mode(void): Traffic light control sequence for daytime                 */
/******************************************************************************/
void Day_Mode()
{
    printf("DAY MODE \r\n");                            
    printf("Step 1 \r\n");                              
    Set_NS(RED);                                        // Set North-South light to Red
    Set_NSLT(RED);                                      // Set North-South Left-Turn light to Red
    Set_EWLT(RED);                                      // Set East-West Left-Turn light to Red
    Set_EW(GREEN);                                      // Set East-West light to Green
    printf("Step 2 \r\n");                              
    printf("EWPED_SW = %d \r\n", EWPED_SW);             
    if (EWPED_SW == 1)                                  // Check if the East-West Pedestrian switch is pressed
    {
        printf("Step 3 \r\n");                          
        PED_Control(0, 8);                              // Start pedestrian countdown for East-West direction
    }
    printf("Step 4 \r\n");                              
    Wait_N_Seconds(6);                                  // Wait for 6 seconds
    printf("Step 5 \r\n");                              
    Set_EW(YELLOW);                                     // Set East-West light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 6 \r\n");                              
    Set_EW(RED);                                        // Set East-West light to Red
    printf("Step 7 \r\n");                              
    printf("NSLT_SW = %d \r\n", NSLT_SW);               
    if (NSLT_SW == 1)                                   // Check if the North-South Left-Turn switch is pressed
    {
        printf("Step 8 \r\n");                          
        Set_NSLT(GREEN);                                // Set North-South Left-Turn light to Green
        Wait_N_Seconds(7);                              // Wait for 7 seconds
        printf("Step 9 \r\n");                          
        Set_NSLT(YELLOW);                               // Set North-South Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("Step 10 \r\n");                         
        Set_NSLT(RED);                                  // Set North-South Left-Turn light to Red
    }
    printf("Step 11 \r\n");                             
    Set_NS(GREEN);                                      // Set North-South light to Green
    printf("Step 12 \r\n");                             
    printf("NSPED = %d \r\n", NSPED_SW);                
    if (NSPED_SW == 1)                                  // Check if the North-South Pedestrian switch is pressed
    {
        printf("Step 13 \r\n");                         
        PED_Control(1, 7);                              // Start pedestrian countdown for North-South direction
    }
    printf("Step 14 \r\n");                             
    Wait_N_Seconds(5);                                  // Wait for 5 seconds
    printf("Step 15 \r\n");                             
    Set_NS(YELLOW);                                     // Set North-South light to Yellow
    Wait_N_Seconds(2);                                  // Wait for 2 seconds
    printf("Step 16 \r\n");                             
    Set_NS(RED);                                        // Set North-South light to Red
    printf("Step 17 \r\n");                             
    printf("EWLT = %d \r\n", EWLT_SW);                  
    if (EWLT_SW == 1)                                   // Check if the East-West Left-Turn switch is pressed
    {
        printf("Step 18 \r\n");                         
        Set_EWLT(GREEN);                                // Set East-West Left-Turn light to Green
        Wait_N_Seconds(6);                              // Wait for 6 seconds
        printf("Step 19 \r\n");                         
        Set_EWLT(YELLOW);                               // Set East-West Left-Turn light to Yellow
        Wait_N_Seconds(2);                              // Wait for 2 seconds
        printf("STEP 20 \r\n");                         
        Set_EWLT(RED);                                  // Set East-West Left-Turn light to Red
    }
    printf("STEP 21 \r\n");                             
    printf("DAY MODE COMPLETED \r\n");                  
}

/******************************************************************************/
/* main(): Main program routine                                               */
/******************************************************************************/
void main(void)
{
    Init_UART();                                        // Initialize UART 
    Init_ADC();                                         // Initialize the ADC 
    Init_TRIS();                                        // Initialize I/O port 
    
    while (1)
    {
        Set_ADCON(0);                                   // Select channel AN0 for the photoresistor
        int step = get_full_ADC();                      // Read ADC value
        float pr_volt = step * (5.0 / 1024.0);          // Convert ADC steps to voltage
        printf("Photo resistor voltage: %fV \r\n", pr_volt); 
        
        // Determine mode: 1 for Day (voltage < 2.5V), 0 for Night
        unsigned int mode = pr_volt < 2.5 ? 1 : 0;
        
        if (mode)                                       // If mode is day
        {
            MODE_LED = 1;                               // Turn on the Day Mode indicator LED
            Day_Mode();                                 // Run the daytime traffic sequence
        }
        else                                            // If mode is 0 night
        {
            MODE_LED = 0;                               // Turn off the Day Mode indicator LED
            Night_Mode();                               // Run the nighttime traffic sequence
        }
    }
}