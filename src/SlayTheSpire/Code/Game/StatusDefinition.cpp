#include "Game/StatusDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"

std::array<StatusDefinition, (size_t)eStatus::NUM_STATUS> StatusDefinition::s_statusDefs;

StatusDefinition const& StatusDefinition::GetStatusDefinitionByType( eStatus statusDefinition )
{
	return s_statusDefs[statusDefinition];
}

void StatusDefinition::InitializeStatusDefinitions()
{
	Texture* vulnerableTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Vulnerable.png" );
	Texture* weakTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Weak.png" );
	Texture* ritualTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Icon_Ritual.png" );
	s_statusDefs[Vulnerable] = StatusDefinition( Vulnerable, vulnerableTexture, 0, 1.f, 1.5f );
	s_statusDefs[Weak] = StatusDefinition( Weak, weakTexture, 0, 0.75f, 1.f );
	s_statusDefs[Ritual] = StatusDefinition( Ritual, ritualTexture, 3, 1.f, 1.f );
}

StatusDefinition::StatusDefinition( eStatus statusType, Texture const* statusTexture, int strengthPerTurn, float damageToOpponentModifier, float damageTakenModifier ) :
	m_statusType( statusType ),
	m_statusTexture( statusTexture ),
	m_strengthPerTurn( strengthPerTurn ),
	m_damageToOpponentModifier( damageToOpponentModifier ),
	m_damageTakenModifier( damageTakenModifier )
{}

