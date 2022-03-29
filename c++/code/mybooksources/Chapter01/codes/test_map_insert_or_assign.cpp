/** 
 * std::map::try_emplace�÷���ʾ
 * zhangyl 2019.10.06
 */

#include <iostream>
#include <map>

class ChatDialog
{
//����ʵ��ʡ��...
public:
    ChatDialog(int64_t userid) : m_userid(userid)
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

private:
    int64_t     m_userid;
};

//���ڹ�����������Ի����map��key�Ǻ���id��value��ChatDialog������Ի�������ָ��
std::map<int64_t, ChatDialog>   m_ChatDialogs;

//C++ 17�汾3
void onDoubleClickFriendItem3(int64_t userid)
{   
    //�ṹ���󶨺�try_emplace���� C++17�﷨    
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid, userid);   
    iter->second.activate();
}

//int main()
//{
//    //��������
//    //906106643 ��userid
//    onDoubleClickFriendItem3(906106643L);
//    //906106644 ��userid
//    onDoubleClickFriendItem3(906106644L);
//    //906106643 ��userid
//    onDoubleClickFriendItem3(906106643L);
//
//    return 0;
//}

int main()
{
    std::map<std::string, int> mapUsersAge{ { "Alex", 45 }, { "John", 25 } };
    mapUsersAge.insert_or_assign("Tom", 26);
    mapUsersAge.insert_or_assign("Alex", 27);

    for (const auto& [userName, userAge] : mapUsersAge)
    {
        std::cout << "userName: " << userName << ", userAge: " << userAge << std::endl;
    }
}

