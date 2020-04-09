#include "server.h"

UserConnect u_con;
NetInfo netif;

int main(int argc,char *argv[])
{
    //daemon(1,1);

    getArg(argc,argv,netif.serverAddr,&netif.port);

    netif.localSocketfd=hostBind(netif.serverAddr,netif.port);

    mysqlInit(&netif);
    
    clientConnect(&netif, &u_con);

}

int getArg(int argc,char *argv[],char *serverAddr,int *port)
{
    const char *argvList[ARGVNUM]={"--ipaddr","--port"};

    strcpy(serverAddr,"127.0.0.1");//strcpy(serverAddr,"10.60.102.252");
    (*port)=21345;

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
                        strcpy(serverAddr,argv[i+1]);
                        break;
                    }
                    case 1:
                    {
                        (*port)=atoi(argv[i+1]);
                        break;
                    }
                    default:
                        break;
                };
                break;
            }
    }
}

int packAnalysis(char *buf, int *packType, void *pack)
{
    char *tempStr;
    tempStr = strtok(buf, "\r\n");//获取Type行信息
    if(tempStr!=NULL)
    {
        tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
        if(!strcmp(tempStr,"ParameterAuthenticate"))
        {
            (*packType)=PARAMETERAUTHENTICATE;
        }
        if(!strcmp(tempStr,"Coordinate"))
        {
            (*packType)=COORDINATE;
        }
    }

    switch(*packType)
    {
        case PARAMETERAUTHENTICATE:
        {
            tempStr = strtok(NULL, "\r\n");//获取MD5
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            strcpy(((ParameterAuthenticatePack *)pack)->MD5,tempStr);
            tempStr = strtok(NULL, "\r\n");//获取Row
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            ((ParameterAuthenticatePack *)pack)->row=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//获取Col
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            ((ParameterAuthenticatePack *)pack)->col=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//获取GameID
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            ((ParameterAuthenticatePack *)pack)->gameID=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//获取delay
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            ((ParameterAuthenticatePack *)pack)->delay=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//长度
            break;
        }
        case COORDINATE :
        {
            tempStr = strtok(NULL, "\r\n");//获取Row
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            tempStr[0] = tempStr[0] - 'A' + '1';
            ((CoordinatePack *)pack)->row = atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//获取Col
            tempStr=(char*)memchr(tempStr, '=', strlen(tempStr))+2;
            ((CoordinatePack *)pack)->col = atoi(tempStr) + 1;
            tempStr = strtok(NULL, "\r\n");//长度
            
            break;
        }
    }
}

int hostBind(char *IPAddr,int port)
{
    struct sockaddr_in serviceAddr;
    int ret;
    memset(&serviceAddr,0,sizeof(serviceAddr));//由于该结构体中存在保留位，需要置0
    serviceAddr.sin_family=AF_INET;//定义地址族类型，AF_INET为IPv4，AF_INET6为IPv6
    serviceAddr.sin_port = htons(port);
    if(IPAddr!=NULL)
        serviceAddr.sin_addr.s_addr = inet_addr(IPAddr);      //inet_addr("127.0.0.1");
    int socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Linux中定义socket为一个文件，返回值为一个文件描述符
    if(socketfd<0)
    {
        printf("socket error\n");
        exit(-1);
    }
    
    fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);

    int reuse=1;
    setsockopt(socketfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)& reuse,sizeof(int));
    
    ret=bind(socketfd, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr));//绑定端口
    if (ret < 0)
    {
        printf("bind error\n");
        exit(-1);
    }
    ret = listen(socketfd, SOMAXCONN);
    if (ret < 0) //listen()函数使套接字进入被动监听状态，SOMAXCONN为缓冲区长度最大
    {
        printf("listen error\n");
        exit(-1);
    }
    return socketfd;
}

int dataRecv(UserConnect uCon, int bufSize, char *recvBuf, int delay)
{
    int ret;

    fd_set fdsr;

    struct timeval tv;
    while(1)
    {
        FD_ZERO(&fdsr);

        FD_SET(uCon.socketfd, &fdsr);
        if(!delay)
            ret = select(uCon.socketfd + 1, &fdsr, NULL, NULL, NULL);
        else
        {
            tv.tv_sec = delay / 1000;
            ret = select(uCon.socketfd + 1, &fdsr, NULL, NULL, &tv);
        }    
        
        if (ret < 0 )//&& errno != EINTR)
        {
            continue;
        }
        else
        {
            if (ret == 0 && delay != 0)
                return TIMEOUT;
            if (ret > 0)
                break;
            if (errno == EINTR)
                continue;
            else
                break;
            
        }
    }

    //if (FD_ISSET(uCon.socketfd, &fdsr))
    while (1)
    {
        ret = recv(uCon.socketfd, recvBuf, bufSize, 0);
        
        if (ret <= 0)
            continue;
        else
        {
            break;
        }
        
    }
    printf("%s\n",recvBuf);
    return ret;
}

