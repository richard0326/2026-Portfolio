#include "stdafx.h"
#include "net_server.h"
#include "mmorpg_server.h"

int main() {
    MmorpgServer mmorpgoServer;
    if (mmorpgoServer.Init() == false)
    {
        return false;
    }

    int maxSession = 5000;
    const wchar_t* ipStr = L"127.0.0.1";
    int port = 9000;
    int workerThread = 4;
    if (mmorpgoServer.Start(ipStr, port, workerThread, maxSession) == false)
    {
        mmorpgoServer.Release();
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

    mmorpgoServer.Stop();

    mmorpgoServer.Release();
    return 0;
}
