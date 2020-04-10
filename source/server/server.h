
#ifndef __CLIENT__H
#define __CLIENT__H

#include "common.h"

#endif

//运行参数
#define ARGVNUM 2

//运行状态
#define TIMEOUT -1

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

//包类型、游戏状态
#define PARAMETERAUTHENTICATE 1
#define SECURITYSTRING 2
#define GAMESTART 3
#define MERGESUCCEEDED 4
#define MERGEFAILED 5
#define GAMEOVER 6
#define COORDINATE 7

typedef struct NetInfo
{
    int localSocketfd;
    char serverAddr[16];
    int port;
    MYSQL *conn_ptr;
} NetInfo;

typedef struct UserConnect{
    int socketfd;
    int gameMode;
    char stu_no[8];
    char secString[41];
    int gameStatus;
    int round;
    int step;
    int row;
    int col;
    int mapid;
    char oldMap[(MAXCOLNUM+2)*(MAXROWNUM+2)];
    char newMap[(MAXCOLNUM+2)*(MAXROWNUM+2)];
    int score;
    int maxValue;
    int delay;
    struct timeval tv_begin;
    struct timeval tv_end;
}UserConnect;

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

int getArg(int argc,char *argv[],char *serverAddr,int *port);
int hostBind(char *serverAddr, int port);
int packAnalysis(char *buf, int *packType, void *pack);
int connectClose(UserConnect destCon);
int clientConnect(NetInfo *netif, UserConnect *userConnect);
int clientAccept(int serverSocketfd);
int login(NetInfo *netif, UserConnect *destCon);
int secPackSend(UserConnect *descCon);
int dataSend(UserConnect uCon, int bufSize, char *sendBuf);
int dataRecv(UserConnect uCon, int bufSize, char *recvBuf, int delay);


int mysqlInit(NetInfo *netif);
int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[]);
int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[]);
int mysqlInsert(MYSQL *conn_ptr, const char *tableName, const char *opt);

int gamePro(NetInfo *netif, UserConnect *destCon);
int gameInit(UserConnect *destCon,int matrix[][MAXCOLNUM+2]);
int gamePack(UserConnect destCon);
int mapInit(int matrix[][MAXCOLNUM+2]);
int mapFill(int matrix[][MAXCOLNUM+2],int row,int col,int maxNum);
int mapStr(int matrix[][MAXCOLNUM+2],int row,int col,char *map);
int matrixPrintf(int matrix[][MAXCOLNUM+2],int row,int col);
int matrixRemove(int matrix[][MAXCOLNUM + 2], int x, int y, int num, int flag,int *maxValue);
int matrixFall(int matrix[][MAXCOLNUM + 2],int row,int col);
int gameOver(int matrix[][MAXCOLNUM + 2],int row,int col);

int logWrite(const char *buf, int type, int mode);
