#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>

#include <windows.h>
#include <iostream>
using namespace std;

#include <vector>
#include <thread>
#include <atomic>
#include <memory>

#include "packet.h"