#include "Game/PlayerBoard.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

void PlayerBoard::InitializePlayerBoard()
{
	m_deck.clear();
	m_hand.ClearPile();
	m_discardPile.ClearPile();
	m_permanentDeck.AddCard( Strike, 7 );
	m_permanentDeck.AddCard( Defend, 3 );
	m_deck = m_permanentDeck.ToVector();

	ShuffleDeck();

	m_playerEnergy = m_playerMaxEnergy;
	m_playerHealth = m_playerMaxHealth;
	m_playerBlock = 0;
}

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

	m_discardPile.ClearPile();
}

bool PlayerBoard::TryMoveCardFromHandToDiscardPile( eCard card )
{
	if( m_hand.Contains( card ) )
	{
		m_hand.RemoveCard( card );
		m_discardPile.AddCard( card );
		return true;
	}
	else
	{
		return false;
	}
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
	int maxIndex = (int)m_deck.size() - 1;

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

void PlayerBoard::ConsumeEnergy( int energyToConsume )
{
	m_playerEnergy -= energyToConsume;
	m_playerEnergy = MaxInt( m_playerEnergy, 0 );
}

int PlayerBoard::GetDeckSize() const
{
	return (int)m_deck.size();
}

int PlayerBoard::GetHandSize() const
{
	return m_hand.GetTotalSize();
}

int PlayerBoard::GetDiscardPileSize() const
{
	return m_discardPile.GetTotalSize();
}

std::vector<eCard> PlayerBoard::GetHandAsVector() const
{
	return m_hand.ToVector();
}

void PlayerBoard::Reset()
{
	m_deck.clear();
	m_hand.ClearPile();
	m_discardPile.ClearPile();

	m_deck = m_permanentDeck.ToVector();

	ShuffleDeck();
	DrawHand();

	m_playerEnergy = m_playerMaxEnergy;
	m_playerHealth = m_playerMaxHealth;
	m_playerBlock = 0;
}

