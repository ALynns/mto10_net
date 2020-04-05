#include "server.h"

int main()
{
    daemon(1,1);
    localBind()

}

int packAnalysis(char *buf, int *packType, void *pack)
{
    char *tempBuf,*tempStr;
    tempBuf = strtok(buf, "\r\n");//��ȡType����Ϣ
    if(tempBuf!=NULL)
    {
        tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
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
            tempStr = strtok(NULL, "\r\n");//��ȡMD5
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            strcpy((ParameterAuthenticatePack *)pack->MD5,);
            tempStr = strtok(NULL, "\r\n");//��ȡRow
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (ParameterAuthenticatePack *)pack->row=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//��ȡCol
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (ParameterAuthenticatePack *)pack->col=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//��ȡGameID
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (ParameterAuthenticatePack *)pack->gameID=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//��ȡdelay
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (ParameterAuthenticatePack *)pack->delay=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//����
            break;
        }
        case COORDINATE :
        {
            tempStr = strtok(NULL, "\r\n");//��ȡRow
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (CoordinatePack *)pack->row=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//��ȡCol
            tempStr=(char*)memchr(tempBuf, '=', strlen(tempBuf))+2;
            (CoordinatePack *)pack->col=atoi(tempStr);
            tempStr = strtok(NULL, "\r\n");//����
            break;
        }
    }
}

int hostBind(char *IPAddr,char *port)
{
    struct sockaddr_in serviceAddr;
    int ret;
    memset(&serviceAddr,0,sizeof(serviceAddr));//���ڸýṹ���д��ڱ���λ����Ҫ��0
    serviceAddr.sin_family=AF_INET;//�����ַ�����ͣ�AF_INETΪIPv4��AF_INET6ΪIPv6
    serviceAddr.sin_port = htons(atoi(port));
    if(IPAddr!=NULL)
        serviceAddr.sin_addr.s_addr = inet_addr(IPAddr);      //inet_addr("127.0.0.1");
    int socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Linux�ж���socketΪһ���ļ�������ֵΪһ���ļ�������
    if(socketfd<0)
    {
        printf("socket error\n");
        exit(-1);
    }
    
    fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);

    int reuse=1;
    setsockopt(socketfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)& reuse,sizeof(int));
    
    ret=bind(socketfd, (struct sockaddr *)&serviceAddr, sizeof(serviceAddr));//�󶨶˿�
    if (ret < 0)
    {
        printf("bind error\n");
        exit(-1);
    }
    ret = listen(socketfd, SOMAXCONN);
    if (ret < 0) //listen()����ʹ�׽��ֽ��뱻������״̬��SOMAXCONNΪ�������������
    {
        printf("listen error\n");
        exit(-1);
    }
    return socketfd;
}

void dataRecv(int signo)
{
    if(connectAmount==0)
        return;

    static int sendSize[MAXCONNECTAMOUNT] = {0};
    int ret;
    

    fd_set fdsr;

    //struct timeval tv;
    //tv.tv_sec = 1;
    while(1)
    {
        FD_ZERO(&fdsr);

        int i;
        for (i = 0; i < connectAmount; ++i)
            FD_SET(socketfd[i], &fdsr);

        ret = select(maxClientSocketfd + 1, &fdsr, NULL, NULL, NULL);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error\n");
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
    
    int i;
    for (i = 0; i < connectAmount; i++)
    {
        if (FD_ISSET(socketfd[i], &fdsr))
            ret = recv(socketfd[i], recvBuf[i], RECVBUFSIZE, 0);
        if (ret < 0)
        {
            printf("����%dRecv error\n", i);
        }
        else
        {
            sendSize[i] += ret;
            printf("����%d�ѽ���%d�ֽ�\n", i, sendSize[i]);
        }
    }
}

void dataSend(int signo)
{
    if(connectAmount==0)
        return;
    static int sendSize[MAXCONNECTAMOUNT] = {0};
    int ret;

    fd_set fdsr;
    
    while(1)
    {
        FD_ZERO(&fdsr);

        int i;
        for (i = 0; i < connectAmount; ++i)
            FD_SET(socketfd[i], &fdsr);

        //struct timeval tv;
        //tv.tv_sec = 1;
        ret = select(maxClientSocketfd + 1, NULL, &fdsr, NULL, NULL);
        if (ret < 0 && errno != EINTR)
        {
            printf("select error,%d\n", ret);
            exit(-1);
        }
        else
        {
            if (ret > 0)
                break;
            if(errno==EINTR)
            {
                continue;
            }    
            else
                break;
        }
        
    }
    //FD_ISSET(socketfd, &fdsr)�ж��׽����Ƿ��������������һ�������������Թ�
    int i;
    for (i = 0; i < connectAmount; i++)
    {
        if(!FD_ISSET(socketfd[i],&fdsr))
            continue;
        ret = send(socketfd[i], sendBuf[i], SENDBUFSIZE, 0);
        if (ret < 0)
        {
            printf("����%d send error\n", i);
            continue;
        }
        sendSize[i] += ret;
        printf("����%d�ѷ���%d�ֽ�\n", i, sendSize[i]);
    }
}
