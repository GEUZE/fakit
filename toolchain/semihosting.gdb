set pagination off
break _write
commands
silent
set $_write_b = (char*)buf
set $_write_c = length
while ($_write_c)
	printf "%c", *$_write_b
	set $_write_b++
	set $_write_c--
end
continue
end
