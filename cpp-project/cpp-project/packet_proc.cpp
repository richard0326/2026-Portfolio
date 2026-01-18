#include "stdafx.h"
#include "player.h"
#include "packet_proc.h"

template <typename T>
inline void ReadFromBuffer(const char* buf, size_t& offset, T& out)
{
	std::memcpy(&out, buf + offset, sizeof(T));
	offset += sizeof(T);
}

void netPacketProc_MoveStart(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}
void netPacketProc_MoveStop(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}
void netPacketProc_Attack1(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}
void netPacketProc_Attack2(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}
void netPacketProc_Attack3(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}

void netPacketProc_Echo(Player* player, byte* pByte)
{
	size_t offset = 0;
	char dir;
	short x;
	short y;
	ReadFromBuffer((char*)pByte, offset, dir);
	ReadFromBuffer((char*)pByte, offset, x);
	ReadFromBuffer((char*)pByte, offset, y);
}