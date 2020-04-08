#include "client.h"
#include <stdarg.h>

#define GLOBALBUFSIZE 512*1024

int rdP = 0;
int wtP = 0;
char recvBufGlobal[GLOBALBUFSIZE];

GameInfo gmif;
NetInfo netif;

int main(int argc,char *argv[])
{
    netif.serverAddr=(char *)malloc(16);
    gmif.stuNo=(char *)malloc(IDLENGTH);
    gmif.stuPasswd=(char *)malloc(PWDLENGTH);
    
    getArg(argc,argv,&gmif.gameMode,netif.serverAddr,&netif.port,gmif.stuNo,gmif.stuPasswd,&gmif.mapid,&gmif.row,&gmif.col,&netif.delay,&gmif.stepMode);
    //printf("%d\n%s\n%d\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n",gmif.gameMode,netif.serverAddr,netif.port,gmif.stuNo,gmif.stuPasswd,gmif.mapid,gmif.row,gmif.col,netif.delay,gmif.stepMode);
    
    if(gmif.gameMode==HELPMODE)
    {
        return 0;
    }
    localBind(&netif);

    
    signal(SIGALRM, dataRecv);
    setTimer(0,1000,0,1000);
    
    login(gmif,netif);
    gamePro(&gmif,&netif);

    free(netif.serverAddr);
    free(gmif.stuNo);
    free(gmif.stuPasswd);   
    return 0;
}

int getArg(int argc,char *argv[],int *gameMode,char *serverAddr,int *port,char *stuNo,char *stuPasswd,int *mapid,int *row,int *col,int *delay,int *stepMode)
{
    const char *argvList[ARGVNUM]={"--base","--competition","--ipaddr","--port","--stuno","--passwd","--mapid","--row","--col","--timeout","--stepping"};

    //默认游戏模式

    //默认IP地址和端口号
    strcpy(serverAddr,"127.0.0.1");
    //strcpy(serverAddr,"10.60.102.252");
    (*port)=21345;

    //默认账号
    strcpy(stuPasswd,"0HhJ)j8JGx+3uq.#");

    //其他参数
    (*mapid)=-1;
    (*row)=-1;
    (*col)=-1;
    (*delay)=5;
    (*stepMode)=STEPPINGMODE_OFF;

    if(argc<1)
    {
        (*gameMode)=HELPMODE;
        return 0;
    }    

    int i, j;
    for (i = 1; i < argc; ++i)
    {
        for (j = 0; j < ARGVNUM; ++j)
            if (!strcmp(argv[i], argvList[j]))
            {
                switch (j)
                {
                    case 0:
                    {
                        (*gameMode)=BASEMODE;
                        sprintf(stuNo, "%d",  1234567);
                        break;
                    }
                    case 1:
                    {
                        (*gameMode)=COMPMODE;
                        sprintf(stuNo, "%d",  7654321);
                        break;
                    }
                    case 2:
                    {
                        strcpy(serverAddr,argv[i+1]);
                        break;
                    }
                    case 3:
                    {
                        *port=atoi(argv[i+1]);
                        break;
                    }
                    case 4:
                    {
                        strcpy(stuNo,argv[i+1]);
                        break;
                    }
                    case 5:
                    {
                        strcpy(stuPasswd,argv[i+1]);
                        break;
                    }
                    case 6:
                    {
                        (*mapid)=atoi(argv[i+1]);
                        break;
                    }
                    case 7:
                    {
                        (*row)=atoi(argv[i+1]);
                        break;
                    }
                    case 8:
                    {
                        (*col)=atoi(argv[i+1]);
                        break;
                    }
                    case 9:
                    {
                        (*delay)=atoi(argv[i+1]);
                        break;
                    }
                    case 10:
                    {
                        (*stepMode)=STEPPINGMODE_ON;
                        break;
                    }
                    
                    default:
                        break;
                };
                break;
            }
    }
    srand(time(0));
    if ((*row) == -1)
    {
        (*row) = rand() % 4 + 5;
    }
    if ((*col) == -1)
    {
        (*col) = rand() % 6 + 5;
    }
    return 0;
}

