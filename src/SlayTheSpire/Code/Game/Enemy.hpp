#pragma once
#include "Game/Entity.hpp"
#include "Game/EnemyDefinition.hpp"



class RandomNumberGenerator;

class Enemy : public Entity
{
public:
	Enemy();
	Enemy( EnemyDefinition const* enemyDef );

	void UpdateEnemyMove( RandomNumberGenerator& rng );
	EnemyMove const& GetEnemyMove() const { return m_currentEnemyMove; }
public:
	EnemyDefinition const* m_enemyDef = nullptr;
	int m_actionsDone = 0;
	EnemyMove m_currentEnemyMove;

private:
	Widget* m_intentWidget = nullptr;
	Widget* m_intentTextWidget = nullptr;
};