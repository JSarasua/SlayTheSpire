#include "Game/GameState.hpp"

GameState::GameState()
{
	//m_playerBoard.InitializePlayerBoard();
}

void GameState::Update( float deltaSeconds )
{
	m_player.Update( deltaSeconds );
	m_enemy.Update( deltaSeconds );
}

