#pragma once
#include "Game/PlayerBoard.hpp"
#include "Game/Enemy.hpp"
#include "Game/Player.hpp"


class GameState
{
public:
	GameState( PlayerBoard const& playerBoard, Enemy const& enemy );
	GameState();
	~GameState() {}

	void Update( float deltaSeconds );

public:
	int m_whoseTurn = 0;
	//PlayerBoard m_playerBoard;
	Player m_player;
	Enemy m_enemy;

	bool m_isGameOver = false;
};