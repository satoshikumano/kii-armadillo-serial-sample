LIBS = -lkiisdk -lssl -lcrypto -lpthread
LIBDIRS = -L. -L/usr/arm-linux-gnueabi/lib
INCLUDES = -I. -I/usr/arm-linux-gnueabi/include/openssl
SOURCES = kii_core_secure_socket.c example.c kii_init_impl.c kii_socket_impl.c kii_task_impl.c

build:
	arm-linux-gnueabi-gcc $(LIBS) $(LIBDIRS) $(INCLUDES) $(SOURCES)
