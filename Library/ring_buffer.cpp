#include "stdafx.h"
#include "ring_buffer.h"
#include <iostream>

RingBuffer::RingBuffer(void)
{
	m_maxSize = DEFAULT_SIZE;
	m_realSize = m_maxSize + 1;

	m_ringBuf = new char[m_realSize];

	m_rearPtr = m_frontPtr = m_ringBuf;
	m_ringEndPtr = m_ringBuf + m_realSize;
}

RingBuffer::RingBuffer(int bufferSize)
{
	if (bufferSize == 0)
	{
		printf("RingBuffer Size Zero\n");
		bufferSize = DEFAULT_SIZE;
	}

	m_maxSize = bufferSize;
	m_realSize = m_maxSize + 1;

	m_ringBuf = new char[m_realSize];

	m_rearPtr = m_frontPtr = m_ringBuf;
	m_ringEndPtr = m_ringBuf + m_realSize;
}

RingBuffer::~RingBuffer(void)
{
	delete[] m_ringBuf;
}

int RingBuffer::GetBufferSize(void)
{
	return m_maxSize;
}

int RingBuffer::GetUseSize(void)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front > rear)
	{
		return (m_ringEndPtr - front) + (rear - m_ringBuf);
	}
	return rear - front;
}

int RingBuffer::GetFreeSize(void)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front == rear)
	{
		return m_maxSize;
	}
	else if (front < rear)
	{
		return m_maxSize - (rear - front);
	}
	return front - (rear + 1);
}

int RingBuffer::DirectEnqueueSize(void)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front > rear)
	{
		return front - (rear + 1);
	}
	else if (front == m_ringBuf) {
		return m_ringEndPtr - (rear + 1);
	}
	return m_ringEndPtr - rear;
}

int RingBuffer::DirectEnqueueSize(int* usedSize)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	*usedSize = 0;
	if (front > rear)
	{
		//*usedSize = (m_RingEndPtr - Front) + (Rear - m_RingBuf);
		return front - (rear + 1);
	}
	else if (front == m_ringBuf) {
		return m_ringEndPtr - (rear + 1);
	}
	*usedSize = (front - 1) - m_ringBuf;
	return m_ringEndPtr - rear;
}

int RingBuffer::DirectDequeueSize(void)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front > rear) {
		return m_ringEndPtr - front;
	}
	return rear - front;
}

int RingBuffer::DirectDequeueSize(int* usedSize)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front > rear) {
		*usedSize = (m_ringEndPtr - front) + (rear - m_ringBuf);
		return m_ringEndPtr - front;
	}
	*usedSize = rear - front;
	return rear - front;
}

int RingBuffer::Enqueue(char* data, int size)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if ((rear + 1 == front) || ((rear + 1) - m_realSize == front))
		return 0;

	char* cpyPtr = rear;

	for (int i = 0; i < size; ++i)
	{
		*cpyPtr = data[i];
		cpyPtr = rear + i + 1;
		if (cpyPtr >= m_ringEndPtr) {
			cpyPtr -= m_realSize;
		}

		if ((cpyPtr + 1 == front) || ((cpyPtr + 1) - m_realSize == front))
		{
			size = i + 1;
			break;
		}
	}

	m_rearPtr = cpyPtr;

	return size;
}

int RingBuffer::Dequeue(char* dest, int size)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front == rear)
		return 0;

	int dequeueSize = size;

	char* cpyPtr = front;
	for (int i = 0; i < size; ++i)
	{
		dest[i] = *cpyPtr;
		cpyPtr = front + i + 1;
		if (cpyPtr >= m_ringEndPtr) {
			cpyPtr -= m_realSize;
		}

		if (cpyPtr == rear) {
			dequeueSize = i + 1;
			break;
		}
	}

	m_frontPtr = cpyPtr;

	return dequeueSize;
}

int RingBuffer::Peek(char* dest, int size)
{
	char* rear = m_rearPtr;
	char* front = m_frontPtr;
	if (front == rear)
		return 0;

	int cpySize = size;
	char* cpyPtr = front;
	for (int i = 0; i < size; ++i)
	{
		dest[i] = *cpyPtr;
		cpyPtr = front + i + 1;
		if (cpyPtr >= m_ringEndPtr) {
			cpyPtr -= m_realSize;
		}

		if (cpyPtr == rear) {
			cpySize = i + 1;
			break;
		}
	}

	return cpySize;
}

bool RingBuffer::MoveRear(int size)
{
	if (size == 0)
		return false;

	char* front = m_frontPtr - 1;
	char* rear = m_rearPtr;
	char* cmpPtr = m_rearPtr + size;

	if (rear < front)
	{
		if (front < cmpPtr)
			return false;
	}
	else
	{
		if (cmpPtr >= m_ringEndPtr) {
			cmpPtr -= m_realSize;
			if (cmpPtr > front)
				return false;
		}
	}

	m_rearPtr = cmpPtr;
	return true;
}

bool RingBuffer::MoveFront(int size)
{
	if (size == 0)
		return false;

	char* front = m_frontPtr;
	char* rear = m_rearPtr;
	char* cmpPtr = front + size;

	if (front < rear)
	{
		if (rear < cmpPtr)
			return false;
	}
	else
	{
		if (cmpPtr >= m_ringEndPtr) {
			cmpPtr -= m_realSize;
			if (cmpPtr > rear)
				return false;
		}
	}

	m_frontPtr = cmpPtr;

	return true;
}

void RingBuffer::ClearBuffer(void)
{
	m_rearPtr = m_frontPtr = m_ringBuf;
}

char* RingBuffer::GetFrontBufferPtr(void)
{
	return m_frontPtr;
}

char* RingBuffer::GetRearBufferPtr(void)
{
	return m_rearPtr;
}

char* RingBuffer::GetBufferPtr(void)
{
	return m_ringBuf;
}