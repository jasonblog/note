#ifndef SOCK_IPC_H_H_
#define SOCK_IPC_H_H_

#define SOCK_IPC_NAME                   "/tmp/sock_ipc_name"
#define SOCK_IPC_MAX_BUF                10240

#define SOCK_IPC_SER_LISTEN_PORT        8902
#define SOCK_IPC_MAX_CONN               5

#define USE_AF_UNIX                     1

typedef struct _ipc_sock_msg_t {
    int     msg_type;
    int     msg_rc;
    int     msg_buflen;
    char    msg_buf[SOCK_IPC_MAX_BUF];
} ipc_sock_msg_t;


#define SLN_IPC_MAX_TYPE                256

typedef enum {
    SLN_IPC_RC_OK = 0,
    SLN_IPC_RC_ERR = -1,
    SLN_IPC_RC_TYPE = -2,
    SLN_IPC_RC_FUNC = -3,
    SLN_IPC_RC_OTHER = -99
} sln_ipc_rc_t;


enum {
    SLN_IPC_TYPE_0x1 = 0,
    SLN_IPC_TYPE_0x2 = 1
};
#endif