int localBind(NetInfo* netif)
{
    netif->socketfd=0;
    while(netif->socketfd<=0)
        netif->socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    fcntl(netif->socketfd, F_SETFL, fcntl(netif->socketfd, F_GETFL, 0) | O_NONBLOCK);
    fd_set fdsr;

    struct sockaddr_in serviceAddr;
    serviceAddr.sin_family=AF_INET;
    serviceAddr.sin_addr.s_addr=inet_addr(netif->serverAddr);
    serviceAddr.sin_port=htons(netif->port);

    struct sockaddr_in clientAddr;
    memset(&clientAddr,0,sizeof(clientAddr));
    clientAddr.sin_family=AF_INET;
    clientAddr.sin_port=htons(0);

    int reuse = 1;
    setsockopt(netif->socketfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int));

    int ret = -1;
    while (ret < 0)
        ret = bind(netif->socketfd, (struct sockaddr *)&clientAddr, sizeof(clientAddr));

    while(1)
    {
        ret = connect(netif->socketfd, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr)); //客户端用connect与服务器端连接
        if (ret == -1)
        {
            if (errno != EINPROGRESS) //
                continue;
            else
            {
                FD_ZERO(&fdsr);
                FD_SET(netif->socketfd, &fdsr);

                ret = select(netif->socketfd + 1, NULL, &fdsr, NULL, 0);
                if (ret < 0)
                {
                    printf("select error\n");
                    exit(-1);
                }
                else
                {
                    int error = -1, slen = sizeof(int);
                    getsockopt(netif->socketfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&slen);
                    if (error == 0)
                    {
                        //printf("Connection successful\n");
                        ret = 1;
                    }
                    
                    else
                        ret = 0;
                }
            }
        }
        if (ret == 0)
            continue;
        else
            break;
    }
}

int login(GameInfo gmif,NetInfo netif)
{
    char buf[300]={0};
    unsigned char keyString[41];
    unsigned char sKey[41];
    unsigned char key[81];

    keyString[0]=0;
    sKey[0]=0;
    key[0]=0;

    strcat(keyString,gmif.stuNo);
    strcat(keyString,"*");
    getMD5(&keyString[8],gmif.stuPasswd);
    

    while(readLine(buf))
        ;
    while(readLine(buf))
        ;
    getVar(NULL,sKey,buf);

    int i;
    for (i = 0; i < 40; ++i)
    {
        int tmp=keyString[i]^sKey[i];
        int de = tmp / 16, po = tmp % 16;
        key[2 * i] = de  >= 10 ? de - 10 + 'a' : de + '0';
        key[2 * i + 1] = po >= 10 ? po - 10 + 'a' : po + '0';
    }

    buf[0] = 0;

    packCreate(buf,"Type","ParameterAuthenticate");
    packCreate(buf,"MD5",key);

    if(gmif.row == -1)
        packCreate(buf, "Row", "-1");
    else
    {
        char row[3];
        sprintf(row, "%d", gmif.row);
        packCreate(buf, "Row", row);
    }

    if(gmif.col == -1)
        packCreate(buf, "Col", "-1");
    else
    {
        char col[3];
        sprintf(col, "%d", gmif.col);
        packCreate(buf, "Col", col);
    }

    char map[30];
    sprintf(map, "%d", gmif.mapid);
    packCreate(buf, "GameID", map);

    char delay[4];
    sprintf(delay, "%d", netif.delay*1000);
    packCreate(buf, "Delay", delay);

    packLength(buf);

    dataSend(netif.socketfd,strlen(buf),buf);
    printf("%s",buf);

    while(readLine(buf))
    ;
    
}

void dataSend(int socketfd,int sendBufSize,char *sendBuf)
{
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(socketfd, &fdsr);
        ret = select(socketfd + 1, NULL, &fdsr, NULL, 0);
        if (ret < 0 && errno != EINTR)
        {
            continue;
        }
        else
        {
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
        }
    }
    //FD_ISSET(socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
    ret=0;
	while (1)
	{
		ret = send(socketfd, &sendBuf[ret],sendBufSize, 0);
        if(ret>0)
            sendBufSize=sendBufSize-ret;
		if(sendBufSize==0)
            break;
	}
}

