#pragma once

class EnemyDefinition;

class Enemy
{
public:


private:
	EnemyDefinition const* m_enemyDef = nullptr;
	int m_enemyHealth = 0;
	int m_enemyBlock = 0;
};