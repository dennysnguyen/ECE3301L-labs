;******************************************************************
;* THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS           *
;* CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B                    *
;******************************************************************

#include<P18F4620.inc>     
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF
    
Delay1 equ 0xFF             ; Define a constant 'Delay1' with value 0xFF for the outer loop
Delay2 equ 0XF5             ; Define a constant 'Delay2' with value 0xF5 for the inner loop
Counter_L equ 0x20          ; Assign the memory address 0x20 to 'Counter_L' for the low byte of the delay counter
Counter_H equ 0x21          ; Assign the memory address 0x21 to 'Counter_H' for the high byte of the delay counter
    ORG 0x0000              ; Set the origin of the program to address 0x0000
    
;******************************************************************
;* This routine initializes the I/O ports                         *
;******************************************************************
START:
    MOVLW 0x0F              ; Load the literal value 0x0F into the WREG register
    MOVWF ADCON1            ; Configure PORTA and PORTE pins as digital I/O
    MOVLW 0x00              ; Load the literal value 0x00 into the WREG register
    MOVWF TRISB             ; Set all pins of PORTB as outputs by clearing the TRISB register

;******************************************************************
;* This is the main loop of the program that flashes the LEDs     *
;******************************************************************
MAIN_LOOP:
    MOVLW 0x05              ; Load the literal value 0x05 (binary 00000101) into WREG
    MOVWF PORTB             ; Output the value to PORTB, turning on LEDs at RB0 and RB2
    CALL DELAY_ONE_SEC      ; Call the one-second delay subroutine
    MOVLW 0x0A              ; Load the literal value 0x0A (binary 00001010) into WREG
    MOVWF PORTB             ; Output the value to PORTB, turning on LEDs at RB1 and RB3
    CALL DELAY_ONE_SEC      ; Call the one-second delay subroutine
    GOTO MAIN_LOOP          ; Create an infinite loop by jumping back to MAIN_LOOP
    
;******************************************************************
;* This routine provides a one-second delay                       *
;******************************************************************
DELAY_ONE_SEC:
    MOVLW Delay1            ; Load the 'Delay1' constant into WREG
    MOVWF Counter_H         ; Initialize the outer loop counter (Counter_H)
LOOP_OUTER:
    NOP                     ; No operation, for timing adjustment
    MOVLW Delay2            ; Load the 'Delay2' constant into WREG
    MOVWF Counter_L         ; Initialize the inner loop counter (Counter_L)
LOOP_INNER:
    NOP                     ; No operation, for timing adjustment
    DECF Counter_L,F        ; Decrement the inner loop counter
    BNZ LOOP_INNER          ; Branch back to LOOP_INNER if Counter_L is not zero
    DECF Counter_H,F        ; Decrement the outer loop counter
    BNZ LOOP_OUTER          ; Branch back to LOOP_OUTER if Counter_H is not zero
    RETURN                  ; Return from the subroutine
    ; end of subroutine DELAY_ONE_SEC
    END                     ; End of the assembly program