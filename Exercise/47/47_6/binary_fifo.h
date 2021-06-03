#ifndef BINARY_FIFO_H
#define BINARY_FIFO_H

int init(const char *fifo, int *rfd, int *wfd);

int reserve(int rfd);

int reserveNB(int rfd);

int release(int wfd);

#endif