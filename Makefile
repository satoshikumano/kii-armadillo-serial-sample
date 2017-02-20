LIBS = -lkiisdk -lkiiuploader -lpthread -lssl -lcrypto
LIBDIRS = -L./kii
ARM_LIBDIRS = -L/usr/arm-linux-gnueabi/lib -L./kii/libs/arm
AMD64_LIBDIRS = -L/usr/lib/x86_64-linux-gnu -L./kii/libs/amd64

INCLUDES = -I./kii
ARM_INCLUDES = -I/usr/arm-linux-gnueabi/include/openssl
AMD64_INCLUDES = -I/usr/include/openssl

SOURCES = $(wildcard *.c)
ARM_CC = arm-linux-gnueabi-gcc
AMD64_CC = gcc

TARGET = serial-sample
TARGET_IP = 10.5.250.103

all: transfer
build-arm:
	$(MAKE) -C kii $@
	$(ARM_CC) $(SOURCES) $(LIBS) $(LIBDIRS) $(ARM_LIBDIRS) $(INCLUDES) $(ARM_INCLUDES) -o $(TARGET)

build-amd64:
	$(MAKE) -C kii $@
	$(AMD64_CC) $(SOURCES) $(LIBS) $(LIBDIRS) $(AMD64_LIBDIRS) $(INCLUDES) $(AMD64_INCLUDES) -o $(TARGET)

transfer: build-arm transfer-app transfer-kii

transfer-app:
	lftp -u ftp, $(TARGET_IP) -e "cd pub; put $(TARGET); put exitfail.h;quit"

transfer-kii:
	cd kii && \
	lftp -u ftp, $(TARGET_IP) -e "mkdir -p pub/kii; cd pub/kii; put libs/arm/libkiisdk.so; put libkiiuploader.so; put kii.h; put kii_upload.h; quit"
