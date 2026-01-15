#include "stdafx.h"
#include "lock_free_queue.h"
#include "lock_free_stack.h"
#include "ring_buffer.h"
#include "session.h"
#include "net_server.h"

NetServer::NetServer() {

}

NetServer::~NetServer() {

}

bool NetServer::Init()
{
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    cout << "Init NetServer" << endl;
    return true;
}

void NetServer::Release()
{
    if (m_hIOCP)
    {
        CloseHandle(m_hIOCP);
    }
    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
    }
    WSACleanup();

    cout << "Release NetServer" << endl;
}

bool NetServer::Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt, int maxSession)
{
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        WSACleanup();
        return false;
    }

    // reuse addr
    BOOL reuse = TRUE;
    setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));

    wcscpy_s(m_ipWstr, ipWstr);
    m_listenInfo.sin_family = AF_INET;
    InetPton(AF_INET, ipWstr, &m_listenInfo.sin_addr);
    m_listenInfo.sin_port = htons((u_short)portNum);

    if (bind(m_listenSocket, (sockaddr*)&m_listenInfo, sizeof(m_listenInfo)) == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << "\n";
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed\n";
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!m_hIOCP) {
        std::cerr << "CreateIoCompletionPort failed\n";
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    m_maxSession = maxSession;
    m_sessionVec.clear();
    m_indexStack = new LockFreeStack<int>();
    for (int i = 0; i < m_maxSession; i++) {
        Session* pSession = new Session();
        pSession->RecvQ = new RingBuffer();
        pSession->SendQ = new LockFreeQueue<byte*>();
        m_sessionVec.push_back(pSession);
        m_indexStack->Push(m_maxSession - i - 1); // 스택이니까 거꾸로 넣기
        m_sessionVec[i]->SessionID = i;
    }

    m_workerCreateCnt = workerCreateCnt;

    m_hIOThread = new HANDLE[m_workerCreateCnt];
    for (int i = 0; i < m_workerCreateCnt; i++)
    {
        m_hIOThread[i] = (HANDLE)_beginthreadex(nullptr, 0, NetServer::netIOThread, this, 0, nullptr);
    }

    m_hAcceptThread = (HANDLE)_beginthreadex(nullptr, 0, NetServer::netAcceptThread, this, 0, nullptr);
    m_hMonitoringThread = (HANDLE)_beginthreadex(nullptr, 0, NetServer::monitoringThread, this, 0, nullptr);

    cout << "IOCP Echo Server listening on port " << ipWstr << ":" << portNum << endl;
    cout << "Workers: " << m_workerCreateCnt << endl;

    cout << "Start NetServer" << endl;
}

void NetServer::Stop()
{
    TerminateThread(m_hMonitoringThread, 0);
    CloseHandle(m_hMonitoringThread);

    if (m_hAcceptThread != nullptr)
    {
        DWORD waitAccept = WaitForSingleObject(m_hAcceptThread, 10000);
        if (waitAccept == WAIT_TIMEOUT)
        {
            TerminateThread(m_hAcceptThread, 0);
            cout << "Server Stop. Terminate Accept Thread" << endl;
        }
        
        CloseHandle(m_hAcceptThread);
        m_hAcceptThread = nullptr;
    }

    if (m_hIOThread != nullptr)
    {
        for (int i = 0; i < m_workerCreateCnt; i++)
        {
            PostQueuedCompletionStatus(m_hIOCP, 0, 0, 0);
        }

        DWORD waitCount = WaitForMultipleObjects(m_workerCreateCnt, m_hIOThread, true, 60000);
        if (waitCount == WAIT_TIMEOUT)
        {
            for (int i = 0; i < m_workerCreateCnt; i++)
            {
                TerminateThread(m_hIOThread[i], 0);
                cout << "Server Stop. Terminate Worker Thread " << i + 1 << endl;
            }
        }

        for (int i = 0; i < m_workerCreateCnt; i++)
        {
            CloseHandle(m_hIOThread[i]);
        }
        if (m_workerCreateCnt == 1)
        {
            delete m_hIOThread;
        }
        else
        {
            delete[] m_hIOThread;
        }
        m_hIOThread = nullptr;
    }

    if (m_hIOCP)
    {
        CloseHandle(m_hIOCP);
        m_hIOCP = nullptr;
    }
    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    if (m_sessionVec.empty() == false)
    {
        for (int i = 0; i < m_sessionVec.size(); i++)
        {
            delete m_sessionVec[i]->SendQ;
            delete m_sessionVec[i]->RecvQ;
            delete m_sessionVec[i];
        }
    }

    if (m_indexStack != nullptr)
    {
        delete m_indexStack;
    }

    cout << "Stop NetServer" << endl;
}

