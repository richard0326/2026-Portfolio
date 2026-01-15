#pragma once
#include <Windows.h>

struct OVERLAPPEDEX : public OVERLAPPED
{
	bool recvFlags;
};

template <typename T>
class LockFreeQueue;
class RingBuffer;
struct Session
{
public:
	// 읽기 전용 데이터
	LockFreeQueue<byte*>* SendQ; // 송신 큐.
	RingBuffer* RecvQ; // 수신 큐.

	int SessionID = 0; // 접속자의 고유 세션 ID.
	SOCKET Socket = 0; // 현 접속의 TCP 소켓.

	// IP Port 값
	wchar_t IpStr[16];
	unsigned short usPort;
	// Shutdown 여부 확인
	bool IsShutdown;
	bool IOSend; // 오버랩 객체를 1개만 사용하기 때문에 모아서 보내기 위한 flag

	OVERLAPPEDEX RecvOverlappedEx;	// 수신 오버랩드 구조체
	OVERLAPPEDEX SendOverlappedEx;	// 송신 오버랩드 구조체

	// 2개 변수는 항상 붙어있어야함. Release할때 동시에 Interlocked 들어가기 때문
	short ReleaseFlag = false;
	short IOCount = 0;

	void* ObjectPtr;
};