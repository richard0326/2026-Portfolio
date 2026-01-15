#pragma once

class Player
{
public:
	Player();
	~Player();

	void SetSessionID(int sessionID);
	int GetSessionID();
private:
	int m_sessionID;
};