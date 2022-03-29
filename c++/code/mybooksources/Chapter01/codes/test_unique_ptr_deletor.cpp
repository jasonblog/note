#include <iostream>
#include <memory>

class Socket
{
public:
    Socket()
    {

    }

    ~Socket()
    {

    }

    //�ر���Դ���
    void close()
    {

    }
};

int main()
{
    auto deletor = [](Socket* pSocket) {
        //�رվ��
        pSocket->close();
        //TODO: �����������������ӡһ����־...
        delete pSocket;
    };

    //std::unique_ptr<Socket, void(*)(Socket * pSocket)> spSocket(new Socket(), deletor);
    std::unique_ptr<Socket, decltype(deletor)> spSocket(new Socket(), deletor);

    return 0;
}