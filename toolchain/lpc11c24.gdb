target remote localhost:2331
# LPC11C24 only supports the SWD interface
monitor interface swd
monitor speed auto
monitor endian little
monitor flash device = LPC11C24
monitor flash download = 1
monitor flash breakpoints = 0
# For LPC11xx cores reset stategy 4 should be used because
# of the bootloader
monitor reset 4
load
monitor clrbp
monitor reset 4
monitor reg r13 = (0) # Stack pointer
monitor reg r15 = (4) # Program counter