int dataSend(UserConnect uCon, int bufSize, char *sendBuf)
{
    int ret;

    fd_set fdsr;

    while (1)
    {
        FD_ZERO(&fdsr);

        FD_SET(u_con.socketfd, &fdsr);

        struct timeval tv;
        tv.tv_sec = 2;
        ret = select(u_con.socketfd + 1, NULL, &fdsr, NULL, &tv);

        if (ret < 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    //FD_ISSET(socketfd, &fdsr)判断套接字是否就绪，本题仅监控一个描述符可以略过
    int sendSize = 0;
    while(1)
    {
        ret = send(u_con.socketfd, &sendBuf[sendSize], bufSize, 0);
        sendSize = sendSize + ret;
        if (ret < 0)
        {
            continue;
        }
        if (sendSize == bufSize)
            break;
    }
    printf("%s\n",sendBuf);
}

int connectClose(UserConnect destCon)
{
    close(destCon.socketfd);
    return 0;
}

int clientConnect(NetInfo *netif, UserConnect *userConnect)
{
    int ret;

    fd_set fdsr;
    struct timeval tv;//超时时间
    tv.tv_sec = 10;

    while(1)
    {
        FD_ZERO(&fdsr);
        FD_SET(netif->localSocketfd, &fdsr);
        ret = select(netif->localSocketfd + 1, &fdsr, NULL, NULL, NULL);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error\n");
            exit(-1);
        }
        else
        {
            if (errno == EINTR && ret < 0)
                continue;
        }

        //FD_ISSET(serverSocketfd, &fdsr)判断套接字是否就绪，本处仅监控一个描述符可以略过
        if (ret > 0)
        {
            //fork();
            //if (getpid() == 0)
            //{
                clientAccept(netif->localSocketfd);
                if (!login(netif, userConnect))
                {
                    gamePro(netif, userConnect);
                }

                connectClose(u_con);
                exit(-1);
            //}
        }
    }

    return 0;
}

int clientAccept(int serverSocketfd)
{
    u_con.socketfd = accept(serverSocketfd, NULL, NULL); //accept非阻塞
    fcntl(u_con.socketfd, F_SETFL, fcntl(u_con.socketfd, F_GETFL, 0) | O_NONBLOCK);
    secPackSend(&u_con);
    return 0;
}

int login(NetInfo *netif, UserConnect *destCon)
{
    char recvBuf[300] = {0}, keyString[41] = {0}, ***result;
    int p_type;
    ParameterAuthenticatePack p_pack;
    dataRecv(*destCon,500,recvBuf,0);
    packAnalysis(recvBuf,&p_type,&p_pack);

    destCon->row = p_pack.row;
    destCon->col = p_pack.col;
    destCon->mapid = p_pack.gameID;
    destCon->delay = p_pack.delay;

    p_pack.MD5[80] = 0;
    
    int i;
    for (i = 0; i < 40; ++i)
    {
        p_pack.MD5[2 * i] = p_pack.MD5[2 * i] >= 'a' ? p_pack.MD5[2 * i] - 'a' + 10 : p_pack.MD5[2 * i] - '0';
        p_pack.MD5[2 * i + 1] = p_pack.MD5[2 * i + 1] >= 'a' ? p_pack.MD5[2 * i + 1] - 'a' + 10 : p_pack.MD5[2 * i + 1] - '0';
        char tmp = p_pack.MD5[2 * i] * 16 + p_pack.MD5[2 * i + 1];
        keyString[i] = tmp ^ destCon->secString[i];   
    }
    char opt[50] = {0}, stu_no[8] = {0};

    if(keyString[7]=='*')
    {
        destCon->gameMode=BASEMODE;
        strncpy(stu_no, keyString, 7);
    }
    else
    {
        destCon->gameMode=COMPMODE;
        for(i=6;i>=0;--i)
        {
            stu_no[6 - i] = keyString[i];
        }
    }
    

    strcat(opt,"stu_no=");
    strcat(opt,stu_no);
    
    result[0][0]=(char *)malloc(33);
    result[0][0][0]=0;
    mysqlSelect(netif->conn_ptr,"stu_password","student",opt,NULL,NULL,result);
    if(!strcmp(result[0][0],&keyString[8]))
        return 0;
    else
        return -1;
    
}

int secPackSend(UserConnect *descCon)
{
    int i;
    char secPack[100] = {0};
    srand(time(0));
    for (i = 0; i < 40; ++i)
    {
        descCon->secString[i] = rand() % 94 + 33;
    }
    descCon->secString[40] = 0;
    packCreate(secPack, "Type", "SecurityString");
    packCreate(secPack, "Content", descCon->secString);
    packLength(secPack);
    dataSend(*descCon, strlen(secPack), secPack);
}

int gamePro(NetInfo *netif, UserConnect *destCon)
{
    static int matrix[MAXROWNUM + 2][MAXCOLNUM + 2] = {0};
    char packBuf[80];
    int packType;
    CoordinatePack c_pack;
    gameInit(destCon,matrix);
    matrixPrintf(matrix, destCon->row, destCon->col);
    
    gamePack(*destCon);
    
    while(1)
    {
        if(dataRecv(u_con, 80, packBuf, destCon->delay)==TIMEOUT)
        {
            printf("Time out\n");
            exit(-1);
        }

        packAnalysis(packBuf, &packType, &c_pack);

        destCon->score = destCon->score + matrixRemove(matrix, c_pack.col, c_pack.row, matrix[c_pack.row][c_pack.col], 0, &destCon->maxValue);
        destCon->step++;
        matrixPrintf(matrix, destCon->row, destCon->col);

        matrixFall(matrix, destCon->row, destCon->col);
        matrixPrintf(matrix, destCon->row, destCon->col);
        mapStr(matrix, destCon->row, destCon->col, destCon->oldMap);

        mapFill(matrix, destCon->row, destCon->col, destCon->maxValue);
        matrixPrintf(matrix, destCon->row, destCon->col);
        mapStr(matrix, destCon->row, destCon->col, destCon->newMap);

        if(gameOver(matrix,destCon->row,destCon->col))
        {
            destCon->gameStatus = GAMEOVER;
        }

        gamePack(*destCon);
        if(destCon->gameStatus==GAMEOVER)
        {
            if(destCon->gameMode==BASEMODE)
            {

            }
            else
            {
                
            }
            
            break;
        }    
    }
    

}

int mysqlInit(NetInfo *netif)
{
    netif->conn_ptr = mysql_init(NULL);
    netif->conn_ptr = mysql_real_connect(netif->conn_ptr, "127.0.0.1", "u1753935", "u1753935", "hw-mto10-u1753935", 0, NULL, 0);
    if(!netif->conn_ptr)
        return -1;
}

int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[])
{
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;
    int res, r, c;

    res = mysql_query(conn_ptr, optStr); //查询语句

    if (res)
    {
        //printf("SELECT error:%s\n",mysql_error(conn_ptr));查询错误
        return -1;
    }
    else
    {
        res_ptr = mysql_store_result(conn_ptr); //取出结果集
        if (res_ptr && result)
        {
            r  =mysql_num_rows(res_ptr);
            c = mysql_num_fields(res_ptr);
            int i = 0, j = 0;
            while ((sqlrow = mysql_fetch_row(res_ptr)))
            { //依次取出记录

                for (i = 0; i < c; ++i)
                {
                    strcpy(result[j][i], sqlrow[i]);
                }    
                ++j;
            }
            if (mysql_errno(conn_ptr))
            {
                fprintf(stderr, "Retrive error:s\n", mysql_error(conn_ptr));
            }
        }
        mysql_free_result(res_ptr);
        if (row)
            (*row) = r;
        if (col)
            (*col) = c;
    }
}
int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[])
{
    char optStr[200] = {0};
    sprintf(optStr,"select %s from %s ",selectItem,tableName);
    if(opt)
    {
        strcat(optStr, "where ");
        strcat(optStr,opt);
    }    
    strcat(optStr,";");
    mysqlOpt(conn_ptr, optStr, row, col, result);
}

