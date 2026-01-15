#pragma once
#include "error.h"

template <typename T>
class LockFreeStack;
class Session;
class NetServer
{
protected:
	NetServer();
	~NetServer();

public:
	bool Init();
	void Release();
	// 오픈 IP / 포트 / 워커스레드 수(생성수, 러닝수) / 나글옵션 / 최대접속자 수
	bool Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession);
	void Stop();

	// accept 직후
	virtual bool OnConnectionRequest(const wchar_t* ipWstr, int portNum) = 0;

	// Accept 후 접속처리 완료 후 호출.
	virtual void OnClientJoin(int SessionID, void* pObject) = 0;
	// Release 후 호출
	virtual void OnClientLeave(int SessionID, void* pObject) = 0;

	// 패킷 수신 완료 후
	virtual int OnRecv(int sessionID, void* pObject, std::shared_ptr<byte[]> byteArr) = 0;
	// 패킷 송신 완료 후
	virtual int OnSend(int sessionID, void* pObject, byte* byteArr) = 0;

	virtual void OnError(const wchar_t* ipWstr, int portNum, int sessionID, void* pObject, int errorCode, const wchar_t* errorMsg) = 0;

private:
	static unsigned __stdcall monitoringThread(void* pParameter);
	static unsigned __stdcall netAcceptThread(void* pParameter);
	static unsigned __stdcall netIOThread(void* pParameter);

	ErrorCode netWSARecvPacket(Session* pSession);  // IOCP 패킷을 읽어드리는 함수
	ErrorCode netWSARecvPost(Session* pSession);	// WSA Recv를 호출하는 함수
	ErrorCode netWSASendPacket(int SessionID, byte* byteArr);	// 패킷(byte 배열)을 보내는 함수
	ErrorCode netWSASendPost(Session* pSession);	// WSA Send를 호출하는 함수
	void DisconnectSession(int SessionID);
	void DisconnectSession(Session* pSession, bool isLock = false);

	Session* AcquireLock(int SessionID);
	void ReleaseLock(Session* pSession);
	void DecreaseIOCount(Session* pSession);

	unsigned int GetPacketLen(byte* byteArr);
private:
	SOCKET		m_listenSocket;
	sockaddr_in m_listenInfo;
	HANDLE		m_hIOCP;
	HANDLE		m_hAcceptThread;
	HANDLE		m_hMonitoringThread;
	HANDLE*		m_hIOThread;

	vector<Session*> m_sessionVec;
	wchar_t m_ipWstr[20];
	int m_portNum;
	int m_maxSession;
	int m_workerCreateCnt;
	LockFreeStack<int>* m_indexStack;

	static const int WsaSendBufferSize = 200;
};