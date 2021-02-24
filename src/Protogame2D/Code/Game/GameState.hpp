#pragma once
#include "Game/PlayerBoard.hpp"
#include "Game/Enemy.hpp"


class GameState
{
	GameState( PlayerBoard const& playerBoard, Enemy const& enemy );
	~GameState();

public:
	int m_whoseTurn = 0;
	PlayerBoard m_playerBoard;
	Enemy m_enemy;

	bool m_isGameOver = false;
};