#include <iostream>
#include "Pop3Socket.h"
#include "Platform.h"

//Winsock������ʼ��
#ifdef WIN32
NetworkInitializer windowsNetworkInitializer;
#endif

const std::string servername = "MailAlertSysem";
const std::string mailserver = "pop.163.com";
const short mailport = 110;
const std::string mailuser = "testformybook@163.com";
const std::string mailpassword = "2019hhxxttxs";
const std::string mailto = "balloonwj@qq.com;analogous_love@qq.com";

int main()
{
    Pop3Socket pop3Socket;
    //�������������
    if (!pop3Socket.connect(mailserver.c_str(), mailport))
    {
        std::cout << "Unable to connect to mail server" << std::endl;
        return 1;
    }

    //��½���������
    if (!pop3Socket.logon(mailuser.c_str(), mailpassword.c_str()))
    {
        std::cout << "Unable to logon to mail server" << std::endl;
        return 1;
    }

    //��ȡ�ʼ��б�
    if (!pop3Socket.getList())
    {
        std::cout << "Unable to get mail list from mail server" << std::endl;
        return 1;
    }

    long mailCount = pop3Socket.getMailCount();
    std::cout << "Mail count: " << mailCount << std::endl;

    int mailIndex;
    //��ʼ�����ʼ�
    for (int i = 0; i < mailCount; ++i)
    {
        //��������ʼ���ʼ��
        mailIndex = pop3Socket.getMailID(mailCount - i - 1);
        if (!pop3Socket.getHeader(mailIndex))
        {
            if (!pop3Socket.isConnected())
                break;

            continue;
        }

        std::wcout << L"title: " << pop3Socket.getTitle(mailIndex) << std::endl;
        std::cout << "from: " << pop3Socket.getFrom(mailIndex) << std::endl;
        std::cout << "messageID: " << pop3Socket.getMessageID(mailIndex) << std::endl;

        std::cout << "date: " << pop3Socket.getDate(mailIndex) << std::endl;
        std::cout << "time: " << pop3Socket.getTime(mailIndex) << std::endl;


        if (!pop3Socket.getMail(mailIndex))
            break;

    }

    pop3Socket.quit();
    pop3Socket.close();

    return 0;
}
