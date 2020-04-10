curPath=$(shell pwd)

mysqlLib = `mysql_config --cflags --libs`

serverFile = common.* server.* md5.*
mto_readFile = mto_read.c

object = mto10_server_d mto10_read

.PHONY : all clean
all : $(object)

mto10_server_d : 
	g++ -o mto10_server_d $(serverFile) $(mysqlLib)

mto10_read :
	g++ -o mto10_read $(mto_readFile) $(mysqlLib)

gdbs : 
	g++ -g -rdynamic $(serverFile) $(mysqlLib)

gdbr :
	g++ -g -rdynamic $(serverPath)/mto_read.c $(mysqlLib)

clean :
	-rm -rf $(object) *.o *.gch core* *.out *.log