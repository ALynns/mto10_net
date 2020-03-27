curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server
commonPath = ../common

object = client
others = client

.PHONY : all clean
all : $(object)
client : $(commonPath)/*.h $(commonPath)/*.c
clean :
	-rm -rf $(object) $(others)