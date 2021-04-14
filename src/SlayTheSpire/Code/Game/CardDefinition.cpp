#include "Game/CardDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/Game.hpp"

std::array<CardDefinition,(size_t)eCard::NUM_CARDS> CardDefinition::s_cardDefs;

CardDefinition const& CardDefinition::GetCardDefinitionByType( eCard cardType )
{
	return s_cardDefs[cardType];
}

CardDefinition const& CardDefinition::GetRandomCardDefinition()
{
	int cardType = g_theGame->m_rand.RollRandomIntInRange( 0, NUM_CARDS - 1 );

	return GetCardDefinitionByType( (eCard)cardType );
}

eCard CardDefinition::GetRandomCardType()
{
	int cardType = g_theGame->m_rand.RollRandomIntInRange( 0, NUM_CARDS - 1 );
	return (eCard)cardType;
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
	Texture const* spotWeaknessTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/SpotWeakness.png" );

	CardDefinition strikeCard = CardDefinition( Strike, 1, 6, 0, strikeTexture );
	CardDefinition defendCard = CardDefinition( Defend, 1, 0, 5, defendTexture );
	CardDefinition spotWeaknessCard = CardDefinition( SpotWeakness, 1, 0, 0, spotWeaknessTexture );
	spotWeaknessCard.m_isSpotWeakness = true;

	s_cardDefs[Strike] = strikeCard;
	s_cardDefs[Defend] = defendCard;
	s_cardDefs[SpotWeakness] = spotWeaknessCard;
}

