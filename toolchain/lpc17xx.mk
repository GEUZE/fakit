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
# Makefile include for using LPC17xx CMSIS
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

include $(BUILD)/cm3.mk

CM3_LPC ?= 17xx

CM3_CMSIS ?= $(BUILD)/../toolchain/lpc$(CM3_LPC)/Core/CM3
CM3_INCLUDES = -I$(CM3_CMSIS)/CoreSupport -I$(CM3_CMSIS)/DeviceSupport/NXP/LPC$(CM3_LPC) -I$(CM3_DRIVER_DIR)/include
CM3_SOURCES += $(CM3_CMSIS)/DeviceSupport/NXP/LPC$(CM3_LPC)/startup/gcc/startup_LPC$(CM3_LPC).s

