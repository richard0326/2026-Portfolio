#include "stdafx.h"
#include "net_server.h"
#include "echo_server.h"

int main() {
    EchoServer echoServer;
    if (echoServer.Init() == false)
    {
        return false;
    }

    int maxSession = 5000;
    const wchar_t* ipStr = L"127.0.0.1";
    int port = 9000;
    int workerThread = 4;
    if (echoServer.Start(ipStr, port, workerThread, maxSession) == false)
    {
        echoServer.Release();
        return false;
    }
    
    bool g_running = true;
    while (g_running)
    {
        char ch;
        std::cin >> ch;

        if (std::tolower(ch) == 'q')
        {
            std::cout << "[INFO] Shutdown signal received.\n";
            g_running = false;
            break;
        }
    }

    echoServer.Stop();

    echoServer.Release();
    return 0;
}
