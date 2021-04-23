#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetGrid.hpp"

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

void Player::FullReset()
{
	Entity::Reset();
	m_playerBoard.FullReset();
}

void Player::ResetNoHealth()
{
	m_strength = 0;
	m_block = 0;
	m_currentStatuses.clear();
	m_statusEffectsWidget->ClearChildren();
	m_strengthWidget = nullptr;
	m_playerBoard.Reset();
	
}

