#pragma once

class EnemyDefinition;

class Enemy
{
public:
	void TakeDamage( int damage );

public:
	EnemyDefinition const* m_enemyDef = nullptr;
	int m_enemyHealth = 100;
	int m_enemyMaxHealth = 100;
	int m_enemyBlock = 0;
};