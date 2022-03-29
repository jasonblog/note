// test_map_try_emplace.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>

class ChatDialog
{
//����ʵ��ʡ��...
public:
    void activate()
    {
        //ʵ��ʡ��
    }
};

//���ڹ�����������Ի����map��key�Ǻ���id��ChatDialog������Ի���ָ��
std::map<int64_t, ChatDialog*> m_ChatDialogs;

//��ͨ�汾
void onDoubleClickFriendItem(int64_t userid)
{
    auto targetChatDialog = m_ChatDialogs.find(userid);
    //���ѶԻ��򲻴��ڣ��򴴽�֮��������
    if (targetChatDialog == m_ChatDialogs.end())
    {
        ChatDialog* pChatDialog = new ChatDialog();
        m_ChatDialogs.insert(std::pair<int64_t, ChatDialog*>(userid, pChatDialog));
        pChatDialog->activate();
    }
    //���ѶԻ�����ڣ�ֱ�Ӽ���
    else
    {
        targetChatDialog->second->activate();
    }
}

//C++ 17�汾1
void onDoubleClickFriendItem2(int64_t userid)
{   
    //�ṹ���󶨺�try_emplace���� C++17�﷨
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid);
    if (inserted)
        iter->second = new ChatDialog();   

    iter->second->activate();
}

int main()
{
    //��������
    //906106643 ��userid
    onDoubleClickFriendItem2(906106643L);
    //906106644 ��userid
    onDoubleClickFriendItem2(906106644L);
    //906106643 ��userid
    onDoubleClickFriendItem2(906106643L);

    return 0;
}