void dataRecv(int signo)
{
    static int sendSize;
    int ret;

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while (1)
    {
        FD_ZERO(&fdsr);
        FD_SET(netif.socketfd, &fdsr);
        ret = select(netif.socketfd + 1, &fdsr, NULL, NULL, 0);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error,%d\n", ret);
            exit(-1);
        }
        else
        {
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
            else
                break;
        }
    }
    //FD_ISSET(netif.socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
	while (1)
	{
		ret = recv(netif.socketfd, &(recvBufGlobal[wtP]), GLOBALBUFSIZE, 0);
		if (ret < 0)
			continue;
		else
			break;
	}
    //printf("%s\n",&recvBufGlobal[wtP]);
    if (ret >= 0)
        wtP = wtP + ret;
}

int readLine(char *buf)
{
    int i;
    for (i = 0;; ++i)
    {
        if (rdP + i >= wtP)
            return -1;
        if (recvBufGlobal[rdP + i] == 0x0a)
        {
            strncpy(buf, &recvBufGlobal[rdP], i - 1);
            buf[i-1]=0;
            rdP = rdP + i + 1;
            return 0;
        }
    }
    return 0;
}

int gamePro(GameInfo *gmif, NetInfo *netif)
{
    char lineBuf[100];
    static int matrix[MAXROWNUM + 2][MAXCOLNUM + 2] = {0};

    dataRecv(0);
    
    
    while(readLine(lineBuf)); //Type

    readLine(lineBuf); //content
    printf("%s\n",lineBuf);
      
    gameStart(gmif, netif);

    readLine(lineBuf); //map
    printf("%s\n",lineBuf);
    char newMatrix[MAXCOLNUM * MAXROWNUM] = {0};
    getVar(NULL, newMatrix, lineBuf);
    matrixReload(matrix, gmif->row, gmif->col, newMatrix);
    matrixPrintf(matrix,gmif->row, gmif->col);

    readLine(lineBuf); //length

    int lastRow,lastCol;

    pointChoose(matrix,*gmif,1,1,0,&lastRow,&lastCol);
    printf("本次选择坐标:Row=%c,Col=%d\n",lastRow-1+'A',lastCol-1);

    char packBuf[200]={0};
    gamePackCreate(lastRow,lastCol-1,packBuf);
    dataSend(netif->socketfd,strlen(packBuf),packBuf);
    
    
    while (1)
    {
        dataRecv(0);
        int endFlag=0;
        int sucFlag=1;
        char content[50];

        while(readLine(lineBuf)); //Type

        readLine(lineBuf); //content
        printf("%s\n",lineBuf);
        getVar(NULL, content, lineBuf);
        if (!strcmp(content, "GameOver")||!strcmp(content, "GameTimeout"))
        {
            endFlag=1;
        }
        if(!strcmp(content, "MergeFailed"))
        {
            sucFlag=0;
        }

        readLine(lineBuf);//gameid
        printf("%s\n",lineBuf);

        readLine(lineBuf);//step
        printf("%s\n",lineBuf);

        readLine(lineBuf);//score
        printf("%s\n",lineBuf);
        
        readLine(lineBuf);//maxvalue
        printf("%s\n",lineBuf);
        

        readLine(lineBuf);//oldmap
        if(sucFlag)
            printf("%s\n",lineBuf);
        
        if(!endFlag)
        {
            if(sucFlag)
            {
                readLine(lineBuf);//newmap
                printf("%s\n",lineBuf);
                getVar(NULL, newMatrix, lineBuf);
                matrixReload(matrix, gmif->row, gmif->col, newMatrix);
                matrixPrintf(matrix,gmif->row, gmif->col);
            }
        }
        else
        {
            if(sucFlag)
            {
                getVar(NULL, newMatrix, lineBuf);
                matrixReload(matrix, gmif->row, gmif->col, newMatrix);
                matrixPrintf(matrix,gmif->row, gmif->col);
            }
        }
        
        readLine(lineBuf);//length

        if(endFlag==1)
            break;

        pointChoose(matrix,*gmif,1,1,0,&lastRow,&lastCol);
        printf("本次选择坐标:Row=%c,Col=%d\n",lastRow-1+'A',lastCol-1);
        gamePackCreate(lastRow,lastCol-1,packBuf);
        dataSend(netif->socketfd,strlen(packBuf),packBuf);
        
    };
}

