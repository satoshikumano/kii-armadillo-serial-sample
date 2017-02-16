build:
	arm-linux-gnueabi-gcc serial.c -o serial-sample

transfer:
	lftp -u ftp, 10.5.250.103 -e "cd pub; put serial-sample; put exitfail.h;quit"
