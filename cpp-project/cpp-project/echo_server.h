#pragma once

class EchoServer : public NetServer
{
public:
	EchoServer();
	~EchoServer();

	// 서버 시작할때
	bool Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt);

	// 서버 멈출 때
	void Stop();

	// accept 직후
	virtual bool OnConnectionRequest(const wchar_t* ipWstr, int portNum);

	// Accept 후 접속처리 완료 후 호출.
	virtual void OnClientJoin(unsigned long long SessionID);
	// Release 후 호출
	virtual void OnClientLeave(unsigned long long SessionID);

	// 패킷/메시지 수신 완료 후
	virtual int OnRecv(unsigned long long SessionID, CSerializeBuffer* pPacket);
	// 에러일 때 호출되는 함수
	virtual void OnError(const wchar_t* ipWstr, int portNum, int errorCode, const wchar_t* errorMsg);

};