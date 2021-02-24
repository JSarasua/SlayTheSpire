#pragma once
#include "Game/CardPile.hpp"
#include "Game/CardDefinition.hpp"
#include <vector>

class GameState;

class PlayerBoard
{
public:
	PlayerBoard() {}
	PlayerBoard( std::vector<eCard> const& deck, CardPile const& hand, CardPile const& discardPile,
		int playerEnergy, int playerMaxEnergy, int playerHealth, int playerBlock, GameState* parentState );

	void InitializePlayerBoard();

	void AddCardToDeckAndShuffle( eCard card );
	void AddCardToDiscardPile( eCard card );
	void RemoveCardFromDeck( eCard card );
	void RemoveCardFromDiscardPile( eCard card );
	void MoveDiscardPileToDeckAndShuffle();
	void DiscardHand();
	void ShuffleDeck();
	void DrawCard();
	void DrawHand(); //5 cards

	int GetDeckSize() const;
	int GetHandSize() const;
	int GetDiscardPileSize() const;

public:
	std::vector<eCard> m_deck; 
	CardPile m_hand;
	CardPile m_discardPile;
	
	int m_playerEnergy = 0;
	int m_playerMaxEnergy = 3;
	int m_playerHealth = 0;
	int m_playerMaxHealth = 80;
	int m_playerBlock = 0;
	GameState* m_parentState = nullptr;
};