# -*-makefile-*-
# $ID: S04191BS1
# $Revision: 1.8 $
#
# Common rules for all GNU targets
#

export CFLAGS += -g -Wall -Wstrict-prototypes -Wpointer-arith -Wwrite-strings -Winline
export CFLAGS += -fno-strict-aliasing -ffunction-sections
#export CFLAGS += -std=c99 -pedantic 
export ARFLAGS = rPcou
INC += -I$(SCIOPTA_HOME)/include/hccfatfs

export SUBDIR=gnu
export OBJEXT=o
export O0= -O0
export OSIZE= -Os
export OSPEED= -O3

export LIBSFS0=libsfs_0.a
export LIBSFS1=libsfs_1.a
export LIBSFS2=libsfs_2.a
export LIBSFS=libsfs_1.a

export LIBSFS0SC=libsfs_0t.a
export LIBSFS1SC=libsfs_1t.a
export LIBSFS2SC=libsfs_2t.a
export LIBSFSSC=libsfs_1t.a
