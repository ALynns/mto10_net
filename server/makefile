curPath=$(shell pwd)

mysqlLib = `mysql_config --cflags --libs`

serverFile = common.* server.* md5.*
mto_readFile = mto_read.c

object = server mto_read

.PHONY : all clean
all : $(object)

server : 
	g++ -o server $(serverFile) $(mysqlLib)

mto_read :
	g++ -o mto_read $(mto_readFile) $(mysqlLib)

gdbs : 
	g++ -g -rdynamic $(serverFile) $(mysqlLib)

gdbr :
	g++ -g -rdynamic $(serverPath)/mto_read.c $(mysqlLib)
clean :
	-rm -rf $(object) *.o *.gch core* *.out  