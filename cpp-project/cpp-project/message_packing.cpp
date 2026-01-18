#include "stdafx.h"
#include "message_packing.h"

template <typename T>
void WriteToBuffer(byte* buf, size_t& offset, const T& v)
{
	std::memcpy(buf + offset, &v, sizeof(T));
	offset += sizeof(T);
}

byte* mpCreateMyCharacter(int sessionID, char dir, short x, short y, char hp)
{
	int type = dfPACKET_SC_CREATE_MY_CHARACTER;

	unsigned int len =
		sizeof(int) + // type
		sizeof(int) + // ID
		sizeof(char) + // dir
		sizeof(short) + // x
		sizeof(short) +  // y
		sizeof(char); // hp

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);
	WriteToBuffer(buffer, offset, hp);

	return buffer;
}

byte* mpCreateOtherCharacter(int sessionID, char dir, short x, short y, char hp)
{
	int type = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	unsigned int len =
		sizeof(int) +  // type
		sizeof(int) +   // sessionID
		sizeof(char) +  // dir
		sizeof(short) + // x
		sizeof(short) + // y
		sizeof(char);   // hp

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);
	WriteToBuffer(buffer, offset, hp);

	return buffer;
}

byte* mpDeleteCharacter(int sessionID)
{
	int type = dfPACKET_SC_DELETE_CHARACTER;
	unsigned int len =
		sizeof(int) + // type
		sizeof(int);   // sessionID

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);

	return buffer;
}

byte* mpMoveStart(int sessionID, char dir, short x, short y)
{
	int type = dfPACKET_SC_MOVE_START;
	unsigned int len =
		sizeof(int) +
		sizeof(int) +
		sizeof(char) +
		sizeof(short) +
		sizeof(short);

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpMoveStop(int sessionID, char dir, short x, short y)
{
	int type = dfPACKET_SC_MOVE_STOP;
	unsigned int len =
		sizeof(int) +
		sizeof(int) +
		sizeof(char) +
		sizeof(short) +
		sizeof(short);

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpAttack1(int sessionID, char dir, short x, short y)
{
	int type = dfPACKET_SC_ATTACK1;
	unsigned int len =
		sizeof(int) + sizeof(int) + sizeof(char) + sizeof(short) + sizeof(short);

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpAttack2(int sessionID, char dir, short x, short y)
{
	int type = dfPACKET_SC_ATTACK2;
	unsigned int len =
		sizeof(int) + sizeof(int) + sizeof(char) + sizeof(short) + sizeof(short);

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpAttack3(int sessionID, char dir, short x, short y)
{
	int type = dfPACKET_SC_ATTACK3;
	unsigned int len =
		sizeof(int) + sizeof(int) + sizeof(char) + sizeof(short) + sizeof(short);

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, dir);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpDamage(int attackID, int damageID, char damageHP)
{
	int type = dfPACKET_SC_DAMAGE;

	unsigned int len =
		sizeof(int) + // type
		sizeof(int) +  // attackID
		sizeof(int) +  // damageID
		sizeof(char);  // damageHP

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, attackID);
	WriteToBuffer(buffer, offset, damageID);
	WriteToBuffer(buffer, offset, damageHP);

	return buffer;
}

byte* mpSync(int sessionID, short x, short y)
{
	int type = dfPACKET_SC_SYNC;

	unsigned int len =
		sizeof(int) + // type
		sizeof(int) +  // sessionID
		sizeof(short) + // x
		sizeof(short);  // y

	size_t PACKET_SIZE = sizeof(len) + len;
	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, sessionID);
	WriteToBuffer(buffer, offset, x);
	WriteToBuffer(buffer, offset, y);

	return buffer;
}

byte* mpEcho(int tickMilliSec)
{
	int type = dfPACKET_CS_ECHO;
	unsigned int len = 
		sizeof(int) +  // type
		sizeof(int);   // tickMilliSec
	size_t PACKET_SIZE = sizeof(len) + len;

	byte* buffer = new byte[PACKET_SIZE];
	size_t offset = 0;

	// len
	WriteToBuffer(buffer, offset, len);

	WriteToBuffer(buffer, offset, type);
	WriteToBuffer(buffer, offset, tickMilliSec);
	return buffer;
}