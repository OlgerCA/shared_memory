CC=gcc
CFLAGS= -std=gnu99 -g -Wl,--export-dynamic
COMMON = -Icommon
LIBS = -pthread -lm
CHECK=-Wall -Wextra

all: common_target initializer_target producer_target consumer_target finalizer_target

common_target:
	(cd common; make)

initializer_target: initializer/main.c common/libcommon.so
	$(CC) $(COMMON) $(CFLAGS) -o initializer_executable initializer/main.c common/libcommon.so $(LIBS)

consumer_target: consumer/main.c common/libcommon.so
	$(CC) $(COMMON) $(CFLAGS) -o consumer_executable consumer/main.c common/libcommon.so $(LIBS)

producer_target: producer/main.c common/libcommon.so
	$(CC) $(COMMON) $(CFLAGS) -o producer_executable producer/main.c common/libcommon.so $(LIBS)

finalizer_target: finalizer/main.c common/libcommon.so
	$(CC) $(COMMON) $(CFLAGS) -o finalizer_executable finalizer/main.c common/libcommon.so $(LIBS)

clean:
	rm -f **/*.o initializer_executable consumer_executable producer_executable finalizer_executable common/libcommon.so
