#include "Game/Enemy.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/EnemyDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Delegate.hpp"

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
		m_health = m_maxHealth;
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
	m_intentTextWidget->SetTextSize( 0.15f );
	m_intentWidget->AddChild( m_intentTextWidget );
	m_intentWidget->SetCanHover( false );
	m_intentTextWidget->SetCanHover( false );

	
	m_currentIntent = &MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType );
	Texture const* moveTexture = m_currentIntent->m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );
}

Enemy::~Enemy()
{}

void Enemy::UpdateEnemyMove( RandomNumberGenerator& rng )
{
	m_actionsDone++;
	m_currentEnemyMove = m_enemyDef->GetNextMove( rng, m_actionsDone );

	m_currentIntent = &MoveTypeDefinition::GetMoveTypeDefinitionByType( m_currentEnemyMove.m_moveType );
	Texture const* moveTexture = m_currentIntent->m_moveTypeTexture;
	m_intentWidget->SetTexture( moveTexture, nullptr, nullptr );

	int attackDamage = m_currentEnemyMove.m_damage;
	int damagePostStrength = GetDamagePostStrength( attackDamage );
	if( attackDamage > 0 )
	{
		m_intentTextWidget->SetText( Stringf( "%i", damagePostStrength ) );
	}
	else
	{
		m_intentTextWidget->SetText( "" );
	}

}

void Enemy::Reset()
{
	Entity::Reset();
	m_actionsDone = 0;
	UpdateEnemyMove( g_theGame->m_rand );
}

void Enemy::SetEnemyDef( EnemyDefinition const* enemyDef )
{
	m_enemyDef = enemyDef;
	Texture const* enemyTexture = m_enemyDef->GetTexture();
	m_entityWidget->SetTexture( enemyTexture, nullptr, nullptr );

	m_maxHealth = m_enemyDef->GetMaxHealth();
	m_health = m_maxHealth;
}

bool Enemy::BeginAttack( EventArgs const& args )
{

	m_startTransform = m_entityWidget->GetTransform();
	Transform endTransform = m_startTransform;
	endTransform.m_position.x -= 1.f;

	Delegate<EventArgs const&>& endAnimationDelegate = m_entityWidget->StartAnimation( endTransform, 0.5f, eSmoothingFunction::SMOOTHSTART3 );
	endAnimationDelegate.SubscribeMethod( this, &Enemy::BeginStopAttack );

	return true;
}

bool Enemy::BeginStopAttack( EventArgs const& args )
{
	g_theGame->EnemyDealDamage( EventArgs() );

	Delegate<EventArgs const&>& endAnimationDelegate = m_entityWidget->StartAnimation( m_startTransform, 0.2f, eSmoothingFunction::SMOOTHSTOP3 );
	//endAnimationDelegate.UnsubscribeObject( this );
	endAnimationDelegate.SubscribeMethod( g_theGame, &Game::EndEnemyTurn );

	return true;
}