int matrixReload(int matrix[][MAXCOLNUM+2],int row,int col,char *newMatrix)
{
    int r,c;
    int len=row*col;

    int i = 0;
    for (r = 1; r <= row; ++r)
    {
        for (c = 1; c <= col; ++c)
        {
            matrix[r][c]=newMatrix[i]-'0';
            ++i;
        }
    }    
}

int matrixPrintf(int matrix[][MAXCOLNUM+2],int row,int col)
{
    int r,c;
    printf("\n");
    for(r=1;r<=row;++r)
    {
        for(c=1;c<=col;++c)
        {
            printf("%d ",matrix[r][c]>0?matrix[r][c]:-1*matrix[r][c]);
        }
        printf("\n");
    }
}

int gameStart(GameInfo *gmif, NetInfo *netif)
{
    char lineBuf[100];
    if(rdP<wtP)
        {
            readLine(lineBuf);//row
            char row[3]={0};
            getVar(NULL,row,lineBuf);
            gmif->row=atoi(row);
        }
        if(rdP<wtP)
        {
            readLine(lineBuf);//col
            char col[3]={0};
            getVar(NULL,col,lineBuf);
            gmif->col=atoi(col);
        }    
        if(rdP<wtP)
        {
            readLine(lineBuf);//gameid
            char mapid[30]={0};
            getVar(NULL,mapid,lineBuf);
            gmif->mapid=atoi(mapid);
        }    

        if(rdP<wtP)
        {
            readLine(lineBuf);//delay
            char delay[6]={0};
            getVar(NULL,delay,lineBuf);
            netif->delay=atoi(delay);
        }    
}

int pointChoose(int matrix[][MAXCOLNUM+2],GameInfo gmif,int row,int col,int flag,int *lastRow,int *lastCol)
{
    static int maxRow=0,maxCol=0,maxSoc=0;
    static int curDigit=0,curNum=0,curSoc=0;

    if(matrix[row][col]==0)//边缘
        return 0;
    
    
    if(flag==0)
    {
        maxRow=0;
        maxCol=0;
        maxSoc=100;
        curDigit=0;
        curNum=0;
        curSoc=0;

        int r,c;
        for(r=1;r<=gmif.row;++r)
            for(c=1;c<=gmif.col;++c)
            {
                if(matrix[r][c]<=0)
                    continue;
                curDigit=matrix[r][c];
                curNum=0;
                curSoc=0;
                pointChoose(matrix,gmif,r,c,1,NULL,NULL);
                curSoc=curDigit;
                if(curSoc<maxSoc&&curNum>1)
                {
                    maxSoc=curSoc;
                    maxRow=r;
                    maxCol=c;
                }    
            }
        (*lastRow)=maxRow;
        (*lastCol)=maxCol;
        return 0;
    }
    else
    {
        if(matrix[row][col]==curDigit)
        {
            matrix[row][col]=matrix[row][col]*(-1);
            curNum++;
            pointChoose(matrix,gmif,row-1,col,1,NULL,NULL);
            pointChoose(matrix,gmif,row+1,col,1,NULL,NULL);
            pointChoose(matrix,gmif,row,col-1,1,NULL,NULL);
            pointChoose(matrix,gmif,row,col+1,1,NULL,NULL);
        }    
    }

    
}

int gamePackCreate(int row,int col,char *packBuf)
{
    packBuf[0]=0;
    char tmp[5]={0};
    packCreate(packBuf,"Type","Coordinate");

    sprintf(tmp,"%c",row+'A'-1);
    packCreate(packBuf,"Row",tmp);

    sprintf(tmp,"%c",col+'0');
    packCreate(packBuf,"Col",tmp);

    packLength(packBuf);
}




