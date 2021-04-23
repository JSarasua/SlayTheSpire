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
	void RoundWon() { m_roundsWon++; }

public:
	int m_whoseTurn = 0;
	Player m_player;
	Enemy m_enemy;

	bool m_isGameOver = false;
	int m_roundsWon = 0;
};