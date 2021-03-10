#include "MoveTypeDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"

MoveTypeDefinition::MoveTypeDefinition( eMoveType moveType, Texture const* moveTypeTexture /*= nullptr */ ) :
	m_moveType( moveType ),
	m_moveTypeTexture( moveTypeTexture )
{}

std::array<MoveTypeDefinition, NUM_MOVETYPES> MoveTypeDefinition::s_moveTypeDefs;

void MoveTypeDefinition::InitializeMoveTypeDefs()
{
	Texture const* attackTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Intent_Attack.png" );
	Texture const* buffTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Intent_Buff.png" );
	Texture const* curseTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Intent_Debuff1.png" );

	s_moveTypeDefs[eMoveType::Attack] = MoveTypeDefinition( eMoveType::Attack, attackTexture );
	s_moveTypeDefs[eMoveType::Buff] = MoveTypeDefinition( eMoveType::Buff, buffTexture );
	s_moveTypeDefs[eMoveType::Curse] = MoveTypeDefinition( eMoveType::Curse, curseTexture );

}

MoveTypeDefinition const& MoveTypeDefinition::GetMoveTypeDefinitionByType( eMoveType moveType )
{
	return s_moveTypeDefs[moveType];
}

