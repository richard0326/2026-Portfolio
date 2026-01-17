#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <ctime>
using namespace std;

#include <vector>
#include <thread>
#include <atomic>
#include <memory>

#include "define.h"
#include "packet.h"
#include "lock_free_queue.h"
#include "lock_free_stack.h"
#include "ring_buffer.h"
#include "session.h"