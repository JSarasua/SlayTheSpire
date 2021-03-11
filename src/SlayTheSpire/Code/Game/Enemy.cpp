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
	intentTransform.m_scale = Vec3( 0.6f, 0.6f, 1.f );
	intentTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, 1.f ) );
	m_intentWidget = new Widget( intentTransform );
	
	Transform intentTextTransform;
	intentTextTransform.m_scale = Vec3( 0.4f, 0.4f, 1.f );
	intentTextTransform.m_position = m_intentWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.f, 0.f ) );
	m_intentTextWidget = new Widget( intentTextTransform );
	m_intentTextWidget->SetTextSize( 0.4f );
	m_intentWidget->AddChild( m_intentTextWidget );
	m_intentWidget->SetCanHover( false );
	m_intentTextWidget->SetCanHover( false );
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
	intentTransform.m_scale = Vec3( 0.6f, 0.6f, 1.f );
	intentTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, 1.f ) );
	m_intentWidget = new Widget( intentTransform );
	m_entityWidget->AddChild( m_intentWidget );

	Transform intentTextTransform;
	intentTextTransform.m_scale = Vec3( 0.4f, 0.4f, 1.f );
	intentTextTransform.m_position = m_intentWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.1f, 0.1f ) );
	m_intentTextWidget = new Widget( intentTextTransform );
	m_intentTextWidget->SetTextSize( 0.4f );
	m_intentWidget->AddChild( m_intentTextWidget );
	m_intentWidget->SetCanHover( false );
	m_intentTextWidget->SetCanHover( false );

	Texture const* moveTexture = MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType ).m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );
}

void Enemy::UpdateEnemyMove( RandomNumberGenerator& rng )
{
	m_actionsDone++;
	m_currentEnemyMove = m_enemyDef->GetNextMove( rng, m_actionsDone );

	Texture const* moveTexture = MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType ).m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );

	int attackDamage = m_currentEnemyMove.m_damage;
	if( m_currentEnemyMove.m_moveType == Attack )
	{
		attackDamage = GetDamagePostStrength( attackDamage );
		m_intentTextWidget->SetText( Stringf( "%i", attackDamage ) );
	}
	else
	{
		m_intentTextWidget->SetText( "" );
	}

}
