#include <my_assert.h>
#include "socket.h"
#include "net_addr.h"

CSocket::CSocket(SOCKET socket)
	: m_Socket(socket)
{

}

CSocket::CSocket(int32 nType/*=SOCK_STREAM*/, int32 nProtocolFamily/*=AF_INET*/, int32 nProtocol/*=0*/)
	: m_Socket(INVALID_SOCKET),
	  m_nType(nType),
	  m_nProtocolFamily(nProtocolFamily),
	  m_nProtocol(nProtocol)
{
}

CSocket::~CSocket()
{
	Close();
}

//----------------------------------------------------------------
bool CSocket::Open()
{
	m_Socket = CreateSocket(m_nType, m_nProtocolFamily, m_nProtocol);
	if (INVALID_SOCKET == m_Socket) {
		MY_ASSERT_STR(false, return false, "CreateSocket failed with error code %d \n", PpeGetLastError());
	}
	return true;
}

//-------------------------tools--------------------------------
SOCKET CSocket::CreateSocket(int32 Type, int32 ProtocolFamily, int32 Protocol)
{
	SOCKET Socket = socket(ProtocolFamily, Type, Protocol);//¥¥Ω®socket
	if (Socket == INVALID_SOCKET) {
		MY_ASSERT_STR(false, return INVALID_SOCKET, "socket failed with error code %d .", PpeGetLastError());
	}
	int iVal = 1;
	if (SOCKET_ERROR ==
		setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&iVal), sizeof(iVal))) {
		MY_ASSERT_STR(false, return INVALID_SOCKET, "setsockopt failed with error code %d .", PpeGetLastError());
	}
	return Socket;
}

//-----------------------------------------------------------------
void CSocket::Close()
{
	if (m_Socket != INVALID_SOCKET) {
		CloseSocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::Shutdown()
{
	if (m_Socket != INVALID_SOCKET) {
		shutdown(m_Socket, SHUT_RDWR);
		CloseSocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownRead()
{
	if (m_Socket != INVALID_SOCKET) {
		shutdown(m_Socket, SHUT_RD);
	}
}

//-----------------------------------------------------------------
void CSocket::ShutdownWrite()
{
	if (m_Socket != INVALID_SOCKET) {
		shutdown(m_Socket, SHUT_WR);
	}
}

//-----------------------------------------------------------------
void CSocket::SetNonblocking()
{
	//ArkAst(m_oSocket != INVALID_SOCKET);
	MakeSocketNonblocking(m_Socket);
}

//-----------------------------------------------------------------
bool CSocket::GetLocalAddress(CNetAddr &addr) const
{
	if (INVALID_SOCKET == m_Socket)
		return false;
	sockaddr_in asiAddress;

#ifdef GH_OS_WIN32
	int32 nSize=0;
#else
	socklen_t nSize = 0;
#endif
	nSize = sizeof(asiAddress);
	memset(&asiAddress, 0, nSize);
	//int nErrorCode;
	if (getsockname(m_Socket, reinterpret_cast<sockaddr *>(&asiAddress), &nSize)) {
		//nErrorCode=ArkGetLastError();
		return false;
	}
	addr.SetPort(ntohs(asiAddress.sin_port));
	addr.SetAddress(inet_ntoa(asiAddress.sin_addr));
	return true;
}

//-----------------------------------------------------------------
bool CSocket::GetRemoteAddress(CNetAddr &add) const
{
	if (INVALID_SOCKET == m_Socket)
		return false;
	sockaddr_in asiAddress;

#ifdef GH_OS_WIN32
	int32 nSize=0;
#else
	socklen_t nSize = 0;
#endif
	nSize = sizeof(asiAddress);
	memset(&asiAddress, 0, sizeof(asiAddress));
	if (getpeername(m_Socket, reinterpret_cast<sockaddr *>(&asiAddress), &nSize)) {
		return false;
	}
	add.SetPort(ntohs(asiAddress.sin_port));
	add.SetAddress(inet_ntoa(asiAddress.sin_addr));
	return true;
}

//-----------------------------------------------------------------
int CSocket::GetSocketError() const
{
	int nError = PpeGetLastError();
	EPipeConnFailedReason eReason;
	switch (nError) {
#if defined(GH_OS_WIN32)
		case EWOULDBLOCK:
#else
	case EINPROGRESS:
#endif
		break;
#ifdef GH_OS_WIN32
		case ENOBUFS:
			eReason = ePCFR_NOBUFFER;
#endif
	case EADDRINUSE:eReason = ePCFR_LOCALADDRINUSE;
	case ECONNREFUSED:eReason = ePCFR_REFUSED;
	case ETIMEDOUT:eReason = ePCFR_TIMEDOUT;
	case ENETUNREACH:eReason = ePCFR_UNREACH;
	case ECONNRESET:eReason = ePCFR_RESET;
	default:MY_ASSERT_STR(false, DO_NOTHING, "unknown failed!!");
	}
	return eReason;

}

//-----------------------------------------------------------------
uint32 CSocket::Bind(const CNetAddr &addr)
{
	//ArkAst(m_oSocket != INVALID_SOCKET);

	sockaddr_in saiAddress;
	Address2SockAddrIn(saiAddress, addr);
	//bind
	if (bind(m_Socket, reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr))) {
		CloseSocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return PpeGetLastError();
	}
	return 0;
}

//--------------------------------- *tools* -------------------------------------------------------------------
void CSocket::Address2SockAddrIn(sockaddr_in &saiAddress, const CNetAddr &address)
{
	memset(&saiAddress, 0, sizeof(saiAddress));
	saiAddress.sin_addr.s_addr = inet_addr(address.GetAddress());
	saiAddress.sin_port = htons(static_cast<u_short>(address.GetPort()));
	saiAddress.sin_family = AF_INET;
}

//-----------------------------------------------------------------
SOCKET CSocket::CreateBindedSocket(const CNetAddr &Address)
{
	SOCKET Socket = CreateSocket();

	sockaddr_in saiAddress;
	Address2SockAddrIn(saiAddress, Address);
	//bind
	if (bind(Socket, reinterpret_cast<sockaddr *>(&saiAddress), sizeof(sockaddr))) {
		CloseSocket(Socket);
		return PpeGetLastError();
	}
	return Socket;
}

//-----------------------------------------------------------------
void CSocket::MakeSocketNonblocking(SOCKET Socket)
{
	if (-1 == evutil_make_socket_nonblocking(static_cast<int>(Socket))) {
		CloseSocket(Socket);
		MY_ASSERT_STR(false, return, "ioctlsocket failed with error code %d.", PpeGetLastError());
	}
}