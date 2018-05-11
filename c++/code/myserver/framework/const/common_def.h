//
// Created by DGuco on 17-6-3.
//

#ifndef SERVER_COMMONDEF_H
#define SERVER_COMMONDEF_H

// 管道标识符
enum enLockIdx
{
    IDX_PIPELOCK_C2S = 0,
    IDX_PIPELOCK_S2C = 1,
    IDX_PIPELOCK_A2C = 2,
    IDX_PIPELOCK_S2L = 3,
};

#endif //SERVER_COMMONDEF_H
