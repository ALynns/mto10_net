curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server

mysqlLib = `mysql_config --cflags --libs`


object = server

.PHONY : all clean
all : $(object)
server : server.h server.c
	g++ -o $(object) $(serverPath)/*.h $(serverPath)/*.c $(mysqlLib)
db : 
	g++ -g -rdynamic $(serverPath)/*.c $(serverPath)/*.h $(mysqlLib)
clean :
	-rm -rf $(object) *.gch core* *.out