unsigned __stdcall NetServer::monitoringThread(void* pParameter)
{
    return 0;
}

unsigned __stdcall NetServer::netAcceptThread(void* pParameter)
{
    NetServer* pThis = (NetServer*)pParameter;

    while (true)
    {
        sockaddr_in clientaddr{};
        int clen = sizeof(clientaddr);
        SOCKET clientSock = accept(pThis->m_listenSocket, (sockaddr*)&clientaddr, &clen);
        if (clientSock == INVALID_SOCKET) {
            cerr << "accept failed: " << WSAGetLastError() << endl;
            break;
        }

        if (pThis->m_indexStack->isEmpty() == true) {
            cout << "user is full" << endl;
            shutdown(clientSock, SD_BOTH);
            closesocket(clientSock);
            continue;
        }

        int sessionId = 0;
        if (pThis->m_indexStack->Pop(&sessionId) == false)
        {
            cerr << "index Pop Fail" << endl;
            break;
        }

        Session* pSession = pThis->m_sessionVec[sessionId];

        InetNtop(AF_INET, &clientaddr.sin_addr, pSession->IpStr, 16);
        pSession->usPort = ntohs(clientaddr.sin_port);
        pSession->Socket = clientSock;

        // 소켓을 IOCP에 연결 (completion key에 conn 포인터)
        HANDLE h = CreateIoCompletionPort((HANDLE)clientSock, pThis->m_hIOCP, (ULONG_PTR)pSession, 0);
        if (!h) {
            std::cerr << "CreateIoCompletionPort(associate) failed" << endl;
            pThis->DisconnectSession(pSession);
            continue;
        }

        ErrorCode error = pThis->netWSARecvPost(pSession);
        if (error != RET_SUCCESS)
        {
            std::cerr << "netWSARecvPost error " << error << endl;
            pThis->DisconnectSession(pSession);
            continue;
        }
    }
    return 0;
}

unsigned __stdcall NetServer::netIOThread(void* pParameter)
{
    NetServer* pThis = (NetServer*)pParameter;

    while (true) {
        DWORD transferred = 0;
        Session* pSession = nullptr;
        OVERLAPPEDEX* pOverlapped = nullptr;
        GetQueuedCompletionStatus(pThis->m_hIOCP, &transferred, (PULONG_PTR)&pSession, (LPOVERLAPPED*)&pOverlapped, INFINITE);

        if (pOverlapped == nullptr)
        {
            // 서버 종료
            cout << "Overlapped IO nullptr : Exit Server " << endl;
            return 0;
        }

        if (transferred == 0)
        {
            pThis->DisconnectSession(pSession);
        }
        else
        {
            if (pOverlapped->recvFlags == true)
            {
                if (pSession->RecvQ->MoveRear(transferred) == false)
                {
                    cout << "RecvQ MoveRear Error " << endl;
                    //CCrashDump::Crash();
                }
                else
                {
                    int retRecv = pThis->netWSARecvPacket(pSession);
                    if (retRecv != RET_SUCCESS)
                    {
                        pThis->DisconnectSession(pSession);
                    }
                }
            }
            // Send Flag
            else
            {
                InterlockedExchange8((char*)&pSession->IOSend, 0);

                int retSendPost = pThis->netWSASendPost(pSession);
                if (retSendPost != RET_SUCCESS)
                {
                    pThis->DisconnectSession(pSession);
                }
            }
        }

        pThis->DecreaseIOCount(pSession);
    }
    return 0;
}

