#include "stdafx.h"
#include "net_server.h"
#include "echo_server.h"

int main() {
    EchoServer echoServer;
    if (echoServer.Init() == false)
    {
        return false;
    }
    if (echoServer.Start(L"127.0.0.1", 9000, 4) == false)
    {
        echoServer.Release();
        return false;
    }
    
    echoServer.Stop();

    echoServer.Release();
    return 0;
}
