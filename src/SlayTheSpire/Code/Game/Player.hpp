#pragma once
#include "Game/Entity.hpp"
#include "Game/PlayerBoard.hpp"

class Player : public Entity
{
public:
	Player();
	Player( Widget* parentWidget, 
		Vec2 const& positionRelativeToParent, 
		GameState* gameState, 
		int maxHealth = 0 );

	PlayerBoard& GetPlayerBoard() { return m_playerBoard; }
public:
	PlayerBoard m_playerBoard;
};