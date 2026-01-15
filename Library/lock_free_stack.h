#pragma once

#include <atomic>

template <typename T>
class LockFreeStack
{
	struct Node
	{
		T data;
		Node* pNext;
	};

	__declspec(align(16)) struct Top
	{
		Node* node;
		long long nodeID;
	};

public:
	LockFreeStack()
	{
		m_topNode = new Top();
		m_topNode->nodeID = 1;
		m_topNode->node = nullptr;
	}

	~LockFreeStack()
	{
		delete m_topNode;
	}

	bool Push(T data)
	{
		Node* inputNode = new Node();
		if (inputNode == nullptr)
			return false;

		inputNode->data = data;
		inputNode->pNext = nullptr;
		Top top;
		while (true)
		{
			top.node = m_topNode->node;
			top.nodeID = m_topNode->nodeID;
			inputNode->pNext = top.node;
			if (top.nodeID == m_topNode->nodeID)
			{
				if (1 == InterlockedCompareExchange128((long long*)m_topNode, top.nodeID + 1, (long long)inputNode, (long long*)&top))
				{
					InterlockedIncrement(&m_stackCount);
					break;
				}
			}
			YieldProcessor();
		}

		return true;
	}

	bool Pop(T* outData)
	{
		if (InterlockedDecrement(&m_stackCount) < 0)
		{
			InterlockedIncrement(&m_stackCount);
			return false;
		}

		Top top;
		while (true)
		{
			top.node = m_topNode->node;
			top.nodeID = m_topNode->nodeID;
			if (top.node == nullptr) {
				YieldProcessor(); continue;
			}
			Node* pNext = top.node->pNext;
			*outData = top.node->data;
			if (top.nodeID == m_topNode->nodeID)
			{
				if (1 == InterlockedCompareExchange128((long long*)m_topNode, top.nodeID + 1, (long long)pNext, (long long*)&top))
				{
					delete  top.node;
					break;
				}
			}
			YieldProcessor();
		}
		return true;
	}

	bool isEmpty()
	{
		return m_topNode->node == nullptr;
	}

	int GetSize()
	{
		return m_stackCount;
	}

private:
	long m_stackCount = 0;
	Top* m_topNode;
};
