# -*-makefile-*-
# $ID: S04075BS3
# $Revision: 1.13 $
#
# Common rules for all GNU targets
#

export CFLAGS += -g -Wall -Wpointer-arith -Wwrite-strings -Winline
ifneq ($(AR),m6811-elf-ar)
export CFLAGS += -Wextra
endif
ifeq ($(findstring g++,$(CC)),)
export CFLAGS += -std=c99 -Wstrict-prototypes 
endif
export CFLAGS += -pedantic -fno-strict-aliasing
export ARFLAGS = rPcous
export SUBDIR=gnu
export OBJEXT=o
export O0= -O0
export OSIZE= -Os
export OSPEED= -O3

export LIBUTIL0=libutil_0.a
export LIBUTIL1=libutil_1.a
export LIBUTIL2=libutil_2.a
export LIBUTIL=libutil_1.a

export LIBSH0=libsh_0.a
export LIBSH1=libsh_1.a
export LIBSH2=libsh_2.a
export LIBSH=libsh_1.a

export LIBUTIL0SC=libutil_0t.a
export LIBUTIL1SC=libutil_1t.a
export LIBUTIL2SC=libutil_2t.a
export LIBUTILSC=libutil_1t.a

export LIBSH0SC=libsh_0t.a
export LIBSH1SC=libsh_1t.a
export LIBSH2SC=libsh_2t.a
export LIBSHSC=libsh_1t.a
