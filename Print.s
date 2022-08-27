; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
	PRESERVE8

val EQU	0 

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB


 

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R0-R5, LR}
	SUB	SP,	#8
	CMP R0, #10
	BLO more
	MOV R3, #10
	UDIV R2, R0, R3
	MUL R4, R2, R3
	ADD	R5, R4, #0x0A
	SUB R1, R0, R4
	ADD R0, R0, R2
	AND R5, R5, #1
	CMP	R5, #1
	STR R1, [SP, #val]
	BL LCD_OutDec
	LDR R0, [SP, #val]
	ADD R0, #0x30
	BL ST7735_OutChar
	BL done
more
	ADD R0, #0x30
	BL ST7735_OutChar
done
	
	ADD	SP,	#8
	POP {R0-R5, PC}



     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file

