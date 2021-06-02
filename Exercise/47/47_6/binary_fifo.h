#ifndef BINARY_FIFO_H
#define BINARY_FIFO_H

int reserve(const char *fifo);

int release(const char *fifo);

int reserveNB(const char *fifo);

#endif