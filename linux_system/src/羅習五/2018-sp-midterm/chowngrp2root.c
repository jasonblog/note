/*
Usage:
sudo chowngrp2root someFile
*/

#include <sys/types.h>
#include <unistd.h>
int main(int argc, char** argv)
{
    chown(argv[1], 0, 0);
}