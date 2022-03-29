/**
 * WSAEventSelect ģ����ʾ
 * zhangyl 2019.03.16
 */
#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, _TCHAR* argv[])
{
    //1. ��ʼ���׽��ֿ�
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    int nError = WSAStartup(wVersionRequested, &wsaData);
    if (nError != 0)
        return -1;
   
    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return -1;
    }

    //2. �������ڼ������׽���
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    
    //3. ���׽���
    if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        closesocket(sockSrv);
        WSACleanup();
        return -1;
    }
    
    //4. ���׽�����Ϊ����ģʽ��׼�����ܿͻ�����
    if (listen(sockSrv, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(sockSrv);
        WSACleanup();
        return -1;
    }

    WSAEVENT hListenEvent = WSACreateEvent();
    if (WSAEventSelect(sockSrv, hListenEvent, FD_ACCEPT) == SOCKET_ERROR)
    {
        WSACloseEvent(hListenEvent);
        closesocket(sockSrv);
        WSACleanup();
        return -1;
    }
        

    WSAEVENT* pEvents = new WSAEVENT[1];
    pEvents[0] = hListenEvent;
    SOCKET* pSockets = new SOCKET[1];
    pSockets[0] = sockSrv;
    DWORD dwCount = 1;
    bool bNeedToMove;

    while (true)
    {
        bNeedToMove = false;
        DWORD dwResult = WSAWaitForMultipleEvents(dwCount, pEvents, FALSE, WSA_INFINITE, FALSE);
        if (dwResult == WSA_WAIT_FAILED)
            continue;

        DWORD dwIndex = dwResult - WSA_WAIT_EVENT_0;
        for (DWORD i = 0; i <= dwIndex; ++i)
        {
            //ͨ��dwIndex����ҵ�hEvents�����е�WSAEvent���󣬽����ҵ���Ӧ��socket
            WSANETWORKEVENTS  triggeredEvents;
            if (WSAEnumNetworkEvents(pSockets[i], pEvents[i], &triggeredEvents) == SOCKET_ERROR)
                continue;

            if (triggeredEvents.lNetworkEvents & FD_ACCEPT)
            {
                if (triggeredEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                    continue;

                //����accept����������������¼�;
                SOCKADDR_IN addrClient;
                int len = sizeof(SOCKADDR);
                //�ȴ��ͻ�������
                SOCKET hSockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
                if (hSockClient != SOCKET_ERROR)
                {
                    //�����ͻ���socket�Ŀɶ��͹ر��¼�
                    WSAEVENT hClientEvent = WSACreateEvent();
                    if (WSAEventSelect(hSockClient, hClientEvent, FD_READ | FD_CLOSE) == SOCKET_ERROR)
                    {
                        WSACloseEvent(hClientEvent);
                        closesocket(hSockClient); 
                        continue;
                    }
                        
                    WSAEVENT* pEvents2 = new WSAEVENT[dwCount + 1];
                    SOCKET* pSockets2 = new SOCKET[dwCount + 1];
                    memcpy(pEvents2, pEvents, dwCount * sizeof(WSAEVENT));
                    pEvents2[dwCount] = hClientEvent;
                    memcpy(pSockets2, pSockets, dwCount * sizeof(SOCKET));
                    pSockets2[dwCount] = hSockClient;
                    delete[] pEvents;
                    delete[] pSockets;
                    pEvents = pEvents2;
                    pSockets = pSockets2;

                    dwCount++;

                    printf("a client connected, socket: %d, current: %d\n", (int)hSockClient, dwCount - 1);
                }
            }
            else if (triggeredEvents.lNetworkEvents & FD_READ)
            {
                if (triggeredEvents.iErrorCode[FD_READ_BIT] != 0)
                    continue;

                char szBuf[64] = { 0 };
                int nRet = recv(pSockets[i], szBuf, 64, 0);
                if (nRet > 0)
                {
                    printf("recv data: %s, client: %d\n", szBuf, pSockets[i]);
                }
            }
            else if (triggeredEvents.lNetworkEvents & FD_CLOSE)
            {
                //�˴���Ҫ�ж�
                //if (triggeredEvents.iErrorCode[FD_READ_BIT] != 0)
                //    continue;
                
                printf("a client disconnected, socket: %d, current: %d\n", (int)pSockets[i], dwCount - 2);

                WSACloseEvent(pEvents[i]);
                closesocket(pSockets[i]);

                //���Ϊ��Ч��ѭ��������ͳһ�Ƴ�
                pSockets[i] = INVALID_SOCKET;

                bNeedToMove = true;
            }
            
        }// end for-loop

        if (bNeedToMove)
        {
            //�Ƴ���Ч���¼�
            std::vector<SOCKET> vValidSockets;
            std::vector<HANDLE> vValidEvents;
            for (size_t i = 0; i < dwCount; ++i)
            {
                if (pSockets[i] != INVALID_SOCKET)
                {
                    vValidSockets.push_back(pSockets[i]);
                    vValidEvents.push_back(pEvents[i]);
                }
            }

            size_t validSize = vValidSockets.size();
            if (validSize > 0)
            {
                WSAEVENT* pEvents2 = new WSAEVENT[validSize];
                SOCKET* pSockets2 = new SOCKET[validSize];
                memcpy(pEvents2, &vValidEvents[0], validSize * sizeof(WSAEVENT));
                memcpy(pSockets2, &vValidSockets[0], validSize * sizeof(SOCKET));
                delete[] pEvents;
                delete[] pSockets;
                pEvents = pEvents2;
                pSockets = pSockets2;

                dwCount = validSize;
            }
        }
        
    }// end while-loop

    closesocket(sockSrv);

    WSACleanup();
                     
    return 0;
}