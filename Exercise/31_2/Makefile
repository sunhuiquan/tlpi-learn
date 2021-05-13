CC = gcc
CFLAGS = -g -Wall

OBJ = ts_dirname.o test.o
TLS_OBJ = tls_dirname.o tls_test.o

all: test

ts_dirname.o: ts_dirname.h ts_dirname.c
test.o: ts_dirname.h test.c

tls_dirname.o: ts_dirname.h ts_dirname.c
	gcc -g -Wall -D THREAD_LOCAL_STORGE ts_dirname.c -c -o tls_dirname.o
tls_test.o: ts_dirname.h test.c
	gcc -g -Wall -D THREAD_LOCAL_STORGE test.c -c -o tls_test.o


test: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test -ltlpi -pthread
	./test

tls_test: $(TLS_OBJ)
	$(CC) $(CFLAGS) $(TLS_OBJ) -o test_tls -ltlpi -pthread
	./test_tls

rm:
	rm -fv *.o test test_tls
