/** 
 * @desc:   �첽��־�࣬AsyncLog.h
 * @author: zhangyl
 * @date:   2019.04.13
 */

#ifndef __ASYNC_LOG_H__
#define __ASYNC_LOG_H__

#include <stdio.h>
#include <string>
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

//#ifdef LOG_EXPORTS
//#define LOG_API __declspec(dllexport)
//#else
//#define LOG_API __declspec(dllimport)
//#endif

#define LOG_API

enum LOG_LEVEL
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,    //����ҵ�����
    LOG_LEVEL_SYSERROR, //ͬ�����������־�����ڳ����˳�ǰ���
    LOG_LEVEL_FATAL,    //FATAL �������־�����ڳ��������־���˳�
    LOG_LEVEL_CRITICAL  //CRITICAL ��־������־������ƣ��������
};

//TODO: �����Ӽ�������
//ע�⣺�����ӡ����־��Ϣ�������ģ����ʽ���ַ���Ҫ��_T()�����������
//e.g. LOGI(_T("GroupID=%u, GroupName=%s, GroupName=%s."), lpGroupInfo->m_nGroupCode, lpGroupInfo->m_strAccount.c_str(), lpGroupInfo->m_strName.c_str());
#define LOGT(...)    CAsyncLog::Output(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOGD(...)    CAsyncLog::Output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(...)    CAsyncLog::Output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(...)    CAsyncLog::Output(LOG_LEVEL_WARNING, __FILE__, __LINE__,__VA_ARGS__)
#define LOGE(...)    CAsyncLog::Output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOGSYSE(...) CAsyncLog::Output(LOG_LEVEL_SYSERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOGF(...)    CAsyncLog::Output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)        //Ϊ����FATAL�������־������crash���򣬲�ȡͬ��д��־�ķ���
#define LOGC(...)    CAsyncLog::Output(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)     //�ؼ���Ϣ��������־�����������

//����������ݰ��Ķ����Ƹ�ʽ
#define LOG_DEBUG_BIN(buf, buflength) CAsyncLog::OutputBinary(buf, buflength)

class LOG_API CAsyncLog
{
public:
    static bool Init(const char* pszLogFileName = nullptr, bool bTruncateLongLine = false, int64_t nRollSize = 10 * 1024 * 1024);
	static void Uninit();

    static void SetLevel(LOG_LEVEL nLevel);
    static bool IsRunning();
	
	//������߳�ID�ź����ں���ǩ�����к�
	static bool Output(long nLevel, const char* pszFmt, ...);
	//����߳�ID�ź����ں���ǩ�����к�	
    static bool Output(long nLevel, const char* pszFileName, int nLineNo, const char* pszFmt, ...);

    static bool OutputBinary(unsigned char* buffer, size_t size);

private:
    CAsyncLog() = delete;
    ~CAsyncLog() = delete;

    CAsyncLog(const CAsyncLog& rhs) = delete;
    CAsyncLog& operator=(const CAsyncLog& rhs) = delete;

    static void MakeLinePrefix(long nLevel, std::string& strPrefix);
    static void GetTime(char* pszTime, int nTimeStrLength);
    static bool CreateNewFile(const char* pszLogFileName);
    static bool WriteToFile(const std::string& data);
    //�ó�����������
    static void Crash();

    static const char* ullto4Str(int n);
    static char* FormLog(int& index, char* szbuf, size_t size_buf, unsigned char* buffer, size_t size);

    static void WriteThreadProc();
	
private:
	static bool		                        m_bToFile;			    //��־д���ļ�����д������̨  
	static FILE*                            m_hLogFile;
    static std::string                      m_strFileName;          //��־�ļ���
    static std::string                      m_strFileNamePID;    //�ļ����еĽ���id
    static bool                             m_bTruncateLongLog;     //����־�Ƿ�ض�
    static LOG_LEVEL                        m_nCurrentLevel;        //��ǰ��־����
    static int64_t                          m_nFileRollSize;        //������־�ļ�������ֽ���
    static int64_t                          m_nCurrentWrittenSize;  //�Ѿ�д����ֽ���Ŀ
    static std::list<std::string>           m_listLinesToWrite;     //��д�����־
    static std::shared_ptr<std::thread>     m_spWriteThread;
    static std::mutex                       m_mutexWrite;
    static std::condition_variable          m_cvWrite;
    static bool                             m_bExit;                //�˳���־
    static bool                             m_bRunning;             //���б�־
};

#endif // !__ASYNC_LOG_H__