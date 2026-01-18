#pragma once

class Player;
void netPacketProc_MoveStart(Player* player, byte* pByte);
void netPacketProc_MoveStop(Player* player, byte* pByte);
void netPacketProc_Attack1(Player* player, byte* pByte);
void netPacketProc_Attack2(Player* player, byte* pByte);
void netPacketProc_Attack3(Player* player, byte* pByte);
void netPacketProc_Echo(Player* player, byte* pByte);
