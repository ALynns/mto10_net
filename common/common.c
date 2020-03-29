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