int gameInit(UserConnect *destCon,int matrix[][MAXCOLNUM+2])
{
    destCon->gameStatus = GAMESTART;
    destCon->round = 1;
    destCon->step = 0;
    if (destCon->mapid == -1)
    {
        destCon->mapid=time(0);
        srand(destCon->mapid);
    }
    mapInit(matrix);
    mapFill(matrix, destCon->row, destCon->col, 3);
    mapStr(matrix, destCon->row, destCon->col, destCon->oldMap);
    mapStr(matrix, destCon->row, destCon->col, destCon->newMap);
    destCon->score = 0;
    destCon->maxValue = 3;
    return 0;
}

int gamePack(UserConnect destCon)
{
    char gamePackbuf[500] = {0};
    char temp[300] = {0};
    packCreate(gamePackbuf,"Type","GameProgress");
    switch (destCon.gameStatus)
    {
        case GAMESTART:
        {
            packCreate(gamePackbuf, "Content", "GameStart");

            sprintf(temp,"%d",destCon.row);
            packCreate(gamePackbuf, "Row", temp);

            sprintf(temp,"%d",destCon.col);
            packCreate(gamePackbuf, "Col", temp);

            sprintf(temp,"%d",destCon.mapid);
            packCreate(gamePackbuf, "GameID", temp);

            sprintf(temp,"%d",destCon.delay);
            packCreate(gamePackbuf, "Delay", temp);

            sprintf(temp,"%s",destCon.newMap);
            packCreate(gamePackbuf, "Map", temp);

            break;
        }
        case MERGESUCCEEDED:
        {
            packCreate(gamePackbuf, "Content", "MergeSucceeded");

            sprintf(temp,"%d",destCon.mapid);
            packCreate(gamePackbuf, "GameID", temp);

            sprintf(temp,"%d",destCon.step);
            packCreate(gamePackbuf, "Step", temp);
            
            sprintf(temp,"%d",destCon.score);
            packCreate(gamePackbuf, "Score", temp);

            sprintf(temp,"%d",destCon.maxValue);
            packCreate(gamePackbuf, "MaxValue", temp);

            sprintf(temp,"%s",destCon.oldMap);
            packCreate(gamePackbuf, "OldMap", temp);

            sprintf(temp,"%s",destCon.newMap);
            packCreate(gamePackbuf, "NewMap", temp);
            break;
        }
        /*case MERGEFAILED:
        {
            packCreate(gamePackbuf, "Content", "MergeFailed");
            break;
        }*/
        case GAMEOVER:
        {
            packCreate(gamePackbuf, "Content", "GameOver");

            sprintf(temp,"%d",destCon.mapid);
            packCreate(gamePackbuf, "GameID", temp);

            sprintf(temp,"%d",destCon.step);
            packCreate(gamePackbuf, "FinalStep", temp);
            
            sprintf(temp,"%d",destCon.score);
            packCreate(gamePackbuf, "FinalScore", temp);

            sprintf(temp,"%d",destCon.maxValue);
            packCreate(gamePackbuf, "FinalMaxValue", temp);

            sprintf(temp,"%s",destCon.newMap);
            packCreate(gamePackbuf, "FinalMap", temp);
            break;
        }

    }
    packLength(gamePackbuf);
    dataSend(destCon, strlen(gamePackbuf), gamePackbuf);
    return 0;
}

