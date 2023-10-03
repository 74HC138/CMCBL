    ;defines for the peripherals of the CMC32A128
    ;ROM banks
    defc ROM_BASE               = 0x0000
    defc ROM_TOP                = 0x7fff
    defc ROM_BANK0              = 0x0000
    defc ROM_BANK0_TOP          = 0x3fff
    defc ROM_BANK0_SZ           = 0x4000
    defc ROM_BANK0_PGSZ         = 0x80
    defc ROM_BANK1              = 0x4000
    defc ROM_BANK1_TOP          = 0x7fff
    defc ROM_BANK1_SZ           = 0x4000
    defc ROM_BANK1_PGSZ         = 0x80

    ;RAM
    defc RAM_BASE               = 0x8000
    defc RAM_TOP                = 0xffff
    defc RAM_SZ                 = 0x8000

    ;peripherals
    defc PER_PIO_BASE           = 0x00
    defc PER_CTC_BASE           = 0x04
    defc PER_SIO_BASE           = 0x08
    defc PER_ETC_BASE           = 0x0c

    ;PIO
    defc PIO_A_DAT              = PER_PIO_BASE + 0
    defc PIO_A_CMD              = PER_PIO_BASE + 1
    defc PIO_B_DAT              = PER_PIO_BASE + 2
    defc PIO_B_CMD              = PER_PIO_BASE + 3

    ;CTC
    defc CTC_CH0                = PER_CTC_BASE + 0
    defc CTC_CH1                = PER_CTC_BASE + 1
    defc CTC_CH2                = PER_CTC_BASE + 2
    defc CTC_CH3                = PER_CTC_BASE + 3

    ;SIO
    defc SIO_A_DAT              = PER_SIO_BASE + 0
    defc SIO_A_CMD              = PER_SIO_BASE + 1
    defc SIO_B_DAT              = PER_SIO_BASE + 2
    defc SIO_B_CMD              = PER_SIO_BASE + 3

    ;Etcetera
    defc ADC_CTR                = PER_ETC_BASE + 0 ;write only
    defc DAC_0                  = PER_ETC_BASE + 1 ;write only
    defc DAC_1                  = PER_ETC_BASE + 2 ;write only
    defc ROM_BA                 = PER_ETC_BASE + 3 ;write only
    defc ADC_DAT                = PER_ETC_BASE + 0 ;read only
    defc SYS_CB                 = PER_ETC_BASE + 1 ;read only


    ;special
    defc SIO_A_CLK              = CTC_CH0
    defc SIO_B_CLK              = CTC_CH1

    defc SUSB_DAT               = SIO_A_DAT
    defc SUSB_CMD               = SIO_A_CMD
    defc SUSB_CLK               = SIO_A_CLK