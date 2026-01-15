#pragma once

#include <Windows.h>

template <typename T>
class LockFreeQueue
{
	struct Node
	{
		T val;
		Node* pNext = nullptr;
	};

	__declspec(align(16)) struct Top
	{
		Node* node;
		long long nodeID;
	};

public:
	LockFreeQueue(int size)
	{
		m_queueCount = 0;

		m_pHead = new Top();
		m_pTail = new Top();
		
		// dummy Node ¸¸µé±â
		m_pHead->node->pNext = nullptr;
		m_pHead->nodeID = 1;
		m_pTail->nodeID = 1;
	}

	~LockFreeQueue()
	{
		delete m_pHead;
		delete m_pTail;
	}

	bool Enqueue(T val)
	{
		Node* inputNode = new Node();
		if (inputNode == nullptr)
			return false;

		inputNode->val = val;
		inputNode->pNext = nullptr;
		Top tTop;
		while (true)
		{
			tTop.nodeID = m_pTail->nodeID;
			tTop.node = m_pTail->node;
			Node* pPrevNext = tTop.node->pNext;
			if (tTop.nodeID != m_pTail->nodeID)
				continue;

			if (pPrevNext == nullptr)
			{
				if (0 == InterlockedCompareExchange64((long long*)&tTop.node->pNext, (long long)inputNode, 0))
				{
					InterlockedCompareExchange128((long long*)m_pTail,
						tTop.nodeID + 1, (long long)inputNode,
						(long long*)&tTop);
					InterlockedIncrement(&m_queueCount);
					break;
				}
			}
			else
			{
				InterlockedCompareExchange128((long long*)m_pTail,
					tTop.nodeID + 1, (long long)pPrevNext,
					(long long*)&tTop);
			}
			YieldProcessor();
		}
		return true;
	}

	bool Dequeue(T* outVal)
	{
		if (InterlockedDecrement(&m_queueCount) < 0)
		{
			InterlockedIncrement(&m_queueCount);
			return false;
		}
		Top hTop;
		Top tTop;
		Node* pHeadNext;
		Node* pTailNext;
		while (true)
		{
			hTop.nodeID = m_pHead->nodeID;
			hTop.node = m_pHead->node;
			pHeadNext = hTop.node->pNext;
			tTop.nodeID = m_pTail->nodeID;
			tTop.node = m_pTail->node;
			pTailNext = tTop.node->pNext;
			if (tTop.nodeID != m_pTail->nodeID)
				continue;

			if (pTailNext != nullptr)
			{
				InterlockedCompareExchange128((long long*)m_pTail, tTop.nodeID + 1, (long long)pTailNext, (long long*)&tTop);
			}
			else
			{
				if (hTop.nodeID != m_pHead->nodeID)
					continue;

				if (pHeadNext != nullptr)
				{
					*outVal = pHeadNext->val;
					if (1 == InterlockedCompareExchange128((long long*)m_pHead, hTop.nodeID + 1, (long long)pHeadNext, (long long*)&hTop))
					{
						delete hTop.node;
						break;
					}
				}
			}
			YieldProcessor();
		}
		return true;
	}

	int GetSize()
	{
		return m_queueCount;
	}

private:
	long m_queueCount = 0;
	Top* m_pHead = nullptr;
	Top* m_pTail = nullptr;
};