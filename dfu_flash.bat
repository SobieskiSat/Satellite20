dfu-util -a 0 -s 0x08000000:leave -D 	Debug/Satellite20.bin
ping -n 2 127.0.0.1 >nul
putty -serial COM8