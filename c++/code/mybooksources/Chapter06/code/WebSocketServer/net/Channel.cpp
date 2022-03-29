#include "Channel.h"
#include <sstream>

#include "../base/Platform.h"
#include "../base/AsyncLog.h"
#include "Poller.h"
#include "EventLoop.h"

using namespace net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = XPOLLIN | XPOLLPRI;
const int Channel::kWriteEvent = XPOLLOUT;

Channel::Channel(EventLoop* loop, int fd__): loop_(loop),
                                            fd_(fd__),
                                            events_(0),
                                            revents_(0),
                                            index_(-1),
                                            logHup_(true),
                                            tied_(false)/*,
                                            eventHandling_(false),
                                            addedToLoop_(false)
                                            */
{
}

Channel::~Channel()
{
	//assert(!eventHandling_);
	//assert(!addedToLoop_);
	if (loop_->isInLoopThread())
	{
		//assert(!loop_->hasChannel(this));
	}
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
	tie_ = obj;
	tied_ = true;
}

bool Channel::enableReading() 
{ 
    events_ |= kReadEvent;
    return update();
}

bool Channel::disableReading()
{
    events_ &= ~kReadEvent; 
    
    return update();
}

bool Channel::enableWriting() 
{
    events_ |= kWriteEvent; 
    
    return update(); 
}

bool Channel::disableWriting()
{ 
    events_ &= ~kWriteEvent; 
    return update();
}

bool Channel::disableAll()
{ 
    events_ = kNoneEvent; 
    return update(); 
}

bool Channel::update()
{
	//addedToLoop_ = true;
	return loop_->updateChannel(this);
}

void Channel::remove()
{
	if (!isNoneEvent())
        return;
	//addedToLoop_ = false;
	loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	std::shared_ptr<void> guard;
	if (tied_)
	{
		guard = tie_.lock();
		if (guard)
		{
			handleEventWithGuard(receiveTime);
		}
	}
	else
	{
		handleEventWithGuard(receiveTime);
	}
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
	//eventHandling_ = true;
    /*
    XPOLLIN �����¼�
    XPOLLPRI�����¼�������ʾ�������ݣ�����tcp socket�Ĵ�������
    POLLRDNORM , ���¼�����ʾ����ͨ���ݿɶ�������
    POLLRDBAND ,�����¼�����ʾ���������ݿɶ���������
    XPOLLOUT��д�¼�
    POLLWRNORM , д�¼�����ʾ����ͨ���ݿ�д
    POLLWRBAND ,��д�¼�����ʾ���������ݿ�д������   ��������
    XPOLLRDHUP (since Linux 2.6.17)��Stream socket��һ�˹ر������ӣ�ע����stream socket������֪������raw socket,dgram socket����������д�˹ر������ӣ����Ҫʹ������¼������붨��_GNU_SOURCE �ꡣ����¼����������ж���·�Ƿ����쳣����Ȼ��ͨ�õķ�����ʹ���������ƣ���Ҫʹ������¼�������������ͷ�ļ���
    ����#define _GNU_SOURCE
      ����#include <poll.h>
    XPOLLERR���������ں����ô�������revents����ʾ�豸��������
    XPOLLHUP���������ں����ô�������revents����ʾ�豸���������poll������fd��socket����ʾ���socket��û���������Ͻ������ӣ�����˵ֻ������socket()����������û�н���connect��
    XPOLLNVAL���������ں����ô�������revents����ʾ�Ƿ������ļ�������fdû�д�
    */
	LOGD(reventsToString().c_str());
	if ((revents_ & XPOLLHUP) && !(revents_ & XPOLLIN))
	{
		if (logHup_)
		{
			LOGW("Channel::handle_event() XPOLLHUP");
		}
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & XPOLLNVAL)
	{
		LOGW("Channel::handle_event() XPOLLNVAL");
	}

	if (revents_ & (XPOLLERR | XPOLLNVAL))
	{
		if (errorCallback_) 
            errorCallback_();
	}
    
	if (revents_ & (XPOLLIN | XPOLLPRI | XPOLLRDHUP))
	{
		//��������socketʱ��readCallback_ָ��Acceptor::handleRead
        //���ǿͻ���socketʱ������TcpConnection::handleRead 
        if (readCallback_) 
            readCallback_(receiveTime);
	}

	if (revents_ & XPOLLOUT)
	{
		//���������״̬����socket����writeCallback_ָ��Connector::handleWrite()
        if (writeCallback_) 
            writeCallback_();
	}
	//eventHandling_ = false;
}

string Channel::reventsToString() const
{
	std::ostringstream oss;
	oss << fd_ << ": ";
	if (revents_ & XPOLLIN)
		oss << "IN ";
	if (revents_ & XPOLLPRI)
		oss << "PRI ";
	if (revents_ & XPOLLOUT)
		oss << "OUT ";
	if (revents_ & XPOLLHUP)
		oss << "HUP ";
	if (revents_ & XPOLLRDHUP)
		oss << "RDHUP ";
	if (revents_ & XPOLLERR)
		oss << "ERR ";
	if (revents_ & XPOLLNVAL)
		oss << "NVAL ";

	return oss.str().c_str();
}
