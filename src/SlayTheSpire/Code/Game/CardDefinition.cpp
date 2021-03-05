#include "Game/CardDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

std::array<CardDefinition,(size_t)eCard::NUM_CARDS> CardDefinition::s_cardDefs;

CardDefinition const& CardDefinition::GetCardDefinitionByType( eCard cardType )
{
	return s_cardDefs[cardType];
}

CardDefinition::CardDefinition( eCard cardType, int costToPlay, int attack, int block, Texture const* texture ) :
	m_cardType( cardType ),
	m_costToPlay( costToPlay ),
	m_attack( attack ),
	m_block( block ),
	m_texture( texture )
{}

void CardDefinition::InitializeCardDefinitions()
{
	Texture const* strikeTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Strike_r.png" );
	Texture const* defendTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Defend_r.png" );

	CardDefinition strikeCard = CardDefinition( Strike, 1, 6, 0, strikeTexture );
	CardDefinition defendCard = CardDefinition( Defend, 1, 0, 5, defendTexture );

	s_cardDefs[Strike] = strikeCard;
	s_cardDefs[Defend] = defendCard;
}

