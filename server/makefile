curPath=$(shell pwd)
clientPath = ../client
serverPath = ../server

mysqlLib = -lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql -ldl -lpthread -lmysqlclient


object = server

.PHONY : all clean
all : clean $(object)
server : server.h server.c
	g++ -o $(object) $(serverPath)/*.h $(serverPath)/*.c $(mysqlLib)
clean :
	-rm -rf $(object) 