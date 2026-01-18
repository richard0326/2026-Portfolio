#pragma once

/**
 * 클라에서 보낼 데이터를 포장하는 곳. 메시지를 패킹하는 함수
 */

template <typename T>
void WriteToBuffer(char* buf, size_t& offset, const T& v);

int mpMoveStart(char* buf, char dir, short x, short y);

int mpMoveStop(char* buf, char dir, short x, short y);

int mpAttack1(char* buf, char dir, short x, short y);

int mpAttack2(char* buf, char dir, short x, short y);

int mpAttack3(char* buf, char dir, short x, short y);

int mpEcho(char* buf, char dir, short x, short y);