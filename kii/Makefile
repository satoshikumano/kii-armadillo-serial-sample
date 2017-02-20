LIBS = -lkiisdk -lssl -lcrypto -lpthread
ARM_LIBDIRS = -L/usr/arm-linux-gnueabi/lib -L./libs/arm
AMD64_LIBDIRS = -L/usr/lib/x86_64-linux-gnu -L./libs/amd64

INCLUDES = -I.
ARM_INCLUDES = -I/usr/arm-linux-gnueabi/include/openssl
AMD64_INCLUDES = -I/usr/include/openssl

SOURCES = $(wildcard *.c)

ARM_CC = arm-linux-gnueabi-gcc
AMD64_CC = gcc

TARGET = libkiiuploader.so

build-arm:
	$(ARM_CC) -fPIC -shared $(LIBS) $(ARM_LIBDIRS) $(INCLUDES) $(ARM_INCLUDES) $(SOURCES) -o $(TARGET)

build-amd64:
	$(AMD64_CC) -fPIC -shared $(LIBS) $(AMD64_LIBDIRS) $(INCLUDES) $(AMD64_INCLUDES) $(SOURCES) -o $(TARGET)
