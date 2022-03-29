/**
 * �ʼ�����߳�, MailMonitor.h
 * zhangyl 2019.05.11
 */

#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

struct MailItem
{
    std::string subject;
    std::string content;
};

class MailMonitor final
{
public:
    static MailMonitor& getInstance();

private:
    MailMonitor() = default;
    ~MailMonitor() = default;
    MailMonitor(const MailMonitor & rhs) = delete;
    MailMonitor& operator=(const MailMonitor & rhs) = delete;

public:
    bool initMonitorMailInfo(const std::string& servername, const std::string& mailserver, short mailport, const std::string& mailfrom, const std::string& mailfromPassword, const std::string& mailto);
    void uninit();
    void wait();

    void run();

    bool alert(const std::string& subject, const std::string& content);

private:
    void alertThread();

    void split(const std::string& str, std::vector<std::string>& v, const char* delimiter = "|");
    
private:
    std::string                             m_strMailName;              //���ڱ�ʶ����һ̨���������͵��ʼ�
    std::string                             m_strMailServer;
    short                                   m_nMailPort;
    std::string                             m_strFrom;
    std::string                             m_strFromPassword;
    std::vector<std::string>                m_strMailTo;

    std::list<MailItem>                     m_listMailItemsToSend;      //��д�����־
    std::shared_ptr<std::thread>            m_spMailAlertThread;
    std::mutex                              m_mutexAlert;
    std::condition_variable                 m_cvAlert;
    bool                                    m_bExit;                    //�˳���־
    bool                                    m_bRunning;                 //���б�־
};
