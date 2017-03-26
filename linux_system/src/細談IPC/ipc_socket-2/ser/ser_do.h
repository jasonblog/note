#ifndef SER_DO_H_H_H_
#define SER_DO_H_H_H_

int
sln_ipc_handle_0x1(
    void* recvbuf,
    int recv_size,
    void* sendbuf,
    int* send_size);

int
sln_ipc_handle_0x2(
    void* recvbuf,
    int recv_size,
    void* sendbuf,
    int* send_size);

#endif
