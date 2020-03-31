curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server
commonPath = ../common

object =  client_comp
others =  client_comp

.PHONY : all clean
all : $(object)
client_base : $(commonPath)/*.h $(commonPath)/*.c
client_comp : $(commonPath)/*.h $(commonPath)/*.c
clean :
	-rm -rf $(object) $(others)