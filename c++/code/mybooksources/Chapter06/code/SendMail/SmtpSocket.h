/**
 * �����ʼ��࣬SmtpSocket.h
 * zhangyl 2019.05.11
 */

#pragma once

#include <string>
#include <vector>
#include "Platform.h"

class SmtpSocket final
{
public:
    static bool sendMail(const std::string& server, short port, const std::string& from, const std::string& fromPassword,
                         const std::vector<std::string>& to, const std::string& subject, const std::string& mailData);

public:
	SmtpSocket(void);
	~SmtpSocket(void);
	
	bool isConnected() const { return m_hSocket; }
	bool connect(const char* pszUrl, short port = 25);
	bool logon(const char* pszUser, const char* pszPassword);
	bool setMailFrom(const char* pszFrom);
	bool setMailTo(const std::vector<std::string>& sendTo);
	bool send(const std::string& subject, const std::string& mailData);

	void closeConnection();
	void quit();	//�˳�

private:
	/*
	����:
		��֤�ӷ��������ص�ǰ��λ����ʹ��ݽ����Ĳ����Ƿ�һ��

	��ע:
		211 ��������ϵͳ״̬
		214 ������Ϣ
		220 ����׼������
		221 �ر�����
		235 �û���֤�ɹ�
		250 �����������
		251 �û����ڱ��أ�ת�ĵ�����·��
		334 �ȴ��û�������֤��Ϣ
		354 ��ʼ�ʼ�����
		421 ���񲻿���
		450 ����δִ�У�����æ
		451 ������ֹ�����ش���
		452 ����δִ�У��洢�ռ䲻��
		500 �����ʶ������Դ���
		501 �����﷨����
		502 ���֧��
		503 ����˳�����
		504 ���������֧��
		550 ����δִ�У����䲻����
		551 �Ǳ����û�
		552 ��ֹ�洢�ռ䲻��
		553 ����δִ�У�����������ȷ
		554 ����ʧ��
	*/
	bool checkResponse(const char* recvCode);

private:	
	bool		                m_bConnected;
	SOCKET		                m_hSocket;
    std::string                 m_strUser;
    std::string                 m_strPassword;
    std::string                 m_strFrom;
    std::vector<std::string>    m_strTo;
};