#pragma once
#include "Game/StatusDefinition.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


class Widget;
class GameState;
class WidgetSlider;
class WidgetGrid;

struct Vec2;

class Entity
{
public:
	Entity();
	Entity( Widget* parentWidget, Vec2 const& positionRelativeToParent, GameState* gameState, int maxHealth = 0 );
	virtual ~Entity();


	void SetEntityPositionRelativeToParent( Vec2 const& positionRelativeToParent );
	void SetParentWidget( Widget* parentWidget );

	virtual void Update( float deltaSeconds );
	void TakeDamage( int damage );
	void GainBlock( int block ) { m_block += block; }
	void ResetBlock() { m_block = 0; }
	int GetDamagePostStrength ( int damage, int multiplier = 1 ) const;
	void GainHealth( int healthToGain ) { m_health += healthToGain; }
	void AddStatus( eStatus status );
	void AddStrength( int strength );
	void UpdateStatuses();
	int GetStrength() const { return m_strength; }
	int GetHealth() const { return m_health; }
	void AddMaxHealth( int health );
	virtual void Reset();

protected:
	int m_health = 80;
	int m_maxHealth = 80;
	int m_block = 0;
	int m_strength = 0;
	int m_dexterity = 0;
	std::vector<eStatus> m_currentStatuses;

	Widget* m_entityWidget = nullptr;
	WidgetSlider* m_healthWidget = nullptr;
	Widget* m_blockWidget = nullptr;
	IntVec2 m_statusEffectsDimensions = IntVec2( 5, 1 );
	WidgetGrid* m_statusEffectsWidget = nullptr;

	Widget* m_strengthWidget = nullptr;

	GameState* m_gameState = nullptr;
};