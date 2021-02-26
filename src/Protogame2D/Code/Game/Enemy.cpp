#include "Game/Enemy.hpp"
#include "Engine/Math/MathUtils.hpp"

void Enemy::TakeDamage( int damage )
{
	m_enemyHealth -= damage;
	m_enemyHealth = MaxInt( m_enemyHealth, 0 );
}