int mapInit(int matrix[][MAXCOLNUM+2])
{
    int r,c;
    for(r=0;r<=MAXROWNUM;++r)
        for(c=0;c<=MAXCOLNUM;++c)
            matrix[r][c] = 0;
    return 0;
}

int mapFill(int matrix[][MAXCOLNUM+2],int row,int col,int maxNum)
{
    int r, c;
    switch(maxNum)
    {
        case 3:
        {
            for (r = 1; r <= row; ++r)
                for (c = 1; c <= col; ++c)
                    if (matrix[r][c] < 1)
                    {
                        matrix[r][c] = rand() % 3 + 1;
                    }
            break;
        }
        case 4:
        {
            for (r = 1; r <= row; ++r)
                for (c = 1; c <= col; ++c)
                    if (matrix[r][c] < 1)
                    {
                        int t=rand() % 100;
                        if(t>=90)
                        {
                            matrix[r][c] = 4;
                            continue;
                        }   
                        if(t>=60)
                        {
                            matrix[r][c] = 3;
                            continue;
                        } 
                        if(t>=30)
                        {
                            matrix[r][c] = 2;
                            continue;
                        }
                        if(t>=0)
                        {
                            matrix[r][c] = 1;
                            continue;
                        }
                    }
            break;
        }
        case 5:
        {
            for (r = 1; r <= row; ++r)
                for (c = 1; c <= col; ++c)
                    if (matrix[r][c] < 1)
                    {
                        int t=rand() % 100;
                        if(t>=90)
                        {
                            matrix[r][c] = 5;
                            continue;
                        }   
                        if(t>=75)
                        {
                            matrix[r][c] = 4;
                            continue;
                        } 
                        if(t>=50)
                        {
                            matrix[r][c] = 3;
                            continue;
                        } 
                        if(t>=25)
                        {
                            matrix[r][c] = 2;
                            continue;
                        }
                        if(t>=0)
                        {
                            matrix[r][c] = 1;
                            continue;
                        }
                    }
            break;
        }
        case 6:
        {
            for (r = 1; r <= row; ++r)
                for (c = 1; c <= col; ++c)
                    if (matrix[r][c] < 1)
                    {
                        int t=rand() % 100;
                        if(t>=95)
                        {
                            matrix[r][c] = 5;
                            continue;
                        }   
                        if(t>=80)
                        {
                            matrix[r][c] = 5;
                            continue;
                        }   
                        if(t>=60)
                        {
                            matrix[r][c] = 4;
                            continue;
                        } 
                        if(t>=40)
                        {
                            matrix[r][c] = 3;
                            continue;
                        } 
                        if(t>=20)
                        {
                            matrix[r][c] = 2;
                            continue;
                        }
                        if(t>=0)
                        {
                            matrix[r][c] = 1;
                            continue;
                        }
                    }
            break;
        }
        default:
        {
            for (r = 1; r <= row; ++r)
                for (c = 1; c <= col; ++c)
                    if (matrix[r][c] < 1)
                    {
                        int t = rand() % 100;
                        if (t >= 95)
                        {
                            matrix[r][c] = maxNum;
                            continue;
                        }
                        if (t >= 90)
                        {
                            matrix[r][c] = maxNum - 1;
                            continue;
                        }
                        if (t >= 80)
                        {
                            matrix[r][c] = maxNum - 2;
                            continue;
                        }
                        matrix[r][c] = t / (80 / (maxNum - 3) + 1);
                    }
            break;
        }
    }
}

