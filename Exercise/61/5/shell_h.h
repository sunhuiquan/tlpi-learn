#ifndef SHELL_H_H
#define SHELL_H_H

#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define PORT_NUMBER 9990
#define HOST_ADDRESS "127.0.0.1"
#define MAXLINE 1024

int pr_sockname_inet4(int sfd, struct sockaddr_in *addr);

#endif