ErrorCode NetServer::netWSARecvPacket(Session* pSession)
{
    while (1)
    {
        unsigned int payloadSize = 0;
        int headerSize = sizeof(payloadSize);
        int UsedSize = pSession->RecvQ->GetUseSize();
        if (UsedSize < headerSize)
        {
            break;
        }

        if (headerSize == 0)
        {
            return RET_SOCKET_ERROR;
        }

        int peekSize = pSession->RecvQ->Peek((char*)&payloadSize, headerSize);
        if (peekSize != headerSize)
        {
            return RET_SOCKET_ERROR;
        }

        if (UsedSize < headerSize + payloadSize)
        {
            break;
        }

        if (pSession->RecvQ->MoveFront(headerSize) == false)
        {
            return RET_SOCKET_ERROR;
        }

        std::shared_ptr<byte[]> byteArr(new byte[headerSize + payloadSize]);
        memcpy(byteArr.get(), &payloadSize, headerSize);
        int deqSize = pSession->RecvQ->Dequeue(((char*)byteArr.get()) + headerSize, payloadSize);
        if (deqSize != payloadSize)
        {
            return RET_SOCKET_ERROR;
        }
        
        ErrorCode retOnRecv = (ErrorCode)OnRecv(pSession->SessionID, pSession->ObjectPtr, byteArr);
        if (retOnRecv != RET_SUCCESS)
        {
            return retOnRecv;
        }
    }

    ErrorCode retRecvPost = netWSARecvPost(pSession);
    if (retRecvPost != RET_SUCCESS)
    {
        return retRecvPost;
    }

    return RET_SUCCESS;
}


ErrorCode NetServer::netWSARecvPost(Session* pSession)
{
    InterlockedIncrement16(&pSession->IOCount);
    int retRecv = 0;
    DWORD recvSize = 0;
    memset(&pSession->RecvOverlappedEx, 0, sizeof(OVERLAPPED));
    pSession->RecvOverlappedEx.recvFlags = true;
    DWORD flag = 0;

    // 비동기로 Recv
    int afterEnqSize = 0;
    int enqSize = pSession->RecvQ->DirectEnqueueSize(&afterEnqSize);
    if (afterEnqSize == 0) // 링버퍼를 1번에 받는 상황
    {
        WSABUF wsaRecvBuf;
        wsaRecvBuf.buf = pSession->RecvQ->GetRearBufferPtr();
        wsaRecvBuf.len = enqSize;

        retRecv = WSARecv(pSession->Socket, &wsaRecvBuf, 1, &recvSize, &flag, &pSession->RecvOverlappedEx, nullptr);
    }
    else // 링버퍼를 2번에 끊어서 받는 상황
    {
        WSABUF wsaRecvBuf[2];
        wsaRecvBuf[0].buf = pSession->RecvQ->GetRearBufferPtr();
        wsaRecvBuf[0].len = enqSize;
        wsaRecvBuf[1].buf = pSession->RecvQ->GetBufferPtr();
        wsaRecvBuf[1].len = afterEnqSize;

        retRecv = WSARecv(pSession->Socket, wsaRecvBuf, 2, &recvSize, &flag, &pSession->RecvOverlappedEx, nullptr);
    }

    // 동기로 Recv가 처리됨. 하지만 IOCP completion도 들어오기 때문에 거기서 처리하는 것
    if (retRecv == 0)
    {
        // 여기서 recv 완료 처리하면 버그 가능성 있음. IOCP에서 처리
    }
    
    // 에러가 발생한 경우
    if (retRecv == SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        if (WSAError != WSA_IO_PENDING) // Pending은 에러 상황이 아니다.
        {
            cout << L"netWSARecvPost() WSARecv Error " << WSAError << endl;
            DecreaseIOCount(pSession);
            return RET_SOCKET_ERROR;
        }
    }

    return RET_SUCCESS;
}

ErrorCode NetServer::netWSASendPacket(int sessionID, byte* byteArr)
{
    Session* pSession = AcquireLock(sessionID);
    if (pSession == nullptr)
    {
        return RET_SOCKET_ERROR;
    }

    if (false == pSession->SendQ->Enqueue(byteArr))
    {
        ReleaseLock(pSession);
        return RET_SOCKET_ERROR;
    }

    ErrorCode retSendPost = netWSASendPost(pSession);
    if (retSendPost != RET_SUCCESS)
    {
        ReleaseLock(pSession);
        return retSendPost;
    }

    ReleaseLock(pSession);
    return RET_SUCCESS;
}

