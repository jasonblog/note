//
//  net_inc.h
//  Created by DGuco on 18-1-7.
//
//

#ifndef _NET_INC_H_
#define _NET_INC_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>


#define PpeGetLastError() errno
#define CloseSocket            close
#define ioctlsocket            ioctl
#define SOCKET            int
typedef struct linger LINGER;
#define SOCKET_ERROR        -1
#define INVALID_SOCKET        -1
#define SD_SEND            SHUT_WR

#define INTER_FACE struct

enum EPipeDisconnectReason {
    ePDR_REMOTE,
    ePDR_ERROR
};

enum EPipeConnFailedReason {
    ePCFR_UNREACH,
    ePCFR_INVALIDADDR,
    ePCFR_REFUSED,
    ePCFR_RESET,
    ePCFR_LOCALADDRINUSE,
    ePCFR_NOBUFFER,
    ePCFR_TIMEDOUT
};

#endif
