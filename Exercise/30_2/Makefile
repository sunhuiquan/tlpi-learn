CC = cc
CFLAGS = -g -Wall -pthread

OBJ = test.o tsb_tree.o

all: test

tsb_tree.o: tsb_tree.c tsb_tree.h
test.o: test.c tsb_tree.h

test: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test

clean:
	rm -fv *.o test
