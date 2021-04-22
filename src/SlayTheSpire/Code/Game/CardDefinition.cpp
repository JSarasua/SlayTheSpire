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



CardDefinition::CardDefinition( eCard cardType, int costToPlay, int attack, int block, Texture const* texture, 
	bool isSpotWeakness, bool isExhaust /*= false*/, int cardDraw /*= 0*/, 
	int maxHealthIncrease /*= 0*/, int enemyStrengthModifier /*= 0*/, int strengthPerTurn /*= 0*/, bool isLimitBreak /*= false */ ):
	m_cardType( cardType ),
	m_costToPlay( costToPlay ),
	m_attack( attack ),
	m_block( block ),
	m_texture( texture ),
	m_isSpotWeakness( isSpotWeakness ),
	m_isExhaust( isExhaust ),
	m_cardDraw( cardDraw ),
	m_maxHealthIncrease( maxHealthIncrease ),
	m_enemyStrengthModifier( enemyStrengthModifier ),
	m_strengthPerTurn( strengthPerTurn ),
	m_isLimitBreak( isLimitBreak )
{}

void CardDefinition::InitializeCardDefinitions()
{
	Texture const* strikeTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Strike_r.png" );
	Texture const* defendTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Defend_r.png" );
	Texture const* spotWeaknessTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/SpotWeakness.png" );
	Texture const* shrugItOffTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ShrugItOff.png" );
	Texture const* bludgeonTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Bludgeon.png" );
	Texture const* imperviousTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Impervious.png" );
	Texture const* feedTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Feed.png" );
	Texture const* disarmTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Disarm.png" );
	Texture const* ironWaveTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/IronWave.png" );
	Texture const* limitBreakTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/LimitBreak.png" );


	CardDefinition strikeCard = CardDefinition( Strike, 1, 6, 0, strikeTexture );
	CardDefinition defendCard = CardDefinition( Defend, 1, 0, 5, defendTexture );
	CardDefinition spotWeaknessCard = CardDefinition( SpotWeakness, 1, 0, 0, spotWeaknessTexture, true );
	CardDefinition shrugItOffCard = CardDefinition( ShrugItOff, 1, 0, 8, shrugItOffTexture, false, false, 1 );
	CardDefinition bludgeonCard = CardDefinition( Bludgeon, 3, 32, 0, bludgeonTexture );
	CardDefinition imperviousCard = CardDefinition( Impervious, 2, 0, 30, imperviousTexture, false, true );
	CardDefinition feedCard = CardDefinition( Feed, 2, 10, 0, feedTexture, false, true, 0, 3 );
	CardDefinition disarmCard = CardDefinition( Disarm, 1, 0, 0, disarmTexture, false, true, 0, 0, -2 );
	CardDefinition ironWaveCard = CardDefinition( IronWave, 1, 5, 5, ironWaveTexture );
	CardDefinition limitBreakCard = CardDefinition( LimitBreak, 1, 0, 0, limitBreakTexture, false, true, 0, 0, 0, 0, true );

	s_cardDefs[Strike] = strikeCard;
	s_cardDefs[Defend] = defendCard;
	s_cardDefs[SpotWeakness] = spotWeaknessCard;
	s_cardDefs[ShrugItOff] = shrugItOffCard;
	s_cardDefs[Bludgeon] = bludgeonCard;
	s_cardDefs[Impervious] = imperviousCard;
	s_cardDefs[Feed] = feedCard;
	s_cardDefs[Disarm] = disarmCard;
	s_cardDefs[IronWave] = ironWaveCard;
	s_cardDefs[LimitBreak] = limitBreakCard;

}

