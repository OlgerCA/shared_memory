CC=gcc
CFLAGS= -std=gnu99 -g -Wl,--export-dynamic
LIBS = -lm -pthread
CHECK=-Wall -Wextra

all:libcommon.so

libcommon.so: Message.c Semaphore.c SharedBuffer.c
	$(CC) -shared -fpic Message.c Semaphore.c SharedBuffer.c -o libcommon.so
