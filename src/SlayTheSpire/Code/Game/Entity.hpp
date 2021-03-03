#pragma once

class Widget;
class GameState;
class WidgetSlider;

struct Vec2;

class Entity
{
public:
	Entity();
	Entity( Widget* parentWidget, Vec2 const& positionRelativeToParent, GameState* gameState, int maxHealth = 0 );

	void SetEntityPositionRelativeToParent( Vec2 const& positionRelativeToParent );
	void SetParentWidget( Widget* parentWidget );

	virtual void Update( float deltaSeconds );

protected:
	int m_health = 0;
	int m_maxHealth = 0;
	int m_block = 0;

	Widget* m_entityWidget = nullptr;
	WidgetSlider* m_healthWidget = nullptr;
	Widget* m_statusEffectsWidget = nullptr;

	GameState* m_gameState = nullptr;
};