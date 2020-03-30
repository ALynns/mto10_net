#include "common.h"

int getMD5(unsigned char *dest,unsigned char *src)
{
    int i;
	unsigned char decrypt[16];
    unsigned char temp[3];

	MD5_CTX md5;
	MD5Init(&md5);         		
	MD5Update(&md5,src,strlen((char *)src));
	MD5Final(&md5,decrypt);

    dest[0] = 0;
    for(i=0;i<16;++i)
	{
		sprintf(temp,"%02x",decrypt[i]);
        strcat(dest,temp);
	}
	return 0;
}

void setTimer(int s_val,int us_val,int s_interval,int us_interval)
{
    struct itimerval new_value, old_value;
    new_value.it_value.tv_sec = s_val;
    new_value.it_value.tv_usec = us_val;
    new_value.it_interval.tv_sec = s_interval;
    new_value.it_interval.tv_usec = us_interval;
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

int packCreate(char *buf, char *tp, char *tp_value)
{
    strcat(buf,tp);
    strcat(buf," = ");
    strcat(buf,tp_value);
    strcat(buf,"\r\n");
    return 0;
}

int packLength(char *buf)
{
    int len;
    char tmp[10];

    strcat(buf, "Length = ");

    len = strlen(buf)+2;//当前字符串长度

    sprintf(tmp, "%d",len);//长度位数
    len = len + strlen(tmp); //总字符串长度
    sprintf(tmp, "%d",len);
    if (len != strlen(buf) + strlen(tmp) + 2)
        len= strlen(buf) + strlen(tmp) + 2;

    sprintf(tmp, "%d", len);
    strcat(buf,tmp);
    strcat(buf,"\r\n");
    return 0;
}

int getVar(char *opt, char *dest, char *src)
{
    int i, flag;
    int srcLength = strlen(src);
    for (i = 0, flag = (opt == NULL ? 1 : 0); i < srcLength; ++i)
    {
        if (flag == 0 && src[i] == ' ' && opt != NULL)
        {
            strncpy(opt, src, i);
            opt[i] = 0;
            flag = 1;
            printf("%s\n",opt);
        }
        if (flag == 1 && src[i - 1] == '=' && src[i] == 0x20 && dest != NULL)
        {
            strcpy(dest, &src[i + 1]);
        }    
    }

}
