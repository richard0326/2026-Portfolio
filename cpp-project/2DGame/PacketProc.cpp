#include "stdafx.h"
#include "PacketProc.h"
#include "GameMgr.h"
#include "ObjectMgr.h"
#include "BaseObject.h"
#include "PlayerObject.h"

/**
 * 서버에서 받은 데이터에 대한 컨텐츠 처리를 진행하는 곳.
 */

template <typename T>
inline void ReadFromBuffer(const char* buf, size_t& offset, T& out)
{
	std::memcpy(&out, buf + offset, sizeof(T));
	offset += sizeof(T);
}

bool netPacketProc_CreateMyCharacter(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	char outHP;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);
	ReadFromBuffer(recvBuffer, offset, outHP);

	CPlayerObject* playerObject = (CPlayerObject*)SINGLETON(CObjectMgr)->CreateObject(eTYPE_PLAYER, outID);
	if (playerObject == nullptr)
		return false;
	
	SINGLETON(CGameMgr)->SetPlayerObject(playerObject);
	playerObject->SetPlayerCharacterTrue();
	playerObject->SetPosition(outX, outY);
	playerObject->SetHP(outHP);
	
	if (outDir == dfPACKET_MOVE_DIR_LL) {
		playerObject->SetDirection(false);
	}
	else {
		playerObject->SetDirection(true);
	}
	playerObject->InputActionProc(dfACTION_STAND);
	
	return true;
}

bool netPacketProc_CreateOtherCharacter(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	char outHP;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);
	ReadFromBuffer(recvBuffer, offset, outHP);

	CPlayerObject* playerObject = (CPlayerObject*)SINGLETON(CObjectMgr)->CreateObject(eTYPE_PLAYER, outID);
	if (playerObject == nullptr)
		return false;

	playerObject->SetPosition(outX, outY);
	playerObject->SetHP(outHP);
	
	if (outDir == dfPACKET_MOVE_DIR_LL) {
		playerObject->SetDirection(false);
	}
	else {
		playerObject->SetDirection(true);
	}
	playerObject->InputActionProc(dfACTION_STAND);

	return true;
}

bool netPacketProc_DeleteCharacter(char* recvBuffer)
{
	int outID;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);

	if (SINGLETON(CObjectMgr)->EraseByObjectID(outID) == false)
		return false;

	return true;
}

bool netPacketProc_MoveStart(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* outPlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&outPlayer) == false)
		return false;

	switch (outDir)
	{
	case dfPACKET_MOVE_DIR_LL:
		outPlayer->InputActionProc(dfACTION_MOVE_LL);
		break;
	case dfPACKET_MOVE_DIR_LU:
		outPlayer->InputActionProc(dfACTION_MOVE_LU);
		break;
	case dfPACKET_MOVE_DIR_UU:
		outPlayer->InputActionProc(dfACTION_MOVE_UU);
		break;
	case dfPACKET_MOVE_DIR_RU:
		outPlayer->InputActionProc(dfACTION_MOVE_RU);
		break;
	case dfPACKET_MOVE_DIR_RR:
		outPlayer->InputActionProc(dfACTION_MOVE_RR);
		break;
	case dfPACKET_MOVE_DIR_RD:
		outPlayer->InputActionProc(dfACTION_MOVE_RD);
		break;
	case dfPACKET_MOVE_DIR_DD:
		outPlayer->InputActionProc(dfACTION_MOVE_DD);
		break;
	case dfPACKET_MOVE_DIR_LD:
		outPlayer->InputActionProc(dfACTION_MOVE_LD);
		break;
	default:
		return false;
	}

	outPlayer->SetPosition(outX, outY);

	return true;
}

bool netPacketProc_MoveStop(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* outPlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&outPlayer) == false)
		return false;

	outPlayer->SetPosition(outX, outY);

	if (outDir == dfPACKET_MOVE_DIR_LL) {
		outPlayer->SetDirection(false);
	}
	else {
		outPlayer->SetDirection(true);
	}
	outPlayer->InputActionProc(dfACTION_STAND);

	return true;
}

bool netPacketProc_Attack1(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* outPlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&outPlayer) == false)
		return false;

	outPlayer->SetPosition(outX, outY);

	if (outDir == dfPACKET_MOVE_DIR_LL) {
		outPlayer->SetDirection(false);
	}
	else {
		outPlayer->SetDirection(true);
	}
	outPlayer->InputActionProc(dfACTION_ATTACK1);
	outPlayer->ResetAnimation();

	return true;
}

bool netPacketProc_Attack2(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* outPlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&outPlayer) == false)
		return false;
	
	outPlayer->SetPosition(outX, outY);

	if (outDir == dfPACKET_MOVE_DIR_LL) {
		outPlayer->SetDirection(false);
	}
	else {
		outPlayer->SetDirection(true);
	}
	outPlayer->InputActionProc(dfACTION_ATTACK2);
	outPlayer->ResetAnimation();

	return true;
}

bool netPacketProc_Attack3(char* recvBuffer)
{
	int outID;
	char outDir;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outDir);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* outPlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&outPlayer) == false)
		return false;

	outPlayer->SetPosition(outX, outY);

	if (outDir == dfPACKET_MOVE_DIR_LL) {
		outPlayer->SetDirection(false);
	}
	else {
		outPlayer->SetDirection(true);
	}
	outPlayer->InputActionProc(dfACTION_ATTACK3);
	outPlayer->ResetAnimation();

	return true;
}

bool netPacketProc_Damage(char* recvBuffer)
{
	int outAttackID;
	int outDamageID;
	char outDamageHP;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outAttackID);
	ReadFromBuffer(recvBuffer, offset, outDamageID);
	ReadFromBuffer(recvBuffer, offset, outDamageHP);

	AddLog(L"attID: %d, defID: %d, HP: %d\n", outAttackID, outDamageID, outDamageHP);

	CPlayerObject* attackPlayer;
 	if (SINGLETON(CObjectMgr)->FindByObjectID(outAttackID, (CBaseObject**)&attackPlayer) == false)
		return false;

	CPlayerObject* damagePlayer;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outDamageID, (CBaseObject**)&damagePlayer) == false)
		return false;

	attackPlayer->SetHitEffect(damagePlayer->GetCurX(), damagePlayer->GetCurY() - 47);
	damagePlayer->SetHP(outDamageHP);

	return true;
}

bool netPacketProc_Sync(char* recvBuffer)
{
	int outID;
	short outX, outY;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outID);
	ReadFromBuffer(recvBuffer, offset, outX);
	ReadFromBuffer(recvBuffer, offset, outY);

	CPlayerObject* player;
	if (SINGLETON(CObjectMgr)->FindByObjectID(outID, (CBaseObject**)&player) == false)
		return false;

	player->SetPosition(outX, outY);
	AddLog(L"Sync : %d, %d ms\n", outX, outY);

	return true;
}

bool netPacketProc_Echo(char* recvBuffer)
{
	int outEcho;
	size_t offset = 0;

	ReadFromBuffer(recvBuffer, offset, outEcho);

	unsigned int millisec = GetTickCount();
	AddLog(L"Round Trip Time : %u ms\n", millisec - outEcho);

	return true;
}