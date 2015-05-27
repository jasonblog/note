# trace-cmd
```
To compile for arm on Ubuntu
1.Download code
apt-get source trace-cmd

2. install arm gcc
sudo apt-get install gcc-arm-linux-gnueabi

3. Compile
make LDFLAGS=-static CC=arm-linux-gnueabi-gcc trace-cmd
```
