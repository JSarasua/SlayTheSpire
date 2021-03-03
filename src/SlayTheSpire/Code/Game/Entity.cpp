#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetSlider.hpp"
#include "Engine/Math/Transform.hpp"

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
	entityTransform.m_scale = Vec3( 1.f, 1.5f, 1.f );
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
	entityTransform.m_scale = Vec3( 1.f, 1.5f, 1.f );
	m_entityWidget = new Widget( entityTransform );

	Texture* redTexture = g_theRenderer->CreateTextureFromColor( Rgba8::RED );
	Texture* greenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::GREEN );

	Transform healthTransform = entityTransform;
	healthTransform.m_position.y -= 1.f;
	m_healthWidget = new WidgetSlider( healthTransform );
	m_healthWidget->SetBackgroundAndFillTextures( redTexture, greenTexture );
	
	m_entityWidget->AddChild( m_healthWidget );
}

void Entity::SetEntityPositionRelativeToParent( Vec2 const& positionRelativeToParent )
{
	Vec2 position = positionRelativeToParent;
	m_entityWidget->SetPosition( position );
	position.y -= 1.f;
	m_healthWidget->SetPosition( position );
}

void Entity::SetParentWidget( Widget* parentWidget )
{
	if( m_entityWidget )
	{
		parentWidget->AddChild( m_entityWidget );
	}

	if( m_healthWidget )
	{
		parentWidget->AddChild( m_healthWidget );
	}

	if( m_statusEffectsWidget )
	{
		parentWidget->AddChild( m_statusEffectsWidget );
	}
}

void Entity::Update( float deltaSeconds )
{
	float healthRatio = (float)m_health/(float)m_maxHealth;
	m_healthWidget->SetSliderValue( healthRatio );
}

