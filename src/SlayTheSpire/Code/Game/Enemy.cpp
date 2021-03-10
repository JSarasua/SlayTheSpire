#include "Game/Enemy.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/EnemyDefinition.hpp"

Enemy::Enemy() : Entity()
{
	Texture const* enemyTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Cultist.png" );
	m_entityWidget->SetTexture( enemyTexture, nullptr, nullptr );

	Transform intentTransform;
	intentTransform.m_scale = Vec3( 0.5f, 0.5f, 1.f );
	intentTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, 1.f ) );
	m_intentWidget = new Widget( intentTransform );
	m_entityWidget->AddChild( m_intentWidget );
}

Enemy::Enemy( EnemyDefinition const* enemyDef ) :
	m_enemyDef( enemyDef )
{
	if( m_enemyDef )
	{
		Texture const* enemyTexture = m_enemyDef->GetTexture();
		m_entityWidget->SetTexture( enemyTexture, nullptr, nullptr );
		
		m_maxHealth = m_enemyDef->GetMaxHealth();
	}


	Transform intentTransform;
	intentTransform.m_scale = Vec3( 0.5f, 0.5f, 1.f );
	intentTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, 1.f ) );
	m_intentWidget = new Widget( intentTransform );
	m_entityWidget->AddChild( m_intentWidget );

	Texture const* moveTexture = MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType ).m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );
}

void Enemy::UpdateEnemyMove( RandomNumberGenerator& rng )
{
	m_actionsDone++;
	m_currentEnemyMove = m_enemyDef->GetNextMove( rng, m_actionsDone );

	Texture const* moveTexture = MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType ).m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );
}
