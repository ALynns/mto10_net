#ifndef __CLIENT__H
#define __CLIENT__H

#include "../common/common.h"

#endif

//���в���
#define ARGVNUM 11

//��Ϸģʽ
#define HELPMODE 0
#define BASEMODE 1
#define COMPMODE 2

//����ģʽ
#define STEPPINGMODE_OFF 0
#define STEPPINGMODE_ON 1

//�˺Ų���
#define IDLENGTH 7
#define PWDLENGTH 32
#define COMPWDLENGTH 32

#define IPADDRLENGTH 16

#define MAXROWNUM 8
#define MINROWNUM 5
#define MAXCOLNUM 10
#define MINCOLNUM 5

typedef struct userConnect{
    int socketfd;
    int row;
    int col;
    int map;
    int 
}userConnect;