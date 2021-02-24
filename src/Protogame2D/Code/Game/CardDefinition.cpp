#include "Game/CardDefinition.hpp"

std::array<CardDefinition,(size_t)eCard::NUM_CARDS> CardDefinition::s_cardDefs;

CardDefinition const& CardDefinition::GetCardDefinitionByType( eCard cardType )
{
	return s_cardDefs[cardType];
}

CardDefinition::CardDefinition( int costToPlay, int attack, int block, Texture* texture ) :
	m_costToPlay( costToPlay ),
	m_attack( attack ),
	m_block( block ),
	m_texture( texture )
{}

void CardDefinition::InitializeCardDefinitions()
{
	CardDefinition strikeCard = CardDefinition( 1, 6, 0, nullptr );
	CardDefinition blockCard = CardDefinition( 1, 0, 5, nullptr );

	s_cardDefs[Strike] = strikeCard;
	s_cardDefs[Block] = blockCard;
}

