#include "Sockets.h"

#include <stdio.h>  // snprintf
#include <string.h>

//#include "../base/Platform.h"

#include "../base/AsyncLog.h"
#include "InetAddress.h"
#include "Endian.h"
#include "Callbacks.h"

using namespace net;

Socket::~Socket()
{
	sockets::close(sockfd_);
}

//bool Socket::getTcpInfo(struct tcp_info* tcpi) const
//{
//	//socklen_t len = sizeof(*tcpi);
//	//memset(tcpi, 0, len);
//	//return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
//    return false;
//}

bool Socket::getTcpInfoString(char* buf, int len) const
{
	//struct tcp_info tcpi;
	//bool ok = getTcpInfo(&tcpi);
	//if (ok)
	//{
	//	snprintf(buf, len, "unrecovered=%u "
	//		"rto=%u ato=%u snd_mss=%u rcv_mss=%u "
	//		"lost=%u retrans=%u rtt=%u rttvar=%u "
	//		"sshthresh=%u cwnd=%u total_retrans=%u",
	//		tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
	//		tcpi.tcpi_rto,          // Retransmit timeout in usec
	//		tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
	//		tcpi.tcpi_snd_mss,
	//		tcpi.tcpi_rcv_mss,
	//		tcpi.tcpi_lost,         // Lost packets
	//		tcpi.tcpi_retrans,      // Retransmitted packets out
	//		tcpi.tcpi_rtt,          // Smoothed round trip time in usec
	//		tcpi.tcpi_rttvar,       // Medium deviation
	//		tcpi.tcpi_snd_ssthresh,
	//		tcpi.tcpi_snd_cwnd,
	//		tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
	//}
	//return ok;
    return false;
}

void Socket::bindAddress(const InetAddress& addr)
{
	sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void Socket::listen()
{
	sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	int connfd = sockets::accept(sockfd_, &addr);
	if (connfd >= 0)
	{
		peeraddr->setSockAddrInet(addr);
	}
	return connfd;
}

void Socket::shutdownWrite()
{
	sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
	int optval = on ? 1 : 0;
#ifdef WIN32
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
#else
	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
#endif
	// FIXME CHECK
}

void Socket::setReuseAddr(bool on)
{
    sockets::setReuseAddr(sockfd_, on);
}

void Socket::setReusePort(bool on)
{
    sockets::setReusePort(sockfd_, on);
}

void Socket::setKeepAlive(bool on)
{
#ifdef WIN32
    //TODO: ��ȫWindows��д��
#else
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
#endif
	// FIXME CHECK
}

//namespace
//{
//	//typedef struct sockaddr SA;
//
//	
//
//}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
	return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
	return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
	return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

struct sockaddr_in* sockets::sockaddr_in_cast(struct sockaddr* addr)
{
	return static_cast<struct sockaddr_in*>(implicit_cast<void*>(addr));
}

SOCKET sockets::createOrDie()
{
#ifdef WIN32
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOGF("sockets::createNonblockingOrDie");
    }
#else
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOGF("sockets::createNonblockingOrDie");
    }
#endif

    return sockfd;
}

SOCKET sockets::createNonblockingOrDie()
{
#ifdef WIN32
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOGF("sockets::createNonblockingOrDie");
    }   
#else
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOGF("sockets::createNonblockingOrDie");
    } 
#endif

    setNonBlockAndCloseOnExec(sockfd);
	return sockfd;
}

void sockets::setNonBlockAndCloseOnExec(SOCKET sockfd)
{
#ifdef WIN32
    //��socket���óɷ�������
    unsigned long on = 1;
    ::ioctlsocket(sockfd, FIONBIO, &on);
#else
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    // FIXME check

    // close-on-exec
    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
    // FIXME check

    (void)ret;
#endif       
}

void sockets::bindOrDie(SOCKET sockfd, const struct sockaddr_in& addr)
{
	int ret = ::bind(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
	if (ret < 0)
	{
		LOGF("sockets::bindOrDie");
	}
}

void sockets::listenOrDie(SOCKET sockfd)
{
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0)
	{
		LOGF("sockets::listenOrDie");
	}
}

int sockets::accept(SOCKET sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#ifdef WIN32
    SOCKET connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
    setNonBlockAndCloseOnExec(connfd);
#else  
    SOCKET connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
	if (connfd < 0)
	{
#ifdef WIN32
        int savedErrno = ::WSAGetLastError();
        LOGSYSE("Socket::accept");
        if (savedErrno != WSAEWOULDBLOCK)
            LOGF("unexpected error of ::accept %d", savedErrno);		
#else
        int savedErrno = errno;
        LOGSYSE("Socket::accept");
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: // ???
        case EPERM:
        case EMFILE: // per-process lmit of open file desctiptor ???
            // expected errors
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            // unexpected errors
            LOGF("unexpected error of ::accept %d", savedErrno);
            break;
        default:
            LOGF("unknown error of ::accept %d", savedErrno);
            break;
    }
        
#endif
	}

	return connfd;
}

void sockets::setReuseAddr(SOCKET sockfd, bool on)
{
    int optval = on ? 1 : 0;
#ifdef WIN32
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)& optval, sizeof(optval));
#else
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
#endif
    // FIXME CHECK
}

void sockets::setReusePort(SOCKET sockfd, bool on)
{
    //Windows ϵͳû�� SO_REUSEPORT ѡ��
#ifndef WIN32
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on)
    {
        LOGSYSE("SO_REUSEPORT failed.");
    }
#endif
}

int sockets::connect(SOCKET sockfd, const struct sockaddr_in& addr)
{
	return ::connect(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
}

int32_t sockets::read(SOCKET sockfd, void *buf, int32_t count)
{
#ifdef WIN32
    return ::recv(sockfd, (char*)buf, count, 0);
#else
	return ::read(sockfd, buf, count);
#endif
}

#ifndef WIN32
ssize_t sockets::readv(SOCKET sockfd, const struct iovec *iov, int iovcnt)
{
	return ::readv(sockfd, iov, iovcnt);
}
#endif

int32_t sockets::write(SOCKET sockfd, const void *buf, int32_t count)
{
#ifdef WIN32
    return ::send(sockfd, (const char*)buf, count, 0);
#else
    return ::write(sockfd, buf, count);
#endif
    
}

void sockets::close(SOCKET sockfd)
{
#ifdef WIN32   
    if (::closesocket(sockfd) < 0)
#else
    if (::close(sockfd) < 0)
#endif
    {
        LOGSYSE("sockets::close, fd=%d, errno=%d, errorinfo=%s", sockfd, errno, strerror(errno));
    } 
}

void sockets::shutdownWrite(SOCKET sockfd)
{
#ifdef WIN32
    if (::shutdown(sockfd, SD_SEND) < 0)	
#else
    if (::shutdown(sockfd, SHUT_WR) < 0)
#endif        
	{
		LOGSYSE("sockets::shutdownWrite");
	}
}

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
    //if (size >= sizeof(struct sockaddr_in))
    //    return;

	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
	size_t end = ::strlen(buf);
	uint16_t port = sockets::networkToHost16(addr.sin_port);
    //if (size > end)
    //    return;

	snprintf(buf + end, size - end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr_in& addr)
{
    if (size >= sizeof(struct sockaddr_in))
        return;

    ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
    //TODO: У����д�ĶԲ���
#ifdef WIN32
    addr->sin_port = htons(port);
#else
	addr->sin_port = htobe16(port);
#endif
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		LOGSYSE("sockets::fromIpPort");
	}
}

int sockets::getSocketError(SOCKET sockfd)
{
	int optval;
#ifdef WIN32
    int optvallen = sizeof(optval);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optvallen) < 0)
        return ::WSAGetLastError();
#else
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);

	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
		return errno;
#endif
	return optval;
}

struct sockaddr_in sockets::getLocalAddr(SOCKET sockfd)
{
    struct sockaddr_in localaddr = { 0 };
	memset(&localaddr, 0, sizeof localaddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    ::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen);
	//if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
	//{
	//	LOG_SYSERR << "sockets::getLocalAddr";
    //  return 
	//}
	return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(SOCKET sockfd)
{
    struct sockaddr_in peeraddr = { 0 };
	memset(&peeraddr, 0, sizeof peeraddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    ::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen);
	//if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
	//{
	//	LOG_SYSERR << "sockets::getPeerAddr";
	//}
	return peeraddr;
}

bool sockets::isSelfConnect(SOCKET sockfd)
{
	struct sockaddr_in localaddr = getLocalAddr(sockfd);
	struct sockaddr_in peeraddr = getPeerAddr(sockfd);
	return localaddr.sin_port == peeraddr.sin_port && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}
