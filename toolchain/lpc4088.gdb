target remote localhost:2331
monitor interface jtag
monitor speed auto
monitor reset 0
monitor endian little
monitor flash device = LPC4088
monitor flash download = 1
monitor flash breakpoints = 1
load
monitor clrbp
define reset
	monitor reset 0
	monitor reg r13 = (0)
	monitor reg pc = (4)
end
reset
