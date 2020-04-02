curPath=$(shell pwd)
clientPath = ./client
serverPath = ./server
commonPath = ./common

object = test
others = test 

.PHONY : all clean

all : $(object)

test : $(commonPath)/* $(clientPath)/* 
