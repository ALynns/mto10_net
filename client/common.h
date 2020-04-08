#ifndef __COMMON__H
#define __COMMON__H

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/file.h>  
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/errno.h>
#include <arpa/inet.h>

#include "md5.h"

#endif

int getMD5(unsigned char *dest,unsigned char *src);
void setTimer(int s_val,int us_val,int s_interval,int us_interval);
int packCreate(char *buf, char *tp, char *tp_value);
int packLength(char *buf);
int getVar(char *opt, char *dest, char *src);
