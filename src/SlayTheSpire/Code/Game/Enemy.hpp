#pragma once
#include "Game/Entity.hpp"
#include "Game/EnemyDefinition.hpp"
#include "Engine/UI/WidgetAnimation.hpp"



class RandomNumberGenerator;

class Enemy : public Entity
{
public:
	Enemy();
	Enemy( EnemyDefinition const* enemyDef );
	~Enemy();

	void UpdateEnemyMove( RandomNumberGenerator& rng );
	EnemyMove const& GetEnemyMove() const { return m_currentEnemyMove; }

	virtual void Reset() override;
	void SetEnemyDef( EnemyDefinition const* enemyDef );

	bool BeginAttack( EventArgs const& args );
	bool BeginStopAttack( EventArgs const& args );

public:
	EnemyDefinition const* m_enemyDef = nullptr;
	int m_actionsDone = 0;
	EnemyMove m_currentEnemyMove;
	Transform m_startTransform;
	MoveTypeDefinition const* m_currentIntent = nullptr;
private:
	Widget* m_intentWidget = nullptr;
	Widget* m_intentTextWidget = nullptr;
};