# -*-makefile-*-
# $ID: S04075BS1
# $Revision: 1.17 $
#
# Common rules for all GNU targets
#

export _CFLAGS += -g -Wall -Wpointer-arith -Wwrite-strings -Winline
ifneq ($(AR),m6811-elf-ar)
export _CFLAGS += -Wextra
endif
ifneq ($(findstring g++,$(CC)),g++)

export _CFLAGS += -std=c99 -Wstrict-prototypes 
endif
export _CFLAGS += -pedantic -fno-strict-aliasing

export ARFLAGS = rPcous

export SUBDIR=gnu
export OBJEXT=o
export O0= -O0
export OSIZE= -Os
export OSPEED= -O3

export LIBGDD=libgdd_1.a
export LIBGDD0=libgdd_0.a
export LIBGDD1=libgdd_1.a
export LIBGDD2=libgdd_2.a

export LIBGDDSC=libgdd_1t.a
export LIBGDD0SC=libgdd_0t.a
export LIBGDD1SC=libgdd_1t.a
export LIBGDD2SC=libgdd_2t.a
