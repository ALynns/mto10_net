#ifndef __CLIENT__H
#define __CLIENT__H

#include "../common/common.h"

#endif

//运行参数
#define ARGVNUM 11

//游戏模式
#define HELPMODE 0
#define BASEMODE 1
#define COMPMODE 2

//单步模式
#define STEPPINGMODE_OFF 0
#define STEPPINGMODE_ON 1

//账号参数
#define IDLENGTH 7
#define PWDLENGTH 32
#define COMPWDLENGTH 32

#define IPADDRLENGTH 16

#define MAXROWNUM 8
#define MINROWNUM 5
#define MAXCOLNUM 10
#define MINCOLNUM 5

//包类型
#define PARAMETERAUTHENTICATE 1
#define SECURITYSTRING 2
#define GAMEPROGRESS 3
#define COORDINATE 4

typedef struct userConnect{
    int socketfd;
    int row;
    int col;
    int mapid;
    int map;
    int score;
    int gameMode;
    int state;
    int outTime;
}userConnect;

typedef struct ParameterAuthenticatePack{
    char MD5[81];
    int row;
    int col;
    int gameID;
    int delay;
}ParameterAuthenticatePack;

typedef struct CoordinatePack{
    int row;
    int col;
}CoordinatePack;

int hostBind(char *IPAddr, char *port);
int packAnalysis(char *buf, int *packType, void *pack);
