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
# Makefile include for using LPC177x_8x CMSIS
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

CM3_LPC = 177x_8x

include $(BUILD)/lpc17xx.mk

