#include "stdafx.h"
#include "net_server.h"
#include "echo_server.h"

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
	return true;
}

// Accept 후 접속처리 완료 후 호출.
void EchoServer::OnClientJoin(int SessionID, void* pObject)
{

}

// Release 후 호출
void EchoServer::OnClientLeave(int SessionID, void* pObject)
{
}

// 패킷/메시지 수신 완료 후
int EchoServer::OnRecv(int SessionID, void* pObject, std::shared_ptr<byte[]> byteArr)
{
	return 0;
}

int EchoServer::OnSend(int SessionID, void* pObject, byte* byteArr)
{
	return 0;
}

// 에러일 때 호출되는 함수
void EchoServer::OnError(const wchar_t* ipWstr, int portNum, int sessionID, void* pObject, int errorCode, const wchar_t* errorMsg)
{

}