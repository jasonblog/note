#include "stdafx.h"
//#include <winsock2.h>
//#include <stdio.h>
//
////����Windows��socket��
//#pragma comment(lib, "ws2_32.lib")
//
//int main(int argc, char* argv[])
//{
//    //�����׽��ֿ�
//    WORD wVersionRequested = MAKEWORD(2, 2);
//    WSADATA wsaData;
//    int err = WSAStartup(wVersionRequested, &wsaData);
//    if (err != 0)
//        return 1;
//
//    SOCKET sockSrv = -1;
//    do 
//    {
//        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
//            break;
//
//        //�������ڼ������׽���        
//        sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//        if (sockSrv == -1)
//            break;
//
//        SOCKADDR_IN addrSrv;
//        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//        addrSrv.sin_family = AF_INET;
//        addrSrv.sin_port = htons(6000);
//        //���׽��֣���6000�˿��ϼ���
//        if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
//            break;
//
//        //���׽�����Ϊ����ģʽ��׼�����ܿͻ�����
//        if (listen(sockSrv, 15) == -1)
//            break;
//
//        SOCKADDR_IN addrClient;
//        int len = sizeof(SOCKADDR);
//        char msg[] = "HelloWorld";
//        while (true)
//        {
//            //�ȴ��ͻ�������������пͻ������ӣ����������
//            SOCKET sockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
//            //���ͻ��˷���HelloWorld��������
//            send(sockClient, msg, strlen(msg), 0);
//            closesocket(sockClient);
//        }// end inner-while-loop
//    } while (0); //end outer-while-loop
//    
//
//    if (sockSrv != -1)
//        closesocket(sockSrv);
//    
//    WSACleanup();
//
//    return 0;
//}
//
////char* p = new char[1024];
////
////if (����1���ɹ�)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (����2���ɹ�)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (����3���ɹ�)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////delete[] p;
////p = NULL;
//
//
//#include "stdafx.h"
//#include <winsock2.h>
//#include <stdio.h>
//
////����Windows��socket��
//#pragma comment(lib, "ws2_32.lib")
//
//int main(int argc, char* argv[])
//{
//    //�����׽��ֿ�
//    WORD wVersionRequested = MAKEWORD(1, 1);
//    WSADATA wsaData;
//    int err = WSAStartup(wVersionRequested, &wsaData);
//    if (err != 0)
//        return 1;
//
//    SOCKET sockSrv = -1;
//    do
//    {
//        if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
//            break;
//
//        //�������ڼ������׽���        
//        sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//        if (sockSrv == -1)
//            break;
//
//        SOCKADDR_IN addrSrv;
//        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//        addrSrv.sin_family = AF_INET;
//        addrSrv.sin_port = htons(6000);
//        //���׽��֣���6000�˿��ϼ���
//        if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
//            break;
//
//        //���׽�����Ϊ����ģʽ��׼�����ܿͻ�����
//        if (listen(sockSrv, 15) == -1)
//            break;
//
//        SOCKADDR_IN addrClient;
//        int len = sizeof(SOCKADDR);
//        char msg[] = "HelloWorld";
//        while (true)
//        {
//            //�ȴ��ͻ�������������пͻ������ӣ����������
//            SOCKET sockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
//            //���ͻ��˷���HelloWorld��������
//            send(sockClient, msg, strlen(msg), 0);
//            closesocket(sockClient);
//        }// end inner-while-loop
//    } while (0); //end outer-while-loop
//
//
//    if (sockSrv != -1)
//        closesocket(sockSrv);
//
//    WSACleanup();
//
//    return 0;
//}
//
//char* p = NULL;
//do 
//{
//   p = new char[1024];
//   if (����1���ɹ�)
//       break;
//
//   if (����2���ɹ�)
//       break;
//
//   if (����3���ɹ�)
//       break;
//} while (0);
//
//delete[] p;
//p = NULL;