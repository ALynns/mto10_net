curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server

mysqlLib = -lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql -ldl -lpthread -lmysqlclient


object = server

.PHONY : all clean
all : $(object)
server : server.h server.c
	g++ -o $(object) $(serverPath)/*.h $(serverPath)/*.c $(mysqlLib)
db : 
	g++ -g -rdynamic $(serverPath)/*.c $(serverPath)/*.h $(mysqlLib)
clean :
	-rm -rf $(object) *.gch core* *.out