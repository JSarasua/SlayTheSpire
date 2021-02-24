#pragma once
#include "Game/CardPile.hpp"
#include "Game/CardDefinition.hpp"
#include <vector>

class GameState;

class PlayerBoard
{

public:
	std::vector<eCard> m_deck; 
	CardPile m_hand;
	CardPile m_discardPile;
	
	int m_playerEnergy = 0;
	int m_playerMaxEnergy = 3;
	int m_playerHealth = 0;
	int m_playerBlock = 0;
	GameState* m_parentState = nullptr;
};