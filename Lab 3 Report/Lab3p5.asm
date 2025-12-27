;******************************************************************
;* This program displays values from two arrays in memory to      *
;* PORTC and PORTD, cycling through them with a one-second delay. *
;******************************************************************

#include <P18F4620.inc>     

    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF


Delay1          equ     0xFF        ; Define a constant 'Delay1' with value 0xFF for the outer loop
Delay2          equ     0xF5        ; Define a constant 'Delay2' with value 0xF5 for the inner loop
Counter_L       equ     0x20        ; Assign the memory address 0x20 to 'Counter_L' for the low byte of the delay counter
Counter_H       equ     0x21        ; Assign the memory address 0x21 to 'Counter_H' for the high byte of the delay counter
Loop_Count      equ     0x29        ; Assign memory address 0x29 to be used as a loop counter
    ORG 0x0000                      ; Set the origin of the program to address 0x0000

;******************************************************************
;* This routine initializes two arrays in memory and configures I/O *
;******************************************************************
START:
    ; Initialize Array 1 at memory locations starting from 0x40
    MOVLW   0x07                    ; Load literal value 7 into WREG
    MOVWF   0x40                    ; Store WREG into memory address 0x40
    MOVLW   0x05                    ; Load literal value 5 into WREG
    MOVWF   0x41                    ; Store WREG into memory address 0x41
    MOVLW   0x06                    ; Load literal value 6 into WREG
    MOVWF   0x42                    ; Store WREG into memory address 0x42
    MOVLW   0x03                    ; Load literal value 3 into WREG
    MOVWF   0x43                    ; Store WREG into memory address 0x43
    MOVLW   0x02                    ; Load literal value 2 into WREG
    MOVWF   0x44                    ; Store WREG into memory address 0x44
    MOVLW   0x00                    ; Load literal value 0 into WREG
    MOVWF   0x45                    ; Store WREG into memory address 0x45
    MOVLW   0x04                    ; Load literal value 4 into WREG
    MOVWF   0x46                    ; Store WREG into memory address 0x46
    MOVLW   0x01                    ; Load literal value 1 into WREG
    MOVWF   0x47                    ; Store WREG into memory address 0x47

    ; Initialize Array 2 at memory locations starting from 0x50 (values are left-shifted by 5)
    MOVLW   0xC0                    ; Load literal value (6 << 5) into WREG
    MOVWF   0x50                    ; Store WREG into memory address 0x50
    MOVLW   0xE0                    ; Load literal value (7 << 5) into WREG
    MOVWF   0x51                    ; Store WREG into memory address 0x51
    MOVLW   0x20                    ; Load literal value (1 << 5) into WREG
    MOVWF   0x52                    ; Store WREG into memory address 0x52
    MOVLW   0x60                    ; Load literal value (3 << 5) into WREG
    MOVWF   0x53                    ; Store WREG into memory address 0x53
    MOVLW   0x80                    ; Load literal value (4 << 5) into WREG
    MOVWF   0x54                    ; Store WREG into memory address 0x54
    MOVLW   0x00                    ; Load literal value (0 << 5) into WREG
    MOVWF   0x55                    ; Store WREG into memory address 0x55
    MOVLW   0x40                    ; Load literal value (2 << 5) into WREG
    MOVWF   0x56                    ; Store WREG into memory address 0x56
    MOVLW   0xA0                    ; Load literal value (5 << 5) into WREG
    MOVWF   0x57                    ; Store WREG into memory address 0x57

    ; Configure I/O Ports
    MOVLW   0x0F                    ; Load the literal value 0x0F into the WREG register
    MOVWF   ADCON1                  ; Configure PORTA and PORTE pins as digital I/O
    MOVLW   0x00                    ; Load the literal value 0x00 into the WREG register
    MOVWF   TRISC                   ; Set all pins of PORTC as outputs by clearing the TRISC register
    MOVWF   TRISD                   ; Set all pins of PORTD as outputs by clearing the TRISD register

;******************************************************************
;* This is the main outer loop of the program                     *
;******************************************************************
WHILE_LOOP:
    MOVLW   0x00                    ; Load 0x00 into WREG
    MOVWF   FSR0H                   ; Set the high byte of FSR0 pointer to 0x00
    MOVLW   0x40                    ; Load 0x40 into WREG
    MOVWF   FSR0L                   ; Set the low byte of FSR0 pointer to 0x40 (points to Array 1)

    MOVLW   0x00                    ; Load 0x00 into WREG
    MOVWF   FSR1H                   ; Set the high byte of FSR1 pointer to 0x00
    MOVLW   0x50                    ; Load 0x50 into WREG
    MOVWF   FSR1L                   ; Set the low byte of FSR1 pointer to 0x50 (points to Array 2)

    MOVLW   0x08                    ; Load the literal value 8 into WREG
    MOVWF   Loop_Count              ; Initialize the 'Loop_Count' to 8

;******************************************************************
;* This inner loop reads from the arrays and outputs to the ports *
;******************************************************************
FOR_LOOP:
    MOVF    INDF0, W                ; Read the value from the address pointed to by FSR0 into WREG
    MOVWF   PORTC                   ; Output the value from WREG to PORTC
    MOVF    INDF1, W                ; Read the value from the address pointed to by FSR1 into WREG
    MOVWF   PORTD                   ; Output the value from WREG to PORTD

    CALL    DELAY_ONE_SEC           ; Call the one-second delay subroutine

    INCF    FSR0L, F                ; Increment the FSR0 pointer to the next element
    INCF    FSR1L, F                ; Increment the FSR1 pointer to the next element
    DECF    Loop_Count, F           ; Decrement the 'Loop_Count' by 1
    BNZ     FOR_LOOP                ; Branch back to FOR_LOOP if 'Loop_Count' is not zero

    GOTO    WHILE_LOOP              ; Jump back to the outer loop to repeat the entire sequence

;******************************************************************
;* This routine provides a one-second delay                       *
;******************************************************************
DELAY_ONE_SEC:
    MOVLW Delay1                    ; Load the 'Delay1' constant into WREG
    MOVWF Counter_H                 ; Initialize the outer loop counter (Counter_H)
LOOP_OUTER:
    NOP                             ; No operation, for timing adjustment
    MOVLW Delay2                    ; Load the 'Delay2' constant into WREG
    MOVWF Counter_L                 ; Initialize the inner loop counter (Counter_L)
LOOP_INNER:
    NOP                             ; No operation, for timing adjustment
    DECF Counter_L,F                ; Decrement the inner loop counter
    BNZ LOOP_INNER                  ; Branch back to LOOP_INNER if Counter_L is not zero
    DECF Counter_H,F                ; Decrement the outer loop counter
    BNZ LOOP_OUTER                  ; Branch back to LOOP_OUTER if Counter_H is not zero
    RETURN                          ; Return from the subroutine
    ; end of subroutine DELAY_ONE_SEC
    END                             ; End of the assembly program