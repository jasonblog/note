#include <stdio.h>
#include <string.h>
#include "sock_ipc.h"

static char     gbuf[256] = "hello, this is server!";

int
sln_ipc_handle_0x1(void* recvbuf, int recv_size, void* sendbuf, int* send_size)
{
    printf("=============%s->%d===========\n", __func__, __LINE__);
    memcpy(sendbuf, gbuf, strlen(gbuf));

    *send_size = strlen(gbuf);

    return SLN_IPC_RC_OK;
}

int
sln_ipc_handle_0x2(void* recvbuf, int recv_size, void* sendbuf, int* send_size)
{
    printf("=============%s->%d===========\n", __func__, __LINE__);
    memcpy(gbuf, recvbuf, recv_size);

    *send_size = 0;

    return SLN_IPC_RC_OK;
}
