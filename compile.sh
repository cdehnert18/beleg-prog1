#!/bin/bash

clear

gcc -Wall -g -o materialverwaltung gui.c list.c loader.c `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

if ! [ $? = 0 ]
then
	echo ERROR
	echo "gcc -Wall -g -o materialverwaltung gui.c list.c loader.c \`pkg-config --cflags --libs gtk+-3.0\` -export-dynamic"
else
	echo OK
fi
