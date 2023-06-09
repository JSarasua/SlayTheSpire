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
	StatusDefinition const& invalidDef = StatusDefinition::GetStatusDefinitionByType( eStatus::INVALID_STATUS );
 	
	std::vector<EnemyMove> cultistOrderedMoves;
	StatusDefinition const& ritualDef = StatusDefinition::GetStatusDefinitionByType( Ritual );
	EnemyMove ritualMove = EnemyMove( Buff, 0, 0, 0, false, false, &ritualDef );
	cultistOrderedMoves.push_back( ritualMove );

	std::vector<EnemyMove> cultistMoves;
	EnemyMove attackMove = EnemyMove( Attack, 6, 0, 0, false, false, &invalidDef );
	cultistMoves.push_back( attackMove );
	Texture const* cultistTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Cultist.png" );

	std::vector<EnemyMove> jawWormOrderedMoves;
	//StatusDefinition const& ritualDef = StatusDefinition::GetStatusDefinitionByType( Ritual );
	EnemyMove jawWormChompMove = EnemyMove( Attack, 11, 0, 0, false, false, &invalidDef );
	jawWormOrderedMoves.push_back( jawWormChompMove );

	std::vector<EnemyMove> jawWormMoves;
	//StatusDefinition const& ritualDef = StatusDefinition::GetStatusDefinitionByType( Ritual );
	EnemyMove jawWormThrashMove = EnemyMove( AttackDefend, 7, 5, 0, false, false, &invalidDef );
	EnemyMove jawWormBellowMove = EnemyMove( BuffDefend, 0, 6, 3, false, false, &invalidDef );
	jawWormMoves.push_back( jawWormChompMove );
	jawWormMoves.push_back( jawWormThrashMove );
	jawWormMoves.push_back( jawWormBellowMove );
	Texture const* jawWormTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/JawWorm.png" );


	s_enemyDefs[Cultist] = EnemyDefinition( cultistOrderedMoves, cultistMoves, 48, cultistTexture );
	s_enemyDefs[JawWorm] = EnemyDefinition( jawWormOrderedMoves, jawWormMoves, 40, jawWormTexture );
}



EnemyDefinition const& EnemyDefinition::GetRandomEnemyDefinition( RandomNumberGenerator& rng )
{
	int enemyIndex = rng.RollRandomIntLessThan( (int)NUM_ENEMIES );
	return s_enemyDefs[enemyIndex];
}

EnemyMove EnemyDefinition::GetNextMove( RandomNumberGenerator& rng, int moveTurn ) const
{
	if( moveTurn <= m_orderedMoves.size() && moveTurn > 0 )
	{
		int moveIndex = moveTurn - 1;
		return m_orderedMoves[moveIndex];
	}
	else
	{
		return GetRandomMove( rng );
	}
}

EnemyMove EnemyDefinition::GetRandomMove( RandomNumberGenerator& rng ) const
{
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
