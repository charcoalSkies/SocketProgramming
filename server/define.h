#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERV_PORT 4633
#define BACKLOG 10

#define INIT_MSG "======================================\nRF-Radar Data Sharing Test Server\n======================================\n"

#define BUFFSIZE 512
#define BUFFID 20
#define BUFFPW 20

#define CLNT_MAX 10

#endif