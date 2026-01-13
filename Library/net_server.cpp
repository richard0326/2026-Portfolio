#include "stdafx.h"
#include "net_server.h"
#include <thread>

static const int kBufSize = 1024;

enum class OpType { Recv, Send };

struct IoContext {
    OVERLAPPED ov{};
    WSABUF wsabuf{};
    char buf[kBufSize];
    OpType op = OpType::Recv;

    // send partial handling
    size_t sentOffset = 0;
    size_t totalToSend = 0;

    IoContext() {
        ZeroMemory(&ov, sizeof(ov));
        wsabuf.buf = buf;
        wsabuf.len = kBufSize;
        ZeroMemory(buf, sizeof(buf));
    }

    void resetOverlapped() {
        ZeroMemory(&ov, sizeof(ov));
    }
};

struct ConnContext {
    SOCKET s = INVALID_SOCKET;
    IoContext recvCtx;
    IoContext sendCtx;
};

NetServer::NetServer() {

}

NetServer::~NetServer() {

}

static bool PostRecv(ConnContext* c) {
    DWORD flags = 0;
    DWORD bytes = 0;

    c->recvCtx.op = OpType::Recv;
    c->recvCtx.resetOverlapped();
    c->recvCtx.wsabuf.buf = c->recvCtx.buf;
    c->recvCtx.wsabuf.len = kBufSize;

    int rc = WSARecv(c->s, &c->recvCtx.wsabuf, 1, &bytes, &flags, &c->recvCtx.ov, nullptr);
    if (rc == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            std::cerr << "[WSARecv] error=" << err << "\n";
            return false;
        }
    }
    return true;
}

static bool PostSend(ConnContext* c, const char* data, size_t len) {
    if (len == 0) return true;

    c->sendCtx.op = OpType::Send;
    c->sendCtx.resetOverlapped();

    // copy to send buffer
    if (len > kBufSize) len = kBufSize;
    memcpy(c->sendCtx.buf, data, len);

    c->sendCtx.sentOffset = 0;
    c->sendCtx.totalToSend = len;

    c->sendCtx.wsabuf.buf = c->sendCtx.buf;
    c->sendCtx.wsabuf.len = static_cast<ULONG>(len);

    DWORD bytes = 0;
    int rc = WSASend(c->s, &c->sendCtx.wsabuf, 1, &bytes, 0, &c->sendCtx.ov, nullptr);
    if (rc == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            std::cerr << "[WSASend] error=" << err << "\n";
            return false;
        }
    }
    return true;
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

bool NetServer::Start(const wchar_t* ipWstr, int portNum, int workerCreateCnt)
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

    SYSTEM_INFO si{};
    GetSystemInfo(&si);
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
        sockaddr_in caddr{};
        int clen = sizeof(caddr);
        SOCKET clientSock = accept(pThis->m_listenSocket, (sockaddr*)&caddr, &clen);
        if (clientSock == INVALID_SOCKET) {
            cerr << "accept failed: " << WSAGetLastError() << endl;
            break;
        }

        // 새 연결 컨텍스트 생성
        auto* conn = new ConnContext();
        conn->s = clientSock;

        // 소켓을 IOCP에 연결 (completion key에 conn 포인터)
        HANDLE h = CreateIoCompletionPort((HANDLE)clientSock, pThis->m_hIOCP, (ULONG_PTR)conn, 0);
        if (!h) {
            std::cerr << "CreateIoCompletionPort(associate) failed\n";
            if (conn->s != INVALID_SOCKET) {
                shutdown(conn->s, SD_BOTH);
                closesocket(conn->s);
                conn->s = INVALID_SOCKET;
            }

            delete conn;
            continue;
        }

        // 첫 Recv 걸기
        if (!PostRecv(conn)) {
            if (conn->s != INVALID_SOCKET) {
                shutdown(conn->s, SD_BOTH);
                closesocket(conn->s);
                conn->s = INVALID_SOCKET;
            }
            delete conn;
            continue;
        }
    }
    return 0;
}

unsigned __stdcall NetServer::netIOThread(void* pParameter)
{
    NetServer* pThis = (NetServer*)pParameter;

    while (true) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED pov = nullptr;

        BOOL ok = GetQueuedCompletionStatus(pThis->m_hIOCP, &bytesTransferred, &completionKey, &pov, INFINITE);

        auto* conn = reinterpret_cast<ConnContext*>(completionKey);
        //if (!g_running.load()) break;

        if (pov == nullptr) {
            // 보통 종료 신호 등
            continue;
        }

        IoContext* io = CONTAINING_RECORD(pov, IoContext, ov);

        if (!ok) {
            DWORD err = GetLastError();
            // 연결이 끊긴 케이스도 많음
            // std::cerr << "[GQCS] error=" << err << "\n";
            if (conn->s != INVALID_SOCKET) {
                shutdown(conn->s, SD_BOTH);
                closesocket(conn->s);
                conn->s = INVALID_SOCKET;
            }
            delete conn;
            continue;
        }

        if (bytesTransferred == 0) {
            // graceful close
            if (conn->s != INVALID_SOCKET) {
                shutdown(conn->s, SD_BOTH);
                closesocket(conn->s);
                conn->s = INVALID_SOCKET;
            }
            delete conn;
            continue;
        }

        if (io->op == OpType::Recv) {
            // 받은 데이터 그대로 에코
            if (!PostSend(conn, io->buf, bytesTransferred)) {
                if (conn->s != INVALID_SOCKET) {
                    shutdown(conn->s, SD_BOTH);
                    closesocket(conn->s);
                    conn->s = INVALID_SOCKET;
                }
                delete conn;
                continue;
            }
            // send 완료 후 다시 recv 걸도록 (send 완료 이벤트에서 PostRecv)
        }
        else { // Send 완료
            // partial send 처리 (대부분 한번에 끝나지만 안전하게)
            io->sentOffset += bytesTransferred;
            if (io->sentOffset < io->totalToSend) {
                size_t remain = io->totalToSend - io->sentOffset;
                io->resetOverlapped();
                io->wsabuf.buf = io->buf + io->sentOffset;
                io->wsabuf.len = static_cast<ULONG>(remain);

                DWORD bytes = 0;
                int rc = WSASend(conn->s, &io->wsabuf, 1, &bytes, 0, &io->ov, nullptr);
                if (rc == SOCKET_ERROR) {
                    int err = WSAGetLastError();
                    if (err != WSA_IO_PENDING) {
                        if (conn->s != INVALID_SOCKET) {
                            shutdown(conn->s, SD_BOTH);
                            closesocket(conn->s);
                            conn->s = INVALID_SOCKET;
                        }
                        delete conn;
                    }
                }
            }
            else {
                // send 다 끝났으면 다음 recv
                if (!PostRecv(conn)) {
                    if (conn->s != INVALID_SOCKET) {
                        shutdown(conn->s, SD_BOTH);
                        closesocket(conn->s);
                        conn->s = INVALID_SOCKET;
                    }
                    delete conn;
                    continue;
                }
            }
        }
    }
    return 0;
}