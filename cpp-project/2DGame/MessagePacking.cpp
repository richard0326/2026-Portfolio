#include "stdafx.h"
#include "MessagePacking.h"

/**
 * 클라에서 보낼 데이터를 포장하는 곳. 메시지를 패킹하는 함수
 */
template <typename T>
void WriteToBuffer(char* buf, size_t& offset, const T& v)
{
	std::memcpy(buf + offset, &v, sizeof(T));
	offset += sizeof(T);
}

int mpMoveStart(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_MOVE_START;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}

int mpMoveStop(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_MOVE_STOP;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}

int mpAttack1(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_ATTACK1;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}

int mpAttack2(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_ATTACK2;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}

int mpAttack3(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_ATTACK3;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}

int mpEcho(char* buf, char dir, short x, short y)
{
	size_t offset = 0;

	int type = dfPACKET_CS_ECHO;
	unsigned int len =
		sizeof(type) +
		sizeof(dir) +
		sizeof(x) +
		sizeof(y);

	WriteToBuffer(buf, offset, len);
	WriteToBuffer(buf, offset, type);
	WriteToBuffer(buf, offset, dir);
	WriteToBuffer(buf, offset, x);
	WriteToBuffer(buf, offset, y);
	return len + sizeof(len);
}