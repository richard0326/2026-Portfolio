#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char** argv) {
    const char* ip = "127.0.0.1";
    int port = 9000;

    if (argc >= 2) ip = argv[1];
    if (argc >= 3) port = std::atoi(argv[2]);

    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons((u_short)port);

    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to " << ip << ":" << port << "\n";
    std::cout << "Type message and press enter. (quit to exit)\n";

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (!std::cin.good()) break;
        if (line == "quit") break;

        // send
        char buf[2048];
        unsigned int len = line.size();
        memcpy(buf, &len, sizeof(len));
        memcpy(buf + sizeof(len), line.data(), len);
        int sent = send(s, buf, (int)sizeof(len) + len, 0);
        if (sent <= 0) {
            std::cerr << "send failed\n";
            break;
        }

        ZeroMemory(buf, sizeof(buf));
        // recv length (4 bytes)
        unsigned int recvLen = 0;
        int recvd = recv(s, reinterpret_cast<char*>(&recvLen), sizeof(recvLen), 0);
        if (recvd != sizeof(recvLen)) {
            std::cerr << "recv length failed\n";
            break;
        }

        // (선택) 길이 방어
        if (recvLen >= sizeof(buf)) {
            std::cerr << "payload too large\n";
            break;
        }

        // recv payload
        recvd = recv(s, buf, (int)recvLen, 0);
        if (recvd != (int)recvLen) {
            std::cerr << "recv payload failed\n";
            break;
        }

        buf[recvLen] = '\0';
        std::cout << "echo: " << buf << "\n";
    }

    shutdown(s, SD_BOTH);
    closesocket(s);
    WSACleanup();
    return 0;
}
