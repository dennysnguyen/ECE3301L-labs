;******************************************************************
;* This program will cycle through the values 0x00 to 0x07 on     *
;* PORTC, with a one-second delay between each value.             *
;******************************************************************

#include<P18F4620.inc>      
    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF


Delay1 equ 0xFF             ; Define a constant 'Delay1' with value 0xFF for the outer loop
Delay2 equ 0xF5             ; Define a constant 'Delay2' with value 0xF5 for the inner loop
Counter_L equ 0x20          ; Assign the memory address 0x20 to 'Counter_L' for the low byte of the delay counter
Counter_H equ 0x21          ; Assign the memory address 0x21 to 'Counter_H' for the high byte of the delay counter
Color_Value equ 0x28        ; Assign memory address 0x28 to store the current output value for PORTC
Loop_Count equ 0x29         ; Assign memory address 0x29 to be used as a loop counter
    ORG 0x0000              ; Set the origin of the program to address 0x0000

;******************************************************************
;* This routine initializes the I/O ports                         *
;******************************************************************
START:
    MOVLW 0x0F              ; Load the literal value 0x0F into the WREG register
    MOVWF ADCON1            ; Configure PORTA and PORTE pins as digital I/O
    MOVLW 0x00              ; Load the literal value 0x00 into the WREG register
    MOVWF TRISC             ; Set all pins of PORTC as outputs by clearing the TRISC register

;******************************************************************
;* This is the main outer loop of the program                     *
;******************************************************************
WHILE_LOOP:
    MOVLW 0x00              ; Load the literal value 0x00 into WREG
    MOVWF Color_Value       ; Initialize 'Color_Value' to 0
    MOVLW 0x08              ; Load the literal value 8 into WREG
    MOVWF Loop_Count        ; Initialize the 'Loop_Count' to 8

;******************************************************************
;* This inner loop cycles through 8 values and outputs to PORTC   *
;******************************************************************
FOR_LOOP:
    MOVF Color_Value,W      ; Move the current 'Color_Value' into the WREG register
    MOVWF PORTC             ; Output the value from WREG to PORTC
    CALL DELAY_ONE_SEC      ; Call the one-second delay subroutine
    INCF Color_Value,F      ; Increment the 'Color_Value' by 1
    DECF Loop_Count,F       ; Decrement the 'Loop_Count' by 1
    BNZ FOR_LOOP            ; Branch back to FOR_LOOP if 'Loop_Count' is not zero

    GOTO WHILE_LOOP         ; Jump back to the outer loop to repeat the entire sequence

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