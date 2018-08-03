
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _SquareMagnitude:     Computation of the Squared Magnitude of an array of
;                       real data elements,
;
; Operation:
;       dstV[n] = Real(srcV[n])^2, with
;
; n in {0, 1,... , numElems-1}
;
; Input:
;       w0 = number elements in vector(s) (numElems)
;       w1 = ptr to real source vector (srcV)
;            Elements in srcV are 16-bit real numbers
;       w2 = ptr to Real destination vector (dstV)
;            Elements in dstV are 16-bit real elements that
;            represent the squared magnitude
;       Note: It is allowed for the source and destination vectors
;             to be the same.
; Return:
;       w0 = ptr to Real destination vector (dstV)
;
; System resources usage:
;       {w0..w5}        used, not restored
;        AccuA          used, not restored
;        CORCON         saved, used, restored
;
; DO and REPEAT instruction usage.
;       1 level DO instruction
;       no REPEAT intructions
;
; Program words (24-bit instructions):
;       19
;
; Cycles (including C-function call and return overheads):
;       20 + 3*numElems
;............................................................................

        .global _SquareMagnitude    ; provide global scope to routine
_SquareMagnitude:

;............................................................................
        ; Save working registers.
        push    w2                      ; save return value (dstV)
        push.d  w4
        push    w10

        push    CORCON                  ; Prepare CORCON for fractional computation.
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

        .equ    FRACT_MODE,FRACT_MODE&0x3ff     ; get rid of leading (sign)
                                                ; bits so that the mask can
                                                ; be used as a Slit10

;............................................................................

        ; Prepare CORCON for fractional computation.
        mov     #FRACT_MODE,W3
        mov     W3,CORCON

;............................................................................

        ; Prepare operation.
        mov     w1, w10                 ; w10 points to beginning of complex input array
        clr     a
        MOV    [w10++], w4         ; Clear A, and Load W4 with Real(srcV[0]) and increment ptr

        dec     w0, w0                  ; w0 = numElems-1
        do      w0, L0
        mpy     w4*w4, a                ; accA = Real(srcV[n])^2
        MOV     [w10++], w4
L0:     sac.r   a, #0, [w2++]           ; round, saturate and store result in dstV[n] array

;Last iteration outside the DO loop
        mpy     w4*w4, a                ; accA = Real(srcV[n])^2
        MOV     [w10++], w4
        sac.r   a, #0, [w2++]           ; round, saturate and store result in dstV[n] array

; }

;............................................................................

        ; restore CORCON.
        pop     CORCON

        ; Restore working registers.
        pop     w10
        pop.d   w4
        pop     w0                              ; restore return value.

;............................................................................

        return

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF




