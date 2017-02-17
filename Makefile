LIBS = -lkiisdk -lkiiuploader -lpthread -lssl -lcrypto
LIBDIRS = -L./kii -L/usr/arm-linux-gnueabi/lib
INCLUDES = -I./kii -I/usr/arm-linux-gnueabi/include/openssl
SOURCES = serial.c
CC = arm-linux-gnueabi-gcc

build:
	$(CC) $(SOURCES) $(LIBS) $(LIBDIRS) $(INCLUDES) -o serial-sample

transfer:
	lftp -u ftp, 10.5.250.103 -e "cd pub; put serial-sample; put exitfail.h;quit"

transfer-kii:
	cd kii && \
	lftp -u ftp, 10.5.250.103 -e "mkdir -p pub/kii; cd pub/kii; put libkiisdk.so; put libkiiuploader.so; put kii.h; put kii_upload.h; quit"
