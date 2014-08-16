#   _____   _    _  _______  __  _  _
#  |  __ \ | |  | ||__   __|/_ || || |
#  | |  | || |  | |   | |    | || || |_
#  | |  | || |  | |   | |    | ||__   _|
#  | |__| || |__| |   | |    | |   | |
#  |_____/  \____/    |_|    |_|   |_|
#
# Delft University of Technology
# Racing Team 2013-2014
#------------------------------------------
# Makefile include for Cortex M3
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

CXX = arm-none-eabi-g++
CC  = arm-none-eabi-gcc
GDB = arm-none-eabi-gdb

CM3_CXXFLAGS = -fno-exceptions -mcpu=cortex-m3 -mthumb
CM3_CCFLAGS  = -fno-exceptions -mcpu=cortex-m3 -mthumb
CM3_LDFLAGS  = -nostdlib -T$(BUILD)/cm3.ld

