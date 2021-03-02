#pragma once
#include "Game/PlayerBoard.hpp"
#include "Game/Enemy.hpp"


class GameState
{
public:
	GameState( PlayerBoard const& playerBoard, Enemy const& enemy );
	GameState();
	~GameState() {}

public:
	int m_whoseTurn = 0;
	PlayerBoard m_playerBoard;
	//Player m_player;
	Enemy m_enemy;

	bool m_isGameOver = false;
};