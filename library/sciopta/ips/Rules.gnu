# -*-makefile-*-
# $ID: S04075BS2
# $Revision: 2.20 $
#
# Common rules for all GNU targets
#

export _CFLAGS += -g -Wall -Wstrict-prototypes -Wpointer-arith -Wwrite-strings -Winline
export _CFLAGS += -std=c99 -pedantic -fno-strict-aliasing
export _CFLAGS +=-I$(IPS_ROOT)
ifneq ($(AR),m6811-elf-ar)
export _CFLAGS += -Wextra
endif
export ARFLAGS = rPcous

export SUBDIR=gnu
export OBJEXT=o
export LIBEXT=a
export O0= -O0
export OSIZE= -Os
export OSPEED= -O3

export LIBIPS=libips
export LIBDHCP=libdhcp
export LIBTELNET=libtelnet
export LIBTFTP=libtftp
export LIBFTP=libftp
export LIBSNMP=libsnmp
export LIBSMTP=libsmtp
export LIBDNS=libdns
export LIBHTTP=libhttp
export LIBTEST=libipstst
