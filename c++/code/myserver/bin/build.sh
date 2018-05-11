#!/bin/sh

cd ../gameserver

svn up

make .

make clean

make -j10

cd -

