#pragma once

class CSerializeBuffer; 
class NetServer
{
protected:
	NetServer();
	~NetServer();

public:
	bool Init();
	void Release();
	// 오픈 IP / 포트 / 워커스레드 수(생성수, 러닝수) / 나글옵션 / 최대접속자 수
	bool Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt);
	void Stop();

	// accept 직후
	virtual bool OnConnectionRequest(const wchar_t* ipWstr, int portNum) = 0;

	// Accept 후 접속처리 완료 후 호출.
	virtual void OnClientJoin(unsigned long long SessionID) = 0;
	// Release 후 호출
	virtual void OnClientLeave(unsigned long long SessionID) = 0;

	// 패킷 수신 완료 후
	virtual int OnRecv(unsigned long long SessionID, CSerializeBuffer* pPacket) = 0;

	virtual void OnError(const wchar_t* ipWstr, int portNum, int errorCode, const wchar_t* errorMsg) = 0;

private:
	static unsigned __stdcall monitoringThread(void* pParameter);
	static unsigned __stdcall netAcceptThread(void* pParameter);
	static unsigned __stdcall netIOThread(void* pParameter);

private:
	SOCKET		m_listenSocket;
	sockaddr_in m_listenInfo;
	HANDLE		m_hIOCP;
	HANDLE		m_hAcceptThread;
	HANDLE		m_hMonitoringThread;
	HANDLE*		m_hIOThread;

	wchar_t m_ipWstr[20];
	int m_portNum;
	bool m_bNoDelayOpt;
	bool m_bRSTOpt;
	bool m_bKeepAliveOpt;
	bool m_bOverlappedSend;
	int m_MaxSession;
	int m_workerCreateCnt;
	int m_workerRunningCnt;
};