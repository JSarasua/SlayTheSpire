#pragma once

class Widget;
class GameState;

struct Vec2;

class Entity
{
public:
	Entity( Widget* parentWidget, Vec2 const& positionRelativeToParent, GameState* gameState, int maxHealth = 0 );

	virtual void Update( float deltaSeconds ) = 0;

protected:
	int m_health = 0;
	int m_maxHealth = 0;
	int m_block = 0;

	Widget* m_entityWidget = nullptr;
	Widget* m_healthWidget = nullptr;
	Widget* m_statusEffectsWidget = nullptr;

	GameState* m_gameState = nullptr;
};