int mapStr(int matrix[][MAXCOLNUM+2],int row,int col,char *map)
{
    int r,c;
    for(r=1;r<=row;++r)
        for(c=1;c<=col;++c)
        {
            map[col * (r - 1) + c - 1] = matrix[r][c] + '0';
        }
    map[row * col] = 0;
    return 0;
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

int matrixRemove(int matrix[][MAXCOLNUM + 2], int x, int y, int num, int flag,int *maxValue)
{
    if (flag == 0 && matrix[y][x] == 0)
    {
        u_con.gameStatus=MERGEFAILED;
        return -50;
    }    
    if (flag == 0 && matrix[y - 1][x] != num && matrix[y + 1][x] != num && matrix[y][x - 1] != num && matrix[y][x + 1] != num)
    {
        u_con.gameStatus = MERGEFAILED;
        return -10;
    }

    if (matrix[y][x] != num)
        return 0;
    else
    {
        matrix[y][x] = matrix[y][x] * (-1);
        matrixRemove(matrix, x - 1, y, num, 1,maxValue);
        matrixRemove(matrix, x + 1, y, num, 1,maxValue);
        matrixRemove(matrix, x, y - 1, num, 1,maxValue);
        matrixRemove(matrix, x, y + 1, num, 1,maxValue);
        if (flag)
            return 0;
    }
    matrix[y][x] = num + 1;
    if (matrix[y][x] > (*maxValue))
        (*maxValue) = matrix[y][x];
    int r, c, removeNum = 1;
    for (r = 1; r <= u_con.row; ++r)
        for (c = 1; c <= u_con.col; ++c)
            if (matrix[r][c] < 0)
            {
                removeNum++;
                matrix[r][c] = 0;
            }

    u_con.gameStatus = MERGESUCCEEDED;
    return num * removeNum * 3;
}

int matrixFall(int matrix[][MAXCOLNUM + 2],int row,int col)
{
    int r, c, i;
    for(r=row;r>=1;--r)
        for(c=col;c>=1;--c)
        {
            i = 1;
            if(matrix[r][c]==0)
            {
                while (matrix[r - i][c] == 0 && i < r)
                    i++;
                if(i == r)
                    continue;
                else
                {
                    matrix[r][c]=matrix[r - i][c];
                    matrix[r - i][c] = 0;
                }
            }
        }
    return 0;
}

int gameOver(int matrix[][MAXCOLNUM + 2],int row,int col)
{
    int r,c;
    for(r=1;r<=row;++r)
        for(c=1;c<=col;++c)
        {
            if (matrix[r][c] == matrix[r + 1][c] || matrix[r][c] == matrix[r][c + 1])
                return 0;
        }
    return 1;
}