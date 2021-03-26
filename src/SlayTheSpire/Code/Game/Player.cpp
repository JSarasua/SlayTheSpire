#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/UI/Widget.hpp"

Player::Player( Widget* parentWidget, 
	Vec2 const& positionRelativeToParent, 
	GameState* gameState, 
	int maxHealth /*= 0 */ ) :
	Entity( parentWidget, positionRelativeToParent, gameState, maxHealth )
{
	m_playerBoard.InitializePlayerBoard();


	Texture* playerTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Ironclad.png" );

	m_entityWidget->SetTexture( playerTexture, nullptr, nullptr );
}

Player::Player() : Entity(), m_playerBoard()
{
	Texture const* playerTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Ironclad.png" );
	m_entityWidget->SetTexture( playerTexture, nullptr, nullptr );
	//m_playerBoard.InitializePlayerBoard();
}

void Player::Reset()
{
	Entity::Reset();
	m_playerBoard.Reset();
}

