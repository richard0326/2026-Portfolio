#include "stdafx.h"
#include "net_server.h"
#include "mmorpg_server.h"
#include "lock_free_queue.h"

MmorpgServer::MmorpgServer()
	: NetServer()
{

}

MmorpgServer::~MmorpgServer()
{

}

// 서버 시작할때
bool MmorpgServer::Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession)
{
	srand(time(0));
	if (NetServer::Start(ipWstr, portNum, workerCreateCnt, maxSession) == false)
	{
		return false;
	}

	return true;
}

// 서버 멈출 때
void MmorpgServer::Stop()
{
	NetServer::Stop();
}

// accept 직후
bool MmorpgServer::OnConnectionRequest(const wchar_t* ipWstr, int portNum)
{
	cout << "OnConnectionRequest : IP  " << ipWstr << ", Port" << portNum << endl;
	return true;
}

// Accept 후 접속처리 완료 후 호출.
void MmorpgServer::OnClientJoin(int sessionID, void* pObject)
{
	cout << "OnClientJoin " << endl;
	//byte* pByte = mpCreateMyCharacter(rand() % 8, rand() % dfRANGE_MOVE_RIGHT, rand() % dfRANGE_MOVE_BOTTOM);
	//if (SendPacket(sessionID, pByte) == false)
	{

	}
}

// Release 후 호출
void MmorpgServer::OnClientLeave(int sessionID, void* pObject, LockFreeQueue<byte*>* releasePacket)
{
	cout << "OnClientLeave " << endl;

	byte* pDeletePacket = nullptr;
	while (releasePacket->Dequeue(&pDeletePacket))
	{
		delete pDeletePacket;
	}
}

// 패킷 수신 완료 후
int MmorpgServer::OnRecv(int sessionID, void* pObject, std::shared_ptr<byte[]> byteArr)
{
	return 0;
}

// 패킷 송신 완료 후
int MmorpgServer::OnSend(int sessionID, void* pObject, LockFreeQueue<byte*>* releasePacket)
{
	byte* pDeletePacket = nullptr;
	while (releasePacket->Dequeue(&pDeletePacket))
	{
		delete pDeletePacket;
	}

	return 0;
}

void MmorpgServer::OnError(const wchar_t* ipWstr, int portNum, int sessionID, void* pObject, int errorCode, const wchar_t* errorMsg)
{
	cout << "OnError : IP " << ipWstr << ":" << portNum << " ErrorCode : " << errorCode << " " << errorMsg << endl;
}