#ifndef SOCKETPOSESERVICE_H
#define SOCKETPOSESERVICE_H

#include "TinyTcpServer.h"
#include <functional>

class SocketPoseService
{
public:
	SocketPoseService();
	~SocketPoseService();

	void getPose(float pos[3], float rot[3]);

private:
	void onConnect(int session);
	void onDisconnect(int session);
	void onRecv(int session, const char *buf, int len);

	float mPos[3] = {0.0, 0.0, 0.0};
	float mRot[3] = {0.0, 0.0, 0.0};
	TinyTcpServer mServer;

	OnConnect onConnectCB = nullptr;
	OnDisconnect onDisconnectCB = nullptr;
	OnRecv onRecvCB = nullptr;
};

#endif // SOCKETPOSESERVICE_H
