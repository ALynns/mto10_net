curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server

object =  mto10_client

head = md5.h common.h client.h

.PHONY : all clean
all : $(object)

mto10_client : $(head) md5.c common.c mto10_client.c

clean :
	-rm -rf $(object) *.o