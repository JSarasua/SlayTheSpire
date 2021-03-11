#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetSlider.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/UI/WidgetGrid.hpp"

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


	Texture* blockTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Icon_Block.png" );
	Transform blockTransform = Transform();
	blockTransform.m_position.x = -1.2f;
	blockTransform.m_scale = Vec3( 0.6f, 0.6f, 1.f );
	m_blockWidget = new Widget( blockTransform );
	m_blockWidget->SetTexture( blockTexture, nullptr, nullptr );
	m_blockWidget->SetCanHover( false );
	m_blockWidget->SetText( "0" );
	m_blockWidget->SetTextSize( 0.4f );
	m_healthWidget->AddChild( m_blockWidget );

	Transform statusTransform = Transform();
	statusTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, -0.15f ) );
	statusTransform.m_scale = Vec3( 2.f, 0.15f, 1.f );
	m_statusEffectsWidget = new WidgetGrid( statusTransform, m_statusEffectsDimensions );
	m_entityWidget->AddChild( m_statusEffectsWidget );
	m_statusEffectsWidget->SetIsVisible( false );
	m_statusEffectsWidget->SetCanHover( false );
	m_statusEffectsWidget->SetCanSelect( false );
	m_statusEffectsWidget->SetCanDrag( false );
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
	healthTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, -0.05f ) );
	healthTransform.m_scale = Vec3( 2.f, 0.2f, 1.f );
	m_healthWidget = new WidgetSlider( healthTransform );
	m_healthWidget->SetBackgroundAndFillTextures( redTexture, greenTexture );
	
	m_entityWidget->AddChild( m_healthWidget );

	Texture* blockTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Icon_Block.png" );
	Transform blockTransform = Transform();
	blockTransform.m_position.x = -1.2f;
	blockTransform.m_scale = Vec3( 0.6f, 0.6f, 1.f );
	m_blockWidget = new Widget( blockTransform );
	m_blockWidget->SetTexture( blockTexture, nullptr, nullptr );
	m_blockWidget->SetCanHover( false );
	m_blockWidget->SetText( "0" );
	m_blockWidget->SetTextSize( 0.4f );
	m_healthWidget->AddChild( m_blockWidget );

	Transform statusTransform = Transform();
	statusTransform.m_position = m_entityWidget->GetLocalAABB2().GetPointAtUV( Vec2( 0.5f, -0.15f ) );
	statusTransform.m_scale = Vec3( 2.f, 0.15f, 1.f );
	m_statusEffectsWidget = new WidgetGrid( statusTransform, m_statusEffectsDimensions );
	m_entityWidget->AddChild( m_statusEffectsWidget );
	m_statusEffectsWidget->SetIsVisible( false );
	m_statusEffectsWidget->SetCanHover( false );
	m_statusEffectsWidget->SetCanSelect( false );
	m_statusEffectsWidget->SetCanDrag( false );
}

Entity::~Entity()
{
// 	if( m_entityWidget )
// 	{
// 		Widget* parentWidget = m_entityWidget->GetParentWidget();
// 		if( parentWidget )
// 		{
// 			parentWidget->RemoveChildWidget( m_entityWidget );
// 		}
// 
// 		delete m_entityWidget;
// 		m_entityWidget = nullptr;
// 	}
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

	g_theEventSystem->FireEvent( "checkFightOver", NOCONSOLECOMMAND, nullptr );
}

void Entity::AddStatus( eStatus status )
{
	StatusDefinition const& statusDef = StatusDefinition::GetStatusDefinitionByType( status );
	Texture const* statusTexture = statusDef.m_statusTexture;

	m_currentStatuses.push_back( status );

	Transform statusTransform;
	statusTransform.m_scale = Vec3( 0.3f, 0.3f, 1.f );
	Widget* statusWidget = new Widget( statusTransform );
	statusWidget->SetCanHover( false );
	statusWidget->SetCanSelect( false );
	statusWidget->SetCanDrag( false );
	statusWidget->SetTexture( statusTexture, nullptr, nullptr );
	m_statusEffectsWidget->AddChild( statusWidget );
}

void Entity::AddStength( int strength )
{
	m_strength += strength;

	if( m_strengthWidget )
	{
		m_strengthWidget->SetText( Stringf( "%i", m_strength ) );
	}
	else
	{
		if( strength != 0 )
		{
			Texture const* strengthTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Icon_Strength.png" );
			Transform statusTransform;
			statusTransform.m_scale = Vec3( 0.3f, 0.3f, 1.f );
			m_strengthWidget = new Widget( statusTransform );
			m_strengthWidget->SetCanHover( false );
			m_strengthWidget->SetCanSelect( false );
			m_strengthWidget->SetCanDrag( false );
			m_strengthWidget->SetTexture( strengthTexture, nullptr, nullptr );
			m_strengthWidget->SetText( Stringf( "%i", m_strength ) );
			m_strengthWidget->SetTextSize( 0.3f );
			m_statusEffectsWidget->AddChild( m_strengthWidget );
		}
	}


}

void Entity::UpdateStatuses()
{
	for( eStatus& status : m_currentStatuses )
	{
		StatusDefinition const& statusDef = StatusDefinition::GetStatusDefinitionByType( status );

		if( status == Ritual )
		{
			AddStength( statusDef.m_strengthPerTurn );
		}
	}
}

