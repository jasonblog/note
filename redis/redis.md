# Redis

## Build Redis

```sh
wget http://download.redis.io/releases/redis-3.2.5.tar.gz
tar xzf redis-3.2.5.tar.gz
cd redis-3.2.5/src
vim Makefile ; 修改OPTIMIZATION?=-O2 为OPTIMIZATION?=-O0
make CFLAGS="-g -O0"
make PREFIX=/usr/local/redis install
```


## Run Redis Server

```sh
cgdb --args ./redis-server ../redis.conf
start
```