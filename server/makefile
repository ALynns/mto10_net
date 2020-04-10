curPath=$(shell pwd)
commonPath = $(curPath)/commonsource
serverPath = $(curPath)/serversource

mysqlLib = `mysql_config --cflags --libs`

serverFile = $(commonPath)/* $(serverPath)/server.c $(serverPath)/server.h

object = server

.PHONY : all clean
all : $(object)
server : 
	g++ -o server $(serverFile) $(mysqlLib)
db : 
	g++ -g -rdynamic $(serverFile) $(mysqlLib)
clean :
	-rm -rf $(object) $(commonPath)/*.o $(commonPath)/*.gch $(commonPath)/core* $(commonPath)/*.out  $(serverPath)/*.o $(serverPath)/*.gch $(serverPath)/*.out $(serverPath)/core* 