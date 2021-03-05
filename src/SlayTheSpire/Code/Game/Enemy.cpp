#include "Game/Enemy.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"


Enemy::Enemy() : Entity()
{
	Texture const* enemyTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Cultist.png" );
	m_entityWidget->SetTexture( enemyTexture, nullptr, nullptr );
}
