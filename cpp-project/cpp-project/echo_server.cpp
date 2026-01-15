#include "stdafx.h"
#include "net_server.h"
#include "echo_server.h"
#include "player.h"

EchoServer::EchoServer()
	: NetServer()
{

}

EchoServer::~EchoServer()
{

}

// 서버 시작할때
bool EchoServer::Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession)
{
	if (NetServer::Start(ipWstr, portNum, workerCreateCnt, maxSession) == false)
	{
		return false;
	}

	return true;
}

// 서버 멈출 때
void EchoServer::Stop()
{

	NetServer::Stop();
}

// accept 직후
bool EchoServer::OnConnectionRequest(const wchar_t* ipWstr, int portNum)
{
	cout << "OnConnectionRequest : IP " << ipWstr << ":" << portNum << endl;
	return true;
}

// Accept 후 접속처리 완료 후 호출.
void EchoServer::OnClientJoin(int sessionID, void* pObject)
{
	cout << "OnClientJoin " << endl;

	Player* player = new Player();
	player->SetSessionID(sessionID);
	SetObject(sessionID, player);
}

// Release 후 호출
void EchoServer::OnClientLeave(int sessionID, void* pObject)
{
	cout << "OnClientLeave " << endl;
	delete (Player*)pObject;
}

// 패킷/메시지 수신 완료 후
int EchoServer::OnRecv(int sessionID, void* pObject, std::shared_ptr<byte[]> byteArr)
{
	cout << "OnRecv " << endl;
	if (SendPacket(sessionID, byteArr.get()) == false)
	{
		cout << "OnRecv Send Fail" << endl;
	}
	return 0;
}

int EchoServer::OnSend(int sessionID, void* pObject, byte* byteArr)
{
	cout << "OnSend " << endl;
	return 0;
}

// 에러일 때 호출되는 함수
void EchoServer::OnError(const wchar_t* ipWstr, int portNum, int sessionID, void* pObject, int errorCode, const wchar_t* errorMsg)
{
	cout << "OnError : IP " << ipWstr << ":" << portNum << " ErrorCode : " << errorCode << " " << errorMsg << endl;
}