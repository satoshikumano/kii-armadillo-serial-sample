LIBS = -lkiisdk -lssl -lcrypto -lpthread
LIBDIRS = -L.
ARM_LIBDIRS = -L/usr/arm-linux-gnueabi/lib
AMD64_LIBDIRS = -L/usr/lib/x86_64-linux-gnu

INCLUDES = -I.
ARM_INCLUDES = -I/usr/arm-linux-gnueabi/include/openssl
AMD64_INCLUDES = -I/usr/include/openssl

SOURCES = $(wildcard *.c)

ARM_CC = arm-linux-gnueabi-gcc
AMD64_CC = gcc

TARGET = libkiiuploader.so

build-arm:
	$(ARM_CC) -fPIC -shared $(LIBS) $(LIBDIRS) $(ARM_LIBDIRS) $(INCLUDES) $(ARM_INCLUDES) $(SOURCES) -o $(TARGET)

build-amd64:
	$(AMD64_CC) -fPIC -shared $(LIBS) $(LIBDIRS) $(AMD64_LIBDIRS) $(INCLUDES) $(AMD64_INCLUDES) $(SOURCES) -o $(TARGET)
