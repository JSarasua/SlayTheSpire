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

	void AddCardToPermanentDeck( eCard card );
	void AddCardToDeckAndShuffle( eCard card, int cardCount = 1 );
	void AddCardToDiscardPile( eCard card );
	void RemoveCardFromDeck( eCard card );
	void RemoveCardFromDiscardPile( eCard card );
	void MoveDiscardPileToDeckAndShuffle();
	bool TryMoveCardFromHandToDiscardPile( eCard card );
	void DiscardHand();
	void ShuffleDeck();
	eCard DrawCard();
	void DrawHand(); //5 cards

	bool CanConsumeEnergy( int energyToConsume ) const { return energyToConsume <= m_playerEnergy; }
	void ConsumeEnergy( int energyToConsume );
	void ResetEnergy() { m_playerEnergy = m_playerMaxEnergy; }
	void GainEnergy( int energyToGain ) { m_playerEnergy += energyToGain; }

	int GetDeckSize() const;
	int GetHandSize() const;
	int GetDiscardPileSize() const;
	int GetEnergy() const { return m_playerEnergy; }
	int GetMaxEnergy() const { return m_playerMaxEnergy; }
	int GetStrikeCardCount() const;
	std::vector<eCard> GetHandAsVector() const;

	void Reset();
	void FullReset();
public:
	std::vector<eCard> m_deck; 
	CardPile m_hand;
	CardPile m_discardPile;
	CardPile m_permanentDeck;
	CardPile m_startingDeck;
	
	int m_playerEnergy = 0;
	int m_playerMaxEnergy = 3;
	int m_playerHealth = 0;
	int m_playerMaxHealth = 80;
	int m_playerBlock = 0;
	GameState* m_parentState = nullptr;
};