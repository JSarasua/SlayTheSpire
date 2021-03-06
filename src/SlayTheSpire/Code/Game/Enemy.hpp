#pragma once
#include "Game/Entity.hpp"



class EnemyDefinition;

class Enemy : public Entity
{
public:
	Enemy();

public:
	EnemyDefinition const* m_enemyDef = nullptr;
};