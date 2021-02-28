#include "Game/Entity.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/UI/Widget.hpp"
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
	m_entityWidget = new Widget( entityTransform );

	parentWidget->AddChild( m_entityWidget );
}

