#include "Game/CardPile.hpp"

void CardPile::AddCard( eCard card, int count /*= 1 */ )
{
	m_cards[card]+= count;
}

void CardPile::RemoveCard( eCard card, int count /*= 1 */ )
{
	m_cards[card]-= count;

	if( m_cards[card] < 0 )
	{
		m_cards[card] = 0;
	}
}

void CardPile::AddPile( CardPile const& cardPile )
{
	for( size_t cardIndex = 0; cardIndex < m_cards.size(); cardIndex++ )
	{
		m_cards[cardIndex] += cardPile.m_cards[cardIndex];
	}
}

void CardPile::ClearPile()
{
	for( int& card: m_cards )
	{
		card = 0;
	}
}

std::vector<eCard> CardPile::ToVector() const
{
	std::vector<eCard> cards;

	for( size_t cardIndex = 0; cardIndex < m_cards.size(); cardIndex++ )
	{
		eCard card = (eCard)cardIndex;
		size_t cardCount = m_cards[cardIndex];

		for( cardCount; cardCount > 0; cardCount-- )
		{
			cards.push_back( card );
		}
	}

	return cards;
}

