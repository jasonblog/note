/** 
 * ��֤ epoll EPOLLONESHOT ѡ�epoll_server_with_oneshot.cpp
 * zhangyl 2019.04.01
 */
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<poll.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<errno.h>
#include<iostream>

int main()
{
    //����һ������socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
        return -1;
    }

    //��������ip��ַ�Ͷ˿ں�
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));


    //������socker����Ϊ��������
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        std::cout << "set listenfd to nonblock error" << std::endl;
        return -1;
    }

    //��ʼ����������ַ
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);

    if (bind(listenfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socker error." << std::endl;
        close(listenfd);
        return -1;
    }

    //��������
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }


    //����epollfd
    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "create epollfd error." << std::endl;
        close(listenfd);
        return -1;
    }

    epoll_event listen_fd_event;
    listen_fd_event.data.fd = listenfd;
    listen_fd_event.events = EPOLLIN;
	
    //������sokcet�󶨵�epollfd��ȥ
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
    {
        std::cout << "epoll_ctl error" << std::endl;
        close(listenfd);
        return -1;
    }

    int n;
    while (true)
    {
        epoll_event epoll_events[1024];
        n = epoll_wait(epollfd, epoll_events, 1024, 1000);
        if (n < 0)
        {
            //���ź��ж�
            if (errno == EINTR) 
                continue;

            //����,�˳�
            break;
        }
        else if (n == 0)
        {
            //��ʱ,����
            continue;
        }
        for (size_t i = 0; i < n; ++i)
        {
            //�¼��ɶ�
            if (epoll_events[i].events & EPOLLIN)
            {
                if (epoll_events[i].data.fd == listenfd)
                {
                    //����socket,����������
                    struct sockaddr_in clientaddr;
                    socklen_t clientaddrlen = sizeof(clientaddr);
                    int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientaddrlen);
                    if (clientfd != -1)
                    {
                        int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
                        int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
                        {
                            close(clientfd);
                            std::cout << "set clientfd to nonblocking error." << std::endl;
                        }
                        else
                        {
                            epoll_event client_fd_event;
                            client_fd_event.data.fd = clientfd;
                            client_fd_event.events = EPOLLIN;
							//��clientfd����EPOLLONESHOTѡ��
							client_fd_event.events |= EPOLLONESHOT;
                            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_fd_event) != -1)
                            {
                                std::cout << "new client accepted,clientfd: " << clientfd << std::endl;
                            }
                            else
                            {
                                std::cout << "add client fd to epollfd error" << std::endl;
                                close(clientfd);
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "client fd: " << epoll_events[i].data.fd << " recv data." << std::endl;
                    //��ͨclientfd
                    char ch;
                    //ÿ��ֻ��һ���ֽ�
                    int m = recv(epoll_events[i].data.fd, &ch, 1, 0);
                    if (m == 0)
                    {
                        //�Զ˹ر������ӣ���epollfd���Ƴ�clientfd
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                        {
                            std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd << std::endl;
                        }
                        close(epoll_events[i].data.fd);
                    }
                    else if (m < 0)
                    {
                        //����
                        if (errno != EWOULDBLOCK && errno != EINTR)
                        {
                            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                            {
                                std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd << std::endl;
                            }
                            close(epoll_events[i].data.fd);
                        }
                    }
                    else
                    {
                        //�����յ�����
                        std::cout << "recv from client:" << epoll_events[i].data.fd << ", " << ch << std::endl;
						
						//�������ٴ�Ϊclientfd�ٴ�ע��EPOLLIN�¼�
						//epoll_event client_fd_event;
						//client_fd_event.data.fd = epoll_events[i].data.fd;
						//client_fd_event.events = EPOLLIN;					
						//if (epoll_ctl(epollfd, EPOLL_CTL_MOD, epoll_events[i].data.fd, &client_fd_event) != -1)
						//{
						//	std::cout << "rearm EPOLLIN event to clientfd: " << epoll_events[i].data.fd << std::endl;
						//}
						//else
						//{
						//	if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
						//	{
						//		std::cout << "remove clientfd from epoll fd successfully, clientfd:" << epoll_events[i].data.fd << std::endl;
						//	}
						//	close(epoll_events[i].data.fd);
						//}
                    }
                }
            }
            else if (epoll_events[i].events & POLLERR)
            {
                // TODO �ݲ�����
            }
        }
    }

    close(listenfd);
    return 0;
}
