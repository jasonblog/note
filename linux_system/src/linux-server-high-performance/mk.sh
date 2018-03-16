# !/bin/sh

#simple make command
g++ -Wall epollclient.cpp -o epollclient
g++ -Wall epollserver.cpp -o epollserver

g++ 5-3-testlisten.c -o testlisten
g++ 5-5-testaccept.c -o testaccept
g++ 5-6-testoobsend.c -o testoobsend
g++ 5-7-testoobrecv.c -o testoobrecv
g++ 5-10-setsendbuffer.c -o setsendbuffer
g++ 5-11-setrecvbuffer.c -o setrecvbuffer
g++ 5-12-daytime.c -o daytime
g++ 6-1-dup-cgi.c -o  dup-cgi
g++ 6-2-web-writev.c -o web-writev
g++ 6-3-sendfile.c -o sendfiletest
g++ 6-4-splice.c   -o splicetest
g++ 6-5-tee.c -o teetest 
g++ 7-1-uideuid.c -o uideuidtest
g++ 8-3-http-analyze.c -o http-analyze 
g++ 9-1-select-recvdata.c -o select-recvdata 
g++ 9-3-epoll-ltandet.c -o epoll-ltoret
g++ 9-4-epolloneshot.c -o epolloneshottest -lpthread
g++ 9-5-nonblock-connect.c -o nonblock-connect
g++ 9-6-client.c -o client-topic
g++ 9-7-server.c -o server-topic
g++ 9-8-tcp-udp.c -o tcp-udp-server
g++ 10-1-unify-signal.c -o unified-signal
g++ 10-3-sigurg-signal.c -o sigurg-signal
gcc 13-3-ipc_private.c -o ipc-private
g++ 13-4-shm-chatroom.c -o shm-chatroom -lrt
g++ 13-5-send-fd.c -o send-fd
g++ 14-1-dead-lock.c -o dea-lock -lpthread
g++ 14-3-multi-thread-fork.c -o multi-thread-fork -lpthread
g++ 14-5-thread-handle-signal.c -o threadhandlesignal -lpthread
g++ processpool.h 15-2-process-pool-cgi.c -o process-pool-cgi
