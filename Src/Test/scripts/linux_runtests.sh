#!/bin/sh

#EXECUTABLE=~/Dev/BuildNaCZIr/CZICmd/CZIcmd
EXECUTABLE=~/Dev/NaCZIr/CZICmd/CZIcmd
SRCIMAGES=/datadrive/NaCZIr/TestData/

if [ -z "$1"] 
then
  python runtests.py -s $SRCIMAGES -d /home/jbohl/Data/out/ -e $EXECUTABLE -t ./testdata_linux.txt 
else
  python runtests.py -s $SRCIMAGES -d /home/jbohl/Data/out/ -e $EXECUTABLE -t ./testdata_linux.txt --skip $1
fi

