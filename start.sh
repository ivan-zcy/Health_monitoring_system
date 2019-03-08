#!/bin/bash

if [ "$1" = "DEBUG" ];then
    cd tool/
    g++ tool.cpp -D DEBUG -c

    cd ../hash/
    g++ hash.cpp -D DEBUG -c

    cd ../rwt/
    g++ rwt.cpp -D DEBUG -c

    cd ..
    g++ Server.cpp -D DEBUG -c
    g++ User.cpp -D DEBUG -c

    g++ User.o tool/tool.o -o User
    g++ Server.o tool/tool.o hash/hash.o rwt/rwt.o -o Server
else
    cd tool/
    g++ tool.cpp -c

    cd ../hash/
    g++ hash.cpp -c

    cd ../rwt/
    g++ rwt.cpp -c

    cd ..
    g++ Server.cpp -c
    g++ User.cpp -c

    g++ User.o tool/tool.o -o User
    g++ Server.o tool/tool.o hash/hash.o rwt/rwt.o -o Server
fi 
