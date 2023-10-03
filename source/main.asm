        org 0x0000
        jp Main

;--------------------------------------
;Includes
;--------------------------------------
        include CMC32A128.asm
        include serial.asm

;--------------------------------------
;Variable definition
;--------------------------------------
        dsect
        org RAM_BASE
DATA_BUFFER:
        dc 0x4000
ROM_WRITE_BUFFER:
        dc ROM_write_end - ROM_write
        dend

;--------------------------------------
;Code
;--------------------------------------
Main:
        ld SP, RAM_TOP

        ;set ROM bank register to point lowBank:0 highBank:1
        ld A, 0x10
        out (ROM_BA), A
        
        ;test for boot mode switch (1)
        ;if clear jump to ROM bank 1
        ;if set enter boot loader
        in A, (SYS_CB)
        and 0B00000001
        jp z, ROM_BANK1

        call Serial_USB_init
        ld HL, ROM_write
        ld DE, ROM_WRITE_BUFFER
        ld BC, ROM_write_end - ROM_write
        ldir ;copy rom write function to ram

CommandLoop:
        call Serial_USB_getc
        cp 'r'
        jp z, FunctionRead
        cp 'w'
        jp z, FunctionWrite
        cp 'g'
        jp z, FunctionGet
        cp 'p'
        jp z, FunctionPut
        cp 'c'
        jp z, FunctionCheck
        cp 's'
        jp z, FunctionSum
        cp '?'
        jp z, FunctionIndentify
        cp 'b'
        jp z, FunctionBoot
        cp 't'
        jp z, FunctionTest

        ld c, '!'
        call Serial_USB_putc
        jp CommandLoop

FunctionRead:
        call GetHexChar
        sla a
        sla a
        sla a
        sla a
        out (ROM_BA), a
        ld HL, ROM_BANK1
        ld DE, DATA_BUFFER
        ld BC, ROM_BANK1_SZ
        ldir
        ld c, '.'
        call Serial_USB_putc
        jp CommandLoop

FunctionWrite:
        call GetHexChar
        ld c, 0
        cp c
        jr nz, .skip
        ld IYL, 0xff ;set reset flag when writing to page 0 to reset the processor when writing is done
 .skip:
        sla a
        sla a
        sla a
        sla a
        call ROM_WRITE_BUFFER
        ld c, '.'
        call Serial_USB_putc
        jp CommandLoop

FunctionGet:
        ld HL, DATA_BUFFER
        ld DE, ROM_BANK1_SZ
 .loop:
        ld c, (HL)
        call Serial_USB_putc
        inc HL
        dec DE
        ld a, 0
        cp d
        jp nz, .loop
        cp e
        jp nz, .loop
        jp CommandLoop

FunctionPut:
        ld HL, DATA_BUFFER
        ld DE, ROM_BANK1_SZ
 .loop:
        call Serial_USB_getc
        ld (HL), a
        inc HL
        dec DE
        ld a, 0
        cp d
        jp nz, .loop
        cp e
        jp nz, .loop

        ld c, '.'
        call Serial_USB_putc
        jp CommandLoop
FunctionCheck:
        call GetHexChar
        sla a
        sla a
        sla a
        sla a
        out (ROM_BA), a
        ld HL, ROM_BANK1
        ld BC, ROM_BANK1_SZ
        ld IX, DATA_BUFFER
 .loop:
        ld a, (IX)
        inc IX
        cpi
        jp nz, .error
        jp pe, .loop
        
        ld c, '.'
        call Serial_USB_putc
        jp CommandLoop
 .error:
        ld c, '!'
        call Serial_USB_putc
        jp CommandLoop
FunctionSum:
        ld HL, DATA_BUFFER
        ld BC, ROM_BANK1_SZ
        ld d, 0
 .loop:
        ld a, d
        add a, (HL)
        inc HL
        dec BC
        ld d, a
        ld a, 0
        cp B
        jp nz, .loop
        cp C
        jp nz, .loop

        ld c, d
        call Serial_USB_putc        
        jp CommandLoop
FunctionIndentify:
        ld HL, .identString
        call Serial_USB_puts
        jp CommandLoop

 .identString:
        string "CMCBLv0_4."

FunctionTest:
        ld c, '.'
        call Serial_USB_putc
        jp CommandLoop
FunctionBoot:
        ld hl, RAM_BASE
        ld bc, RAM_SZ
        ld a, 0x00
    .fillLoop:
        ld (hl), a
        inc hl
        dec bc
        cp b
        jp nz, .fillLoop
        cp c
        jp nz, .fillLoop

        ld hl, 0
        ld bc, 0
        ld a, 0x10
        out (ROM_BA), a
        jp ROM_BANK1

GetHexChar:
        call Serial_USB_getc
        sub 0x30
        cp 0x0a
        ret c ;the digit was 0-9
        sub 0x07
        cp 0x10
        ret c ;the digit was A-F
        sub 0x20
        cp 0x10
        ret c ;the digit was a-f
        ld a, 0 ;the digit was not a hex digit, return 0
        ret
;rom bank in a
ROM_write:
        ld HL, DATA_BUFFER
        ld IX, ROM_BANK1
        ld BC, ROM_BANK1_SZ
        ld d, ROM_BANK1_PGSZ
        ld e, a 
 .writeInit:
        ;disable software data protection
        ld a, 0x10
        out (ROM_BA), a
        ld a, 0xaa
        ld (0x5555), a
        ld a, 0x00
        out (ROM_BA), a
        ld a, 0x55
        ld (0x6aaa), a
        ld a, 0x10
        out (ROM_BA), a
        ld a, 0xa0
        ld (0x5555), a
 
 .writeBlock:
        ld a, e
        out (ROM_BA), a
        ld a, (HL)
        ld (IX), a
        inc HL
        inc IX
        dec BC
        dec d
        ld a, 0
        cp d
        jr nz, .writeBlock
        
        dec HL
 .waitForFinished:
        ld a, (IX - 1)
        cp (HL)
        jr nz, .waitForFinished
        inc HL

        ld d, ROM_BANK1_PGSZ
        ld a, 0
        cp B
        jr nz, .writeInit
        cp C
        jr nz, .writeInit

        ld c, IYL
        ld a, 0
        cp c

        ret z
        rst 0
ROM_write_end:

