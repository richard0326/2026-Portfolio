#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
using namespace std;
