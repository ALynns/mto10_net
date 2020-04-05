curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server

object =  client_base client_comp
others =  client_base client_comp

head = md5.h common.h client.h

.PHONY : all clean
all : $(object)

client_base : $(head) md5.c common.c client_base.c
client_comp : $(head) md5.c common.c client_comp.c

clean :
	-rm -rf $(object) $(others)