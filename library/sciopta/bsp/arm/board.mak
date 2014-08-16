# -*-makefile-*-
# Boards:
# 0 - Atmel At75C220-DK
# 1 - Ashling evba7
# 2 - GP32
# 3 - phyCore2294
# 4 - at91eb40a
# 5 - Sandgate2
# 6 - ARMulate ARM7TDMI
# 7 - phyCore55800
# 8 - at91sam7s-ek
# 9 - at91sam7x-ek
#10 - str711-sk
#11 - at91sam9261-ek
#12 - str912-ek
#13 - iSystemSimulator
#14 - phyCorePXA270
#15 - at91sam7se-ek
#16 - ARMulate ARM926e
#17 - EA_LPC2468_16_OEM
#18 - cm-x270
#19 - at91sam9260-ek
#20 - at91sam9263-ek
#21 - sdk-lh79524-10
#22 - at91sam7a3-ek
#23 - ColibriPXA320
#24 - mcimx27lite
#25 - stm32-p103
#26 - ek-lm3s6965
#27 - trizeps5
#28 - stm3210e-eval
#29 - phyCorei.MX35


ifeq ($(BOARD_SEL),evba7)
override BOARD_SEL := 1
endif

ifeq ($(BOARD_SEL),2294)
override BOARD_SEL := 3
endif

ifeq ($(BOARD_SEL),sam7s)
override BOARD_SEL := 8
endif

ifeq ($(BOARD_SEL),sam7x)
override BOARD_SEL := 9
endif

ifeq ($(BOARD_SEL),str7)
override BOARD_SEL := 10
endif

ifeq ($(BOARD_SEL),sam9)
override BOARD_SEL := 11
endif

ifeq ($(BOARD_SEL),str9)
override BOARD_SEL := 12
endif

ifeq ($(BOARD_SEL),pxa)
override BOARD_SEL := 14
endif

ifeq ($(BOARD_SEL),sam7se)
override BOARD_SEL := 15
endif

ifeq ($(BOARD_SEL),2468)
override BOARD_SEL := 17
endif

ifeq ($(BOARD_SEL),sam9260)
override BOARD_SEL := 19
endif

ifeq ($(BOARD_SEL),sam9263)
override BOARD_SEL := 20
endif

ifeq ($(BOARD_SEL),lh)
override BOARD_SEL := 21
endif

ifeq ($(BOARD_SEL),a3)
override BOARD_SEL := 22
endif

ifeq ($(BOARD_SEL),colibri)
override BOARD_SEL := 23
endif

ifeq ($(BOARD_SEL),imx27lite)
override BOARD_SEL := 24
endif

ifeq ($(BOARD_SEL),ek-lm3s6965)
override BOARD_SEL := 26
endif

ifeq ($(BOARD_SEL),trizeps)
override BOARD_SEL := 27
endif

ifeq ($(BOARD_SEL),stm32)
override BOARD_SEL := 28
endif

ifeq ($(BOARD_SEL),imx35)
override BOARD_SEL := 29
endif

BOARD0=at75c220-dk
CPU0=at75c220

BOARD1=ashling_evba7
CPU1=lpc21xx

BOARD2=gp32
CPU2=s3c2400x

BOARD3=phyCore2294
CPU3=lpc21xx

BOARD4=at91eb40a
CPU4=at91x40

BOARD5=sandgate2
CPU5=pxa270

BOARD6=ARMulate
CPU6=ARM7TDMI

BOARD7=phyCore55800
CPU7=at91m55800

BOARD8=at91sam7s-ek
CPU8=at91sam7

BOARD9=at91sam7x-ek
CPU9=at91sam7

BOARD10=str711-sk
CPU10=str7

BOARD11=at91sam9261-ek
CPU11=at91sam9

BOARD12=str912-sk
CPU12=str9

BOARD13=iSystemSimulator
CPU13=iSystemSimulator

BOARD14=phyCorePXA270
CPU14=pxa270

BOARD15=at91sam7se-ek
CPU15=at91sam7

BOARD16=ARMulate
CPU16=at91sam9

BOARD17=EA_LPC2468_16_OEM
CPU17=lpc24xx_lpc23xx

BOARD18=cm-x270
CPU18=pxa270

BOARD19=at91sam9260-ek
CPU19=at91sam9

BOARD20=at91sam9263-ek
CPU20=at91sam9

BOARD21=sdk-lh79524-10
CPU21=lh7952x

BOARD22=at91sam7a3-ek
CPU22=at91sam7

BOARD23=ColibriPXA320
CPU23=pxa320

BOARD24=mcimx27lite
CPU24=imx27

BOARD25=stm32-p103
CPU25=stm32

BOARD26=ek-lm3s6965
CPU26=stellaris

BOARD27=trizeps5
CPU27=pxa320

BOARD28=stm3210e-eval
CPU28=stm32

BOARD29=phyCorei.MX35
CPU29=imx35

CPU :=$(CPU$(BOARD_SEL))
BOARD :=$(BOARD$(BOARD_SEL))

##########################################

ifneq ($(RVCS),1)

ifeq ($(BOARD_SEL),5)
CPUMODE= -mcpu=xscale
endif

ifeq ($(BOARD_SEL),11)
CPUMODE= -march=armv5te
endif

ifeq ($(BOARD_SEL),12)
CPUMODE= -march=armv5te
endif

ifeq ($(BOARD_SEL),14)
CPUMODE= -mcpu=xscale
endif

ifeq ($(BOARD_SEL),18)
CPUMODE= -mcpu=xscale
endif

ifeq ($(BOARD_SEL),19)
CPUMODE= -march=armv5te
endif

ifeq ($(BOARD_SEL),20)
CPUMODE= -march=armv5te
endif

ifeq ($(BOARD_SEL),23)
CPUMODE= -mcpu=xscale
endif

ifeq ($(BOARD_SEL),24)
CPUMODE= -march=armv5te
endif

ifeq ($(BOARD_SEL),25)
CPUMODE= -mcpu=cortex-m3
CORTEXM3=1
endif

ifeq ($(BOARD_SEL),26)
CPUMODE= -mcpu=cortex-m3
CORTEXM3=1
endif

ifeq ($(BOARD_SEL),27)
CPUMODE= -mcpu=xscale
endif

ifeq ($(BOARD_SEL),28)
CPUMODE= -mcpu=cortex-m3
CORTEXM3=1
endif

ifeq ($(BOARD_SEL),29)
CPUMODE= -mcpu=arm1136jf
endif

CPUMODE ?= -march=armv4t
else

ifeq ($(BOARD_SEL),5)
CPUMODE= --cpu Xscale
endif

ifeq ($(BOARD_SEL),11)
CPUMODE= --cpu ARM926EJ-S
endif

ifeq ($(BOARD_SEL),12)
CPUMODE= --cpu ARM926EJ-S
endif

ifeq ($(BOARD_SEL),14)
CPUMODE= --cpu Xscale
endif

ifeq ($(BOARD_SEL),16)
CPUMODE= --cpu ARM926EJ-S
ARMULATE=1
endif

ifeq ($(BOARD_SEL),18)
CPUMODE= --cpu Xscale
endif

ifeq ($(BOARD_SEL),19)
CPUMODE= --cpu ARM926EJ-S
endif

ifeq ($(BOARD_SEL),23)
CPUMODE= --cpu Xscale
endif

ifeq ($(BOARD_SEL),24)
CPUMODE= --cpu ARM926EJ-S
endif

ifeq ($(BOARD_SEL),27)
CPUMODE= --cpu Xscale
endif

ifeq ($(BOARD_SEL),12)
CPUMODE= --cpu ARM926EJ-S
endif

endif
#######################

ifeq ($(RVCS),1)
ifeq ($(ARMULATE),1)
ifeq ($(TINY),1)
LDSCRIPT ?= $(CPU)-tiny.sct
else
LDSCRIPT ?= $(CPU).sct
endif
else
ifeq ($(TINY),1)
LDSCRIPT ?= $(BOARD)-tiny.sct
else
LDSCRIPT ?= $(BOARD).sct
endif
endif

else

ifeq ($(TINY),1)
LDSCRIPT ?= $(BOARD)-tiny.ld
else
LDSCRIPT ?= $(BOARD).ld
endif

endif
