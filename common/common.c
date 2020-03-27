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

