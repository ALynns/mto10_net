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

typedef struct GameInfo
{
    int gameMode;
    char *stuNo;
    char *stuPasswd;
    int mapid;
    int row;
    int col;
    int stepMode;
} GameInfo;

typedef struct NetInfo
{
    char *serverAddr;
    int port;
    int timeOut;
    int socketfd;
} NetInfo;



int getArg(int argc,char *argv[],int *gameMode,char *ipAddr,int *port,char *stuNo,char *stuPasswd,int *mapid,int *row,int *col,int *timeOut,int *stepMode);
int localBind(NetInfo* netif);
int login(GameInfo gmif,NetInfo netif);
void dataSend(int socketfd,int sendBufSize,char *sendBuf);
void dataRecv(int signo);
int readLine(char *buf);
int gamePro(GameInfo gmif, NetInfo netif);
int matrixReload(char **matrix,int row,int col,char *newMatrix);