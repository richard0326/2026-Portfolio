#pragma once

/**
 * 서버에서 받은 데이터에 대한 컨텐츠 처리를 진행하는 곳.
 */
bool netPacketProc_CreateMyCharacter(char* recvBuffer);

bool netPacketProc_CreateOtherCharacter(char* recvBuffer);

bool netPacketProc_DeleteCharacter(char* recvBuffer);

bool netPacketProc_MoveStart(char* recvBuffer);

bool netPacketProc_MoveStop(char* recvBuffer);

bool netPacketProc_Attack1(char* recvBuffer);

bool netPacketProc_Attack2(char* recvBuffer);

bool netPacketProc_Attack3(char* recvBuffer);

bool netPacketProc_Damage(char* recvBuffer);

bool netPacketProc_Sync(char* recvBuffer);

bool netPacketProc_Echo(char* recvBuffer);