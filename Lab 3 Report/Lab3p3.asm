;******************************************************************
;* This program will read the values of bits 0-2 of PORT A and    *
;* output them to the pins 0 through 2 of PORT C                  *
;******************************************************************

#include <P18F4620.inc>     
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF
    ORG 0x0000              ; Set the origin of the program to address 0x0000

;******************************************************************
;* This routine initializes the I/O ports                         *
;******************************************************************
START:
    MOVLW 0x0F              ; Load the literal value 0x0F into the WREG register
    MOVWF ADCON1            ; Configure PORTA and PORTE pins as digital I/O

    MOVLW 0xFF              ; Load the literal value 0xFF into the WREG register
    MOVWF TRISA             ; Set all pins of PORTA as inputs by setting the TRISA register
    MOVLW 0x00              ; Load the literal value 0x00 into the WREG register
    MOVWF TRISC             ; Set all pins of PORTC as outputs by clearing the TRISC register

;******************************************************************
;* This is the main loop that reads from PORTA and writes to PORTC *
;******************************************************************
MAIN_LOOP:
    MOVF PORTA, W           ; Move the contents of PORTA into the WREG register
    ANDLW 0x07              ; Mask the upper 5 bits of WREG, keeping only the lower 3 bits (RA0-RA2)
    MOVWF PORTC             ; Move the masked value from WREG to PORTC
    GOTO MAIN_LOOP          ; Create an infinite loop by jumping back to MAIN_LOOP
    END                     ; End of the assembly program