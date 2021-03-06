#include "Game/EnemyDefinition.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/StatusDefinition.hpp"

std::array<EnemyDefinition, (size_t)eEnemy::NUM_ENEMIES> EnemyDefinition::s_enemyDefs;


EnemyDefinition::EnemyDefinition( std::vector<EnemyMove> orderedMoves, std::vector<EnemyMove> moves, int maxHealth, Texture const* texture ):
	m_maxHealth( maxHealth ),
	m_enemyTexture( texture ),
	m_moves( moves ),
	m_orderedMoves( orderedMoves )
{}

void EnemyDefinition::InitializeEnemyDefinitions()
{
// 	std::vector<EnemyMove> cultistOrderedMoves;
// 	cultistOrderedMoves( Buff, 0, 0, )
// 	std::vector<EnemyMove> cultistMoves;
// 	cultistMoves.emplace_back( Attack, 6, 0, 0, false, false );
// 
// 	Texture const* cultistTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Cultist.png" );
// 	s_enemyDefs[Cultist] = EnemyDefinition( cultistMoves, 80, cultistTexture );
}
EnemyMove EnemyDefinition::GetRandomMove( RandomNumberGenerator& rng, int moveTurn )
{
	if( moveTurn <= m_orderedMoves.size() && moveTurn > 0 )
	{
		int moveIndex = moveTurn - 1;
		return m_orderedMoves[moveIndex];
	}

	int numberOfMoves = (int)m_moves.size();

	if( numberOfMoves > 0 )
	{
		int randomIndex = rng.RollRandomIntLessThan( numberOfMoves );
		return m_moves[randomIndex];
	}
	else
	{
		ERROR_AND_DIE( "No elements in moves list" );
	}
}

EnemyMove::EnemyMove( eMoveType move, int damage, int block, int strength, bool giveVulnerable, bool giveWeak, StatusDefinition const* statusDef ): 
	m_moveType( move ),
	m_damage( damage ),
	m_block( block ),
	m_strength( strength ),
	m_giveVulnerable( giveVulnerable ),
	m_giveWeak( giveWeak ),
	m_statusDef( statusDef )
{}
