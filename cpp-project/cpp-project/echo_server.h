#pragma once

class SerializeBuffer;
class EchoServer : public NetServer
{
public:
	EchoServer();
	~EchoServer();

	// 서버 시작할때
	bool Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession);

	// 서버 멈출 때
	void Stop();

	// accept 직후
	virtual bool OnConnectionRequest(const wchar_t* ipWstr, int portNum);

	// Accept 후 접속처리 완료 후 호출.
	virtual void OnClientJoin(int SessionID, void* pObject);
	// Release 후 호출
	virtual void OnClientLeave(int SessionID, void* pObject);

	// 패킷 수신 완료 후
	virtual int OnRecv(int sessionID, void* pObject, std::shared_ptr<byte[]> byteArr);
	// 패킷 송신 완료 후
	virtual int OnSend(int sessionID, void* pObject, byte* byteArr);

	virtual void OnError(const wchar_t* ipWstr, int portNum, int sessionID, void* pObject, int errorCode, const wchar_t* errorMsg);

};