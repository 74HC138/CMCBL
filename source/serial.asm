;Setup USB serial port for 9600 baud
Serial_USB_init:
        ;setup timer
        ld a, 0B01000101
        out (SUSB_CLK), a
        ld a, 1
        out (SUSB_CLK), a

        ;setup serial
        ld a, 0B00011000
        out (SUSB_CMD), a ;channel reset
        ld a, 0B00000010
        out (SIO_B_CMD), a ;set interrupt vector
        ld a, 0B00010000
        out (SIO_B_CMD), a ;interrupt vector = 0x10
        ld a, 0B00010100
        out (SUSB_CMD), a
        ld a, 0B01000100
        out (SUSB_CMD), a ;CLK div 16, 1 stop bit, no parity
        ld a, 0B00000011
        out (SUSB_CMD), a
        ld a, 0B11100001
        out (SUSB_CMD), a ;8 bits per char, RTS CTS hw handshake, rx enabled
        ld a, 0B00000101
        out (SUSB_CMD), a
        ld a, 0B01101010
        out (SUSB_CMD), a ;8 bits per char, RTS set, tx enabled
        ld a, 0B00000001
        out (SIO_B_CMD), a
        ld a, 0B00000100
        out (SIO_B_CMD), a
        ld a, 0B00010001
        out (SUSB_CMD), a
        ld a, 0B00011110
        out (SUSB_CMD), a

        ret

;char to output in c
Serial_USB_putc:
        in a, (SUSB_CMD)
        and 0B00000100
        jr z, Serial_USB_putc

        ld a, c
        out (SUSB_DAT), a
        ret

;string in HL
Serial_USB_puts:
        ld a, (HL)
        cp 0x00
        ret z
        ld c, a
        call Serial_USB_putc
        inc HL
        jr Serial_USB_puts

;input char in a
Serial_USB_getc:
        in a, (SUSB_CMD)
        and 0B00000001
        jr z, Serial_USB_getc

        in a, (SUSB_DAT)
        ret


