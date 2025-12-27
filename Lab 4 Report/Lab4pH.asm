#include <P18F4620.inc>

    config OSC = INTIO67
    config WDT = OFF
    config LVP = OFF
    config BOREN = OFF

InA     equ 0x20
InB	equ 0x21
Result  equ 0x22

    ORG 0x0000

START:
    MOVLW   0x0F
    MOVWF   ADCON1	; ADCON1 = 0x0F

    MOVLW   0xFF        ; Set PORTA as input
    MOVWF   TRISA
    MOVLW   0xFF        ; Set PORTB as input
    MOVWF   TRISB
    MOVLW   0x00
    MOVWF   TRISC       ; Set PORTC as output
    MOVWF   TRISD       ; Set PORTD as output
    MOVLW   0x07        ; Set PORTE bits 0-2 as input
    MOVWF   TRISE

;==============================================================================
; MAIN PROGRAM LOOP aka function switch
;==============================================================================
MAIN_LOOP:
    ; Check Bit 2
    BTFSC   PORTE, 2        ; Bit 2 is 0 so skip the goto
    GOTO    BIT2_IS_1       ; It was 1, so jump to the second half of the logic.

BIT2_IS_0:
    ; Check Bit 1
    BTFSC   PORTE, 1        ; Is Bit 1 zero? Skip if it's 0.
    GOTO    BIT2_0_BIT1_1   ; It was 1 (path 01X)

BIT2_0_BIT1_0:              ; We are here if switches are 00X
    BTFSC   PORTE, 0        ; Is Bit 0 zero? Skip if it's 0.
    GOTO    TASK_ADD ; Switches are 001, perform ADD
    GOTO    TASK_COMP; Switches are 000, perform COMP

BIT2_0_BIT1_1:              ; We are here if switches are 01X
    BTFSC   PORTE, 0        ; Is Bit 0 zero? Skip if it's 0.
    GOTO    TASK_AND; Switches are 011, perform AND
    GOTO    TASK_OR ; Switches are 010, perform OR

BIT2_IS_1:
    ; Check Bit 1
    BTFSC   PORTE, 1        ; Is Bit 1 zero? Skip if it's 0.
    GOTO    BIT2_1_BIT1_1   ; It was 1 (path 11X)

BIT2_1_BIT1_0:              ; We are here if switches are 10X
    GOTO    TASK_XOR	    ; perform XOR

BIT2_1_BIT1_1:              ; We are here if switches are 11X
    GOTO    TASK_BCD; perform BCD

;==============================================================================
; TASK BLOCKS
;==============================================================================
TASK_COMP:	; LED indicator
    CLRF    PORTD       ; Indicator for COMP (000)
    CALL    SUB_COMP
    GOTO    MAIN_LOOP

TASK_ADD:
    MOVLW   0x01
    MOVWF   PORTD       ; Indicator for ADD (001)
    CALL    SUB_ADD
    GOTO    MAIN_LOOP

TASK_OR:
    MOVLW   0x02
    MOVWF   PORTD       ; Indicator for OR (010)
    CALL    SUB_OR
    GOTO    MAIN_LOOP
    
TASK_AND:
    MOVLW   0x03
    MOVWF   PORTD       ; Indicator for AND (011)
    CALL    SUB_AND
    GOTO    MAIN_LOOP

TASK_XOR:
    MOVLW   0x04
    MOVWF   PORTD       ; Indicator for XOR (10X)
    CALL    SUB_XOR
    GOTO    MAIN_LOOP
    
TASK_BCD:
    MOVLW   0x06
    MOVWF   PORTD       ; Indicator for BCD (11X)
    CALL    SUB_BCD
    GOTO    MAIN_LOOP

;==============================================================================
; SUBROUTINES
;==============================================================================

; 1's Complement
SUB_COMP:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    COMF    InA, W
    ANDLW   0x0F
    MOVWF   Result
    MOVFF   Result, PORTC
    BZ      Z_SET_COMP
    GOTO    Z_NOT_SET_COMP
Z_SET_COMP:
    BSF     PORTC, 5
    GOTO    TEST_DONE_COMP
Z_NOT_SET_COMP:
    BCF     PORTC, 5
TEST_DONE_COMP:
    RETURN

; Addition
SUB_ADD:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F
    MOVWF   InB
    MOVF    InA, W
    ADDWF   InB, W
    MOVWF   Result
    MOVFF   Result, PORTC
    BTFSS   STATUS, C
    GOTO    CARRY_CLEAR_ADD
    BSF     PORTC, 4
    GOTO    CHECK_ZERO_ADD
CARRY_CLEAR_ADD:
    BCF     PORTC, 4
CHECK_ZERO_ADD:
    BZ      Z_SET_ADD
    GOTO    Z_NOT_SET_ADD
Z_SET_ADD:
    BSF     PORTC, 5
    GOTO    TEST_DONE_ADD
Z_NOT_SET_ADD:
    BCF     PORTC, 5
TEST_DONE_ADD:
    RETURN

; OR
SUB_OR:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F
    MOVWF   InB
    MOVF    InA, W
    IORWF   InB, W
    MOVWF   Result
    MOVFF   Result, PORTC
    BTFSS   STATUS, C
    GOTO    CARRY_CLEAR_OR
    BSF     PORTC, 4
    GOTO    CHECK_ZERO_OR
CARRY_CLEAR_OR:
    BCF     PORTC, 4
CHECK_ZERO_OR:
    BZ      Z_SET_OR
    GOTO    Z_NOT_SET_OR
Z_SET_OR:
    BSF     PORTC, 5
    GOTO    TEST_DONE_OR
Z_NOT_SET_OR:
    BCF     PORTC, 5
TEST_DONE_OR:
    RETURN
    
; AND Subroutine
SUB_AND:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F
    MOVWF   InB
    MOVF    InA, W
    ANDWF   InB, W
    MOVWF   Result
    MOVFF   Result, PORTC
    BTFSS   STATUS, C
    GOTO    CARRY_CLEAR_AND
    BSF     PORTC, 4
    GOTO    CHECK_ZERO_AND
CARRY_CLEAR_AND:
    BCF     PORTC, 4
CHECK_ZERO_AND:
    BZ      Z_SET_AND
    GOTO    Z_NOT_SET_AND
Z_SET_AND:
    BSF     PORTC, 5
    GOTO    TEST_DONE_AND
Z_NOT_SET_AND:
    BCF     PORTC, 5
TEST_DONE_AND:
    RETURN
    
; XOR
SUB_XOR:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVF    PORTB, W
    ANDLW   0x0F
    MOVWF   InB
    MOVF    InA, W
    XORWF   InB, W
    MOVWF   Result
    MOVFF   Result, PORTC
    BTFSS   STATUS, C
    GOTO    CARRY_CLEAR_XOR
    BSF     PORTC, 4
    GOTO    CHECK_ZERO_XOR
CARRY_CLEAR_XOR:
    BCF     PORTC, 4
CHECK_ZERO_XOR:
    BZ      Z_SET_XOR
    GOTO    Z_NOT_SET_XOR
Z_SET_XOR:
    BSF     PORTC, 5
    GOTO    TEST_DONE_XOR
Z_NOT_SET_XOR:
    BCF     PORTC, 5
TEST_DONE_XOR:
    RETURN

; BCD Conversion
SUB_BCD:
    MOVF    PORTA, W
    ANDLW   0x0F
    MOVWF   InA
    MOVLW   0x09
    CPFSGT  InA
    GOTO    BCD_DONE
BCD_DONE:
    MOVFF   InA, PORTC
    MOVF    InA, F  
    BZ	    Z_SET_BCD
    GOTO    Z_NOT_SET_BCD
Z_SET_BCD:
    BSF     PORTC, 5    
    GOTO    TEST_DONE_BCD
Z_NOT_SET_BCD:
    BCF     PORTC, 5    
TEST_DONE_BCD:
    RETURN

    END	    ;fix code to store 'Result'