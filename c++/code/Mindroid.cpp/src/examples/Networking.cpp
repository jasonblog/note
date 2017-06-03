#include <cstdio>
#include "mindroid/lang/Thread.h"
#include "mindroid/net/ServerSocket.h"
#include "mindroid/net/Socket.h"
#include "mindroid/net/DatagramSocket.h"
#include "mindroid/net/SocketAddress.h"
#include "mindroid/lang/String.h"

using namespace mindroid;

class ServerSocketThread : public Thread {
public:
    virtual void run() {
        mServerSocket = new ServerSocket();
        mServerSocket->setReuseAddress(true);
        if (!mServerSocket->bind(1234)) {
            printf("TCP-Server: Cannot bind to port 1234\n");
            return;
        }
        while (!isInterrupted() && !mServerSocket->isClosed()) {
            printf("TCP-Server: Waiting for clients...\n");
            sp<Socket> socket = mServerSocket->accept();
            if (socket != NULL) {
                const uint32_t size = 16;
                char data[size];
                socket->readFully((uint8_t*)data, size - 1);
                data[size - 1] = '\0';
                printf("TCP-Server: Received message %s from client\n", data);
                socket->write(data, size);
                socket->close();
            }
        }
        mServerSocket->close();
    }

    void shutdown() {
        mServerSocket->close();
    }

private:
    sp<ServerSocket> mServerSocket;
};

class DatagramServerThread : public Thread {
public:
    virtual void run() {
        mDatagramSocket = new DatagramSocket();
        if (!mDatagramSocket->bind(1234)) {
            printf("UDP-Server: Cannot bind to port 1234\n");
            return;
        }
        sp<SocketAddress> senderAddress = new SocketAddress();
        while (!isInterrupted() && !mDatagramSocket->isClosed()) {
            const uint32_t size = 16;
            char data[size];
            mDatagramSocket->recv((uint8_t*)data, size - 1, senderAddress);
            data[size - 1] = '\0';
            printf("UDP-Server: Received message %s from client %s:%d\n", data, senderAddress->getHostName()->c_str(), senderAddress->getPort());
            mDatagramSocket->send(data, size - 1, senderAddress);
            mDatagramSocket->close();
        }
        mDatagramSocket->close();
    }

    void shutdown() {
        mDatagramSocket->close();
    }

private:
    sp<DatagramSocket> mDatagramSocket;
};

int main() {
    // TCP client/server test
    sp<ServerSocketThread> serverSocketThread = new ServerSocketThread();
    serverSocketThread->start();
    Thread::sleep(1000);

    sp<Socket> socket = new Socket();
    printf("TCP-Client: Connecting to port 1234\n");
    if (socket->connect("localhost", 1234) == 0) {
        sp<String> string = new String("123456789ABCDEF");
        socket->write(string->c_str(), string->length());
        int32_t size = 16;
        char data[size];
        data[size - 1] = '\0';
        size = socket->readFully((uint8_t*)data, size - 1);
        printf("TCP-Client: Received message %s from server\n", data);
    } else {
        printf("Cannot connect to port 1234\n");
    }
    socket->close();
    serverSocketThread->interrupt();
    serverSocketThread->shutdown();
    serverSocketThread->join();

    printf("\n\n");

    // UDP client/server test
    sp<DatagramServerThread> datagramServerThread = new DatagramServerThread();
    datagramServerThread->start();
    Thread::sleep(1000);

    sp<DatagramSocket> udpClientSocket = new DatagramSocket();
    sp<String> string = new String("123456789ABCDEF");
    sp<SocketAddress> serverAddress = new SocketAddress("127.0.0.1", 1234);
    udpClientSocket->send(string->c_str(), string->length(), serverAddress);
    int32_t size = 16;
    char data[size];
    sp<SocketAddress> senderAddress = new SocketAddress();
    udpClientSocket->recv((uint8_t*)data, size - 1, senderAddress);
    data[size - 1] = '\0';
    printf("UDP-Client: Received message %s from server %s:%d\n", data, senderAddress->getHostName()->c_str(), senderAddress->getPort());
    udpClientSocket->close();
    datagramServerThread->interrupt();
    datagramServerThread->shutdown();
    datagramServerThread->join();

    return 0;
}