ErrorCode NetServer::netWSASendPost(Session* pSession)
{
    if (pSession->SendQ->GetSize() == 0)
    {
        return RET_SUCCESS;
    }

    char retIOSend = InterlockedExchange8((char*)&pSession->IOSend, 1);
    if (retIOSend == 0)
    {
        WSABUF wsaSendBuf[WsaSendBufferSize];
        memset(&pSession->SendOverlappedEx, 0, sizeof(OVERLAPPED));
        pSession->SendOverlappedEx.recvFlags = false;

        // 여러가지 Send를 모아서 보내는 역할
        int sendPacketCnt = 0;
        LockFreeQueue<byte*>* tmpSendQ = pSession->SendQ;
        for (int packetLoop = 0; packetLoop < WsaSendBufferSize; packetLoop++)
        {
            byte* byteArr = nullptr;
            if (tmpSendQ->Dequeue(&byteArr) == false)
                break;

            wsaSendBuf[packetLoop].buf = (char*)byteArr;
            wsaSendBuf[packetLoop].len = GetPacketLen(byteArr);
            sendPacketCnt++;
        }

        // Send를 시도했지만, 보낼 패킷이 없는 경우
        if (sendPacketCnt == 0)
        {
            InterlockedExchange8((char*)&pSession->IOSend, 0);
            return RET_SUCCESS;
        }

        InterlockedIncrement16(&pSession->IOCount);

        DWORD sendBytes = 0;
        int	sendErr = WSASend(pSession->Socket, wsaSendBuf, sendPacketCnt, &sendBytes, 0, (LPWSAOVERLAPPED)&pSession->SendOverlappedEx, nullptr);
        if (sendErr == 0)
        {
            // 동기로 Send한 경우
            // IOCP 완료 통지도 따로 간다.
        }
        else
        {
            int WSAError = WSAGetLastError();
            if (WSAError != WSA_IO_PENDING)
            {
                cout << "WSASend() Error " << WSAError << endl;
                DecreaseIOCount(pSession);
                return RET_SOCKET_ERROR;
            }
        }
    }

    return RET_SUCCESS;
}

void NetServer::DisconnectSession(int sessionID)
{
    Session* pSession = AcquireLock(sessionID);
    if (pSession == nullptr)
        return;

    DisconnectSession(pSession, true);
}

void NetServer::DisconnectSession(Session* pSession, bool isLock)
{
    if (isLock == false)
    {
        if (1 == InterlockedIncrement16(&pSession->IOCount))
        {
            DecreaseIOCount(pSession);
            return ;
        }
    }

    pSession->IsShutdown = true;
    if (CancelIoEx((HANDLE)pSession->Socket, nullptr) == 0)
    {
        int WSAError = WSAGetLastError();
        if (WSAError != ERROR_NOT_FOUND)
        {
            cout << "Disconnect1() Error " << WSAError << endl;
        }
    }
    shutdown(pSession->Socket, SD_BOTH);

    ReleaseLock(pSession);
}

Session* NetServer::AcquireLock(int sessionID)
{
    Session* pSession = m_sessionVec[sessionID];
    if (1 == InterlockedIncrement16(&pSession->IOCount))
    {
        DecreaseIOCount(pSession);
        return nullptr;
    }
    return pSession;
}

void NetServer::ReleaseLock(Session* pSession)
{
    DecreaseIOCount(pSession);
}

void NetServer::DecreaseIOCount(Session* pSession)
{
    if (0 == InterlockedDecrement16(&pSession->IOCount))
    {
        if (InterlockedCompareExchange((LONG*)&pSession->ReleaseFlag, true, false) == false)
        {
        }
    }
}

unsigned int NetServer::GetPacketLen(byte* byteArr)
{
    // 패킷의 제일 앞단 unsigned int 4byte를 길이로 고정
    return ((unsigned int*)byteArr)[0];
}

void NetServer::InitSession(Session* pSession)
{
    pSession->IOCount = 0;
    pSession->IOSend = 0;
    ZeroMemory(pSession->IpStr, 16);
    pSession->ReleaseFlag = false;
    pSession->ObjectPtr = nullptr;
    pSession->IsShutdown = false;
    pSession->usPort = 0;
    pSession->Socket = 0;
}

bool NetServer::SendPacket(int sessionID, byte* byteArr)
{
    if (netWSASendPacket(sessionID, byteArr) != RET_SUCCESS)
    {
        return false;
    }
    return true;
}

bool NetServer::SetObject(int sessionID, void* pObject)
{
    Session* pSession = m_sessionVec[sessionID];
    if (pSession == nullptr || pSession->ReleaseFlag) {
        return false;
    }
    pSession->ObjectPtr = pObject;
    return true;
}

void* NetServer::GetObject(int sessionID)
{
    Session* pSession = m_sessionVec[sessionID];
    if (pSession == nullptr || pSession->ReleaseFlag) {
        return nullptr;
    }

    return pSession->ObjectPtr;
}