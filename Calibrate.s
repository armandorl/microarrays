; -----------------------------------------------------------------------------
; GENERAL ASSEMBLY NOTES:
; Returned values
; Wn registers are 16 bits long
; Function return values are returned in W0 for 8- or 16-bit scalars, W1:W0 for 32-bit
; scalars, and W3:W2:W1:W0 for 64-bit scalars.

; Preserving registers
; The compiler arranges for registers W8-W15 to be preserved across ordinary function
; calls. Registers W0-W7 are available as scratch registers. For interrupt functions, the
; compiler arranges for all necessary registers to be preserved, namely W0-W15 and RCOUNT
; -----------------------------------------------------------------------------

; -----------------------------------------------------------------------------
;
;  PROCEDURE: Calibrate
;
;  DESCRIPTION:
;
;  Obtains the offset value for each signal
;
;  Parameters:
;   in/out - W0 points to the beginning of the data block
;
; -----------------------------------------------------------------------------

    .text
    .global _Calibrate

; Operational modes for fractional computation:
    .equ    SATA_ON,1<<7                    ; AccA sat. enabled
    .equ    SATB_ON,1<<6                    ; AccB sat. enabled
    .equ    SATDW_ON,1<<5                   ; data write sat. enabled
    .equ    ACCSAT_ON,1<<4                  ; Accs sat. 9.31 enabled
    .equ    FRACT_SET,SATA_ON|SATB_ON|SATDW_ON|ACCSAT_ON    ; set mask

    .equ    RND_OFF,~(1<<1)                 ; convergent rnd. enabled
    .equ    IF_OFF,~(1<<0)                  ; multiply fractional enabled
    .equ    FRACT_RESET,RND_OFF&IF_OFF      ; reset mask
    .equ    FRACT_MODE,FRACT_SET&FRACT_RESET        ; fractional mask
    .equ    FRACT_MODE,FRACT_MODE&0x3ff

_Calibrate:
    push	CORCON
	MOV     #FRACT_MODE,W3
    MOV     W3,CORCON

    CLR A         ; Set the accumulator to 0

    ; repeat for all the array
    REPEAT #63 ; BLOCKSIZE - 1 times
    ADD [W0++],A

    SAC A, W2 ; store scaled input
    MOV #64, W3   ; W3 = 64 (BLOCKSIZE)
    DIV.U W2, W3  ; W0 = W2 / W3

    ; Restore CORCON
    pop	CORCON

    RETURN

; -----------------------------------------------------------------------------
; END OF PROCEDURE: Calibrate
; -----------------------------------------------------------------------------



