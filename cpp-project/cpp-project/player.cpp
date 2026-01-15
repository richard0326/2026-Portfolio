#include "stdafx.h"
#include "player.h"

Player::Player() {

}

Player::~Player()
{

}

void Player::SetSessionID(int sessionID)
{
	m_sessionID = sessionID;
}

int Player::GetSessionID()
{
	return m_sessionID;
}