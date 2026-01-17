#pragma once

template <typename T>
void WriteToBuffer(byte* buf, size_t& offset, const T& v);
byte* mpCreateMyCharacter(char dir, short x, short y);
byte* mpCreateOtherCharacter(int sessionID, char dir, short x, short y, char hp);
byte* mpDeleteCharacter(int sessionID);
byte* mpMoveStart(int sessionID, char dir, short x, short y);
byte* mpMoveStop(int sessionID, char dir, short x, short y);
byte* mpAttack1(int sessionID, char dir, short x, short y);
byte* mpAttack2(int sessionID, char dir, short x, short y);
byte* mpAttack3(int sessionID, char dir, short x, short y);
byte* mpDamage(int attackID, int damageID, char damageHP);
byte* mpSync(int sessionID, short x, short y);
byte* mpEcho(int tickMilliSec);