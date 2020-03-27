OBJ = common.o md5.o

all : $(OBJ)

md5.o : md5.h md5.c

common.o : common.h md5.h

clean :
	-rm -rf $(OBJ)