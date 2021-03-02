#pragma once
#include "Game/CardDefinition.hpp"
#include <array>
#include <vector>


class CardPile
{
public:
	CardPile() = default;

	void AddCard( eCard card, int count = 1 );
	void RemoveCard( eCard card, int count = 1 );
	void AddPile( CardPile const& cardPile );
	void ClearPile();
	bool Contains( eCard card ) const;
	std::vector<eCard> ToVector() const;
	int GetTotalSize() const;
private:
	std::array<int, eCard::NUM_CARDS> m_cards;
};