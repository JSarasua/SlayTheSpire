#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetSlider.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"

Entity::Entity( Widget* parentWidget, 
	Vec2 const& positionRelativeToParent, 
	GameState* gameState, 
	int maxHealth ) :
	m_gameState( gameState ),
	m_maxHealth( maxHealth ),
	m_health( maxHealth )
{
	Transform entityTransform = Transform();
	entityTransform.m_position = positionRelativeToParent;
	entityTransform.m_scale = Vec3( 2.5f, 2.5f, 1.f );
	m_entityWidget = new Widget( entityTransform );

	parentWidget->AddChild( m_entityWidget );

	Texture* redTexture = g_theRenderer->CreateTextureFromColor( Rgba8::RED );
	Texture* greenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::GREEN );

	Transform healthTransform = entityTransform;
	healthTransform.m_position.y -= 1.f;
	m_healthWidget = new WidgetSlider( healthTransform );
	m_healthWidget->SetBackgroundAndFillTextures( redTexture, greenTexture);
	parentWidget->AddChild( m_healthWidget );

}

Entity::Entity()
{
	Transform entityTransform = Transform();
	entityTransform.m_scale = Vec3( 2.5f, 2.5f, 1.f );
	m_entityWidget = new Widget( entityTransform );
	m_entityWidget->SetCanHover( false );

	Texture* redTexture = g_theRenderer->CreateTextureFromColor( Rgba8::RED );
	Texture* greenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::GREEN );

	Transform healthTransform = Transform();
	healthTransform.m_position.y -= 1.5f;
	healthTransform.m_scale = Vec3( 2.f, 0.2f, 1.f );
	m_healthWidget = new WidgetSlider( healthTransform );
	m_healthWidget->SetBackgroundAndFillTextures( redTexture, greenTexture );
	
	m_entityWidget->AddChild( m_healthWidget );

	Texture* blockTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Icon_Block.png" );
	Transform blockTransform = Transform();
	blockTransform.m_position.x = 1.2f;
	blockTransform.m_scale = Vec3( 0.6f, 0.6f, 1.f );
	m_blockWidget = new Widget( blockTransform );
	m_blockWidget->SetTexture( blockTexture, nullptr, nullptr );
	m_blockWidget->SetCanHover( false );
	m_blockWidget->SetText( "0" );
	m_blockWidget->SetTextSize( 0.4f );
	m_healthWidget->AddChild( m_blockWidget );
	//m_entityWidget->AddChild( m_statusEffectsWidget )
}

void Entity::SetEntityPositionRelativeToParent( Vec2 const& positionRelativeToParent )
{
	Vec2 position = positionRelativeToParent;
	m_entityWidget->SetPosition( position );
}

void Entity::SetParentWidget( Widget* parentWidget )
{
	if( m_entityWidget )
	{
		parentWidget->AddChild( m_entityWidget );
	}

// 	if( m_healthWidget )
// 	{
// 		parentWidget->AddChild( m_healthWidget );
// 	}
// 
// 	if( m_statusEffectsWidget )
// 	{
// 		parentWidget->AddChild( m_statusEffectsWidget );
// 	}
}

void Entity::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	float healthRatio = (float)m_health/(float)m_maxHealth;
	m_healthWidget->SetSliderValue( healthRatio );

	m_blockWidget->SetText( Stringf( "%i", m_block ) );
}

void Entity::TakeDamage( int damage )
{
	int mitigatedDamage = damage - m_block;
	mitigatedDamage = MaxInt( mitigatedDamage, 0 );

	m_block = MaxInt( m_block - damage, 0 );

	m_health -= mitigatedDamage;
	m_health = MaxInt( m_health, 0 );
}

