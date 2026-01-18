#pragma once

template <typename T>
class LockFreeQueue;
class FPSTimer;
class MmorpgServer : public NetServer
{
	struct Job
	{
		int sessionID;
		std::shared_ptr<byte[]> byteArr;
	};

public:
	MmorpgServer();
	~MmorpgServer();

	// 서버 시작할때
	bool Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession);
	// 서버 멈출 때
	void Stop();
	// accept 직후
	virtual bool OnConnectionRequest(const wchar_t* ipWstr, int portNum);
	// Accept 후 접속처리 완료 후 호출.
	virtual void OnClientJoin(int sessionID);
	// Release 후 호출
	virtual void OnClientLeave(int sessionID, LockFreeQueue<byte*>* releasePacket);
	// 패킷 수신 완료 후
	virtual int OnRecv(int sessionID, std::shared_ptr<byte[]> byteArr);
	// 패킷 송신 완료 후
	virtual int OnSend(int sessionID, LockFreeQueue<byte*>* releasePacket);
	virtual void OnError(const wchar_t* ipWstr, int portNum, int sessionID, int errorCode, const wchar_t* errorMsg);

private:
	static DWORD __stdcall LogicThread(void* pParameter);

	HANDLE m_hLogicLoop;
	LockFreeQueue<Job*>* m_logicJobQueue;
	FPSTimer* m_fpsTimer;
};