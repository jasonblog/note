#include "stdafx.h"
#include <Windows.h>
#include <string>

char g_szTime[64] = { "time not set..." };
LPVOID mainWorkerFiber = NULL;

void WINAPI workerFiberProc(LPVOID lpFiberParameter)
{
    while (true)
    {
        //��������һ��ܺ�ʱ�Ĳ���
        SYSTEMTIME st;
        GetLocalTime(&st);
        wsprintfA(g_szTime, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        printf("%s\n", g_szTime);

        //�л������˳�
        //SwitchToFiber(mainWorkerFiber);
    }  
}

int main()
{
    mainWorkerFiber = ConvertThreadToFiber(NULL);
    
    int index = 0;
    while (index < 100)
    {
        ++index;
                 
        LPVOID pWorkerFiber = CreateFiber(0, workerFiberProc, NULL);
        if (pWorkerFiber == NULL)
            return -1;
        //�л����µ��˳�
        SwitchToFiber(pWorkerFiber);

        memset(g_szTime, 0, sizeof(g_szTime));
        strncpy(g_szTime, "time not set...", strlen("time not set..."));
        

        printf("%s\n", g_szTime);
        
        Sleep(1000);      
    }

    //�л����߳�ģʽ
    ConvertFiberToThread();
    
    return 0;
}

