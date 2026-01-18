#include "stdafx.h"
#include "net_server.h"
#include "mmorpg_server.h"
#include "player.h"
#include "fps_timer.h"
#include "lock_free_queue.h"

MmorpgServer::MmorpgServer()
	: NetServer()
{
	m_fpsTimer = new FPSTimer();
	m_fpsTimer->Init(50);
	m_logicJobQueue = new LockFreeQueue<Job*>();
}

MmorpgServer::~MmorpgServer()
{
	delete m_logicJobQueue;
	m_fpsTimer->Release();
	delete m_fpsTimer;
}

// 서버 시작할때
bool MmorpgServer::Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession)
{
	srand(time(0));
	if (NetServer::Start(ipWstr, portNum, workerCreateCnt, maxSession) == false)
	{
		return false;
	}

	m_hLogicLoop = CreateThread(nullptr, 0, MmorpgServer::LogicThread, this, 0, nullptr);

	return true;
}

// 서버 멈출 때
void MmorpgServer::Stop()
{
	TerminateThread(m_hLogicLoop, 0);
	NetServer::Stop();
}

// accept 직후
bool MmorpgServer::OnConnectionRequest(const wchar_t* ipWstr, int portNum)
{
	cout << "OnConnectionRequest : IP  " << ipWstr << ", Port " << portNum << endl;
	return true;
}

// Accept 후 접속처리 완료 후 호출.
void MmorpgServer::OnClientJoin(int sessionID)
{
	cout << "OnClientJoin " << endl;
	byte* pByte = mpCreateMyCharacter(sessionID, 0, rand() % dfRANGE_MOVE_RIGHT, rand() % dfRANGE_MOVE_BOTTOM, 100);
	if (SendPacket(sessionID, pByte) == false)
	{
		cout << "SendPacket Fail" << endl;
	}
}

// Release 후 호출
void MmorpgServer::OnClientLeave(int sessionID, LockFreeQueue<byte*>* releasePacket)
{
	cout << "OnClientLeave " << endl;

	byte* pDeletePacket = nullptr;
	while (releasePacket->Dequeue(&pDeletePacket))
	{
		delete pDeletePacket;
	}
}

// 패킷 수신 완료 후
int MmorpgServer::OnRecv(int sessionID, std::shared_ptr<byte[]> byteArr)
{
	Job* pJob = new Job();
	pJob->sessionID = sessionID;
	pJob->byteArr = byteArr;
	if (m_logicJobQueue->Enqueue(pJob) == false)
	{
		cout << "Job fail" << endl;
	}

	return 0;
}

// 패킷 송신 완료 후
int MmorpgServer::OnSend(int sessionID, LockFreeQueue<byte*>* releasePacket)
{
	byte* pDeletePacket = nullptr;
	while (releasePacket->Dequeue(&pDeletePacket))
	{
		delete pDeletePacket;
	}

	return 0;
}

void MmorpgServer::OnError(const wchar_t* ipWstr, int portNum, int sessionID, int errorCode, const wchar_t* errorMsg)
{
	cout << "OnError : IP " << ipWstr << ":" << portNum << " ErrorCode : " << errorCode << " " << errorMsg << endl;
}

DWORD __stdcall MmorpgServer::LogicThread(void* pParameter)
{
	MmorpgServer* pThis = (MmorpgServer*)pParameter;
	while (true)
	{
		pThis->m_fpsTimer->DontSkipRendering();

		Job* outJob;
		while (pThis->m_logicJobQueue->Dequeue(&outJob))
		{
			int type = -1;
			memcpy(&type, outJob->byteArr.get() + sizeof(unsigned int), sizeof(int));

			switch (0)
			{
			case dfPACKET_MOVE_DIR_LL:
				break;
			case dfPACKET_CS_MOVE_START:
				netPacketProc_MoveStart(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			case dfPACKET_CS_MOVE_STOP:
				netPacketProc_MoveStop(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			case dfPACKET_CS_ATTACK1:
				netPacketProc_Attack1(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			case dfPACKET_CS_ATTACK2:
				netPacketProc_Attack2(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			case dfPACKET_CS_ATTACK3:
				netPacketProc_Attack3(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			case dfPACKET_CS_ECHO:
				netPacketProc_Echo(nullptr, outJob->byteArr.get() + sizeof(unsigned int) + sizeof(type));
				break;
			}
		}
	}
}