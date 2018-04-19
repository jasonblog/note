#include "SocketPoseClient.h"

#include <string.h>

SocketPoseClient::SocketPoseClient() 
{
	const std::string hostname = "127.0.0.1";
	int port = 7000;
	printf("start client, connect to %s:%d\n", hostname.c_str(), port);

	onConnectCB = std::bind(&SocketPoseClient::onConnect, this, std::placeholders::_1);
	mClient.setOnConnect(onConnectCB);
	onDisconnectCB = std::bind(&SocketPoseClient::onDisconnect, this, std::placeholders::_1);
	mClient.setOnDisconnect(onDisconnectCB);
	onRecvCB = std::bind(&SocketPoseClient::onRecv, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	mClient.setOnRecv(onRecvCB);
	mClient.start(hostname, 7000);
}

SocketPoseClient::~SocketPoseClient() 
{
	mClient.stop();
}

inline void opencv2opengl(float pos[3], float euler[3])
{
	pos[0] = -pos[0];
	pos[1] = -pos[1];
	//pos[2] = pos[2];
	euler[0] = -euler[0];
	euler[1] = -euler[1];
	euler[2] = -euler[2];
}

void SocketPoseClient::sendPose(float pos[3], float euler[3], bool cv2gl)
{
	if (cv2gl) {
		float scale = 100.0;
		opencv2opengl(pos, euler);
		pos[0] = pos[0] * scale;
		pos[1] = pos[1] * scale;
		pos[2] = pos[2] * scale;
	}

	char buf[64];
	sprintf(buf, "%f, %f, %f, %f, %f, %f", pos[0], pos[1], pos[2], euler[0], euler[1], euler[2]);
	mClient.send(buf, (int)strlen(buf));
}

void SocketPoseClient::onConnect(int session)
{
	printf("onConnect\n");
}

void SocketPoseClient::onDisconnect(int session)
{
	printf("onDisconnect\n");
}

void SocketPoseClient::onRecv(int session, const char *buf, int len)
{
	//printf("session=%d, len=%d, buf=%s\n", session, len, buf);
	printf("Server: %s\n", buf);
}
