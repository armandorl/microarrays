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
;  PROCEDURE: ScaleSignal
;
;  DESCRIPTION:
;
;  Function used to adjust the ADC input signal (remove offset and scale it)
;
;  Parameters:
;   in/out - W0 points to the beginning of the data block
;   in     - W1 contains the offset value
;
; -----------------------------------------------------------------------------
    .text
    .global _ScaleSignal

_ScaleSignal:

    LAC  W1, B  ; move the offset value to ACCB
    ; scale the entire data block before processing
    DO #63, SCALE ; BLOCKSIZE - 1 times
    LAC [W0], A ; move the next data sample to ACCA
    SUB A       ; remove offset from signal ACCA = ACCA - ACCB
    SFTAC A, #6 ; shift ACCA by 6 bits for 10-bit ADC
    SCALE:
    SAC A, [W0++] ; store scaled input (overwrite original)
    

; -----------------------------------------------------------------------------
; END OF PROCEDURE: ScaleSignal
; -----------------------------------------------------------------------------

    .text
    .global _CalcTcy

_CalcTcy:

    REPEAT #30000
    BTG  PORTB, #15  ; move the offset value to ACCB
    RETURN

