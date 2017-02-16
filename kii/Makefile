LIBS = -lkiisdk -lssl -lcrypto -lpthread
LIBDIRS = -L. -L/usr/arm-linux-gnueabi/lib
INCLUDES = -I. -I/usr/arm-linux-gnueabi/include/openssl
SOURCES = kii_upload.c kii_core_secure_socket.c kii_init_impl.c kii_socket_impl.c kii_task_impl.c
CC = arm-linux-gnueabi-gcc

build:
	$(CC) -fPIC -shared $(LIBS) $(LIBDIRS) $(INCLUDES) $(SOURCES) -o libkiiuploader.so
