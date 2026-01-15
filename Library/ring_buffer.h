#pragma once

class RingBuffer
{
private:
	enum {
		DEFAULT_SIZE = 5000,
	};

public:
	RingBuffer(void);
	RingBuffer(int bufferSize);
	~RingBuffer(void);

	int GetBufferSize(void);
	int GetUseSize(void);
	int GetFreeSize(void);
	int DirectEnqueueSize(void);
	int DirectEnqueueSize(int* usedSize);
	int DirectDequeueSize(void);
	int DirectDequeueSize(int* usedSize);
	int Enqueue(char* data, int size);
	int Dequeue(char* dest, int size);
	int Peek(char* dest, int size);
	bool MoveRear(int size);
	bool MoveFront(int size);
	void ClearBuffer(void);
	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);
	char* GetBufferPtr(void);
private:
	// 큐의 최대 크기
	int		m_maxSize;
	int		m_realSize;
	
	// 링버퍼 배열
	char*	m_ringBuf;
	char*	m_ringEndPtr;

	// 데이터의 시작과 마지막 포인터
	char*	m_frontPtr;
	char*	m_rearPtr;
};