#pragma once
#include "Game/CardDefinition.hpp"
#include <array>
#include <vector>


class CardPile
{
public:
	void AddCard( eCard card, int count = 1 );
	void RemoveCard( eCard card, int count = 1 );
	void AddPile( CardPile const& cardPile );
	void ClearPile();
	std::vector<eCard> ToVector() const;
private:
	std::array<int, eCard::NUM_CARDS> m_cards;
};