/** 
 * std::map::try_emplace�÷���ʾ
 * zhangyl 2019.10.06
 */

#include <iostream>
#include <map>
#include <memory>

class ChatDialog
{
//����ʵ��ʡ��...
public:
    ChatDialog()
    {
        std::cout << "ChatDialog constructor" << std::endl;
    }

    ~ChatDialog()
    {
        std::cout << "ChatDialog destructor" << std::endl;
    }

    void activate()
    {
        //ʵ��ʡ��
    }
};

//���ڹ�����������Ի����map��key�Ǻ���id��value��ChatDialog������Ի�������ָ��
std::map<int64_t, std::unique_ptr<ChatDialog>> m_ChatDialogs;

//C++ 17�汾2
void onDoubleClickFriendItem3(int64_t userid)
{   
    //�ṹ���󶨺�try_emplace���� C++17�﷨
    auto spChatDialog = std::make_unique<ChatDialog>();
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid, std::move(spChatDialog));
    iter->second->activate();
}

int main()
{
    //��������
    //906106643 ��userid
    onDoubleClickFriendItem3(906106643L);
    //906106644 ��userid
    onDoubleClickFriendItem3(906106644L);
    //906106643 ��userid
    onDoubleClickFriendItem3(906106643L);

    return 0;
}

