#include "Game/PlayerBoard.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

void PlayerBoard::AddCardToDeckAndShuffle( eCard card )
{
	m_deck.push_back( card );
	ShuffleDeck();
}

void PlayerBoard::AddCardToDiscardPile( eCard card )
{
	m_discardPile.AddCard( card );
}

void PlayerBoard::RemoveCardFromDeck( eCard card )
{
	for( size_t deckIndex = 0; deckIndex < m_deck.size(); deckIndex++ )
	{
		if( m_deck[deckIndex] == card )
		{
			m_deck[deckIndex] = m_deck.back();
			m_deck.pop_back();
			break;
		}
	}
}

void PlayerBoard::RemoveCardFromDiscardPile( eCard card )
{
	m_discardPile.RemoveCard( card );
}

void PlayerBoard::MoveDiscardPileToDeckAndShuffle()
{
	std::vector<eCard> discardVector = m_discardPile.ToVector();
	m_deck.insert( m_deck.end(), discardVector.begin(), discardVector.end() );
	ShuffleDeck();
}

void PlayerBoard::DiscardHand()
{
	m_discardPile.AddPile( m_hand );
	m_hand.ClearPile();
}

void PlayerBoard::ShuffleDeck()
{
 	RandomNumberGenerator& rng = g_theGame->m_rand;

	int currentIndex = 0;
	int maxIndex = (int)m_deck.size();

	while( currentIndex < maxIndex )
	{
		int indexToSwap = rng.RollRandomIntInRange( currentIndex, maxIndex );
		eCard firstCard = m_deck[currentIndex];
		eCard secondCard = m_deck[indexToSwap];

		m_deck[currentIndex] = secondCard;
		m_deck[indexToSwap] = firstCard;

		currentIndex++;
	}
}

void PlayerBoard::DrawCard()
{
	if( m_deck.size() == 0 )
	{
		MoveDiscardPileToDeckAndShuffle();

	}
	
	eCard drawnCard = m_deck.back();
	m_deck.pop_back();
	m_hand.AddCard( drawnCard );
}

void PlayerBoard::DrawHand()
{
	DrawCard();
	DrawCard();
	DrawCard();
	DrawCard();
	DrawCard();
}

