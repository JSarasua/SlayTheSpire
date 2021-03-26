#pragma once
#include <array>
#include <vector>
#include "Game/MoveTypeDefinition.hpp"

enum eEnemy
{
	Cultist,
	NUM_ENEMIES
};



class StatusDefinition;

struct EnemyMove
{
public:
	EnemyMove() {}
	EnemyMove( eMoveType move, int damage, int block, int strength, bool giveVulnerable, bool giveWeak, StatusDefinition const* statusDef );

	eMoveType m_moveType = Attack;
	int m_damage = 0;
	int m_block = 0;
	int m_strength = 0;
	bool m_giveVulnerable = false;
	bool m_giveWeak = false;
	StatusDefinition const* m_statusDef = nullptr;
};

class RandomNumberGenerator;
class Texture;

class EnemyDefinition
{
private:
	EnemyDefinition( std::vector<EnemyMove> orderedMoves, std::vector<EnemyMove> moves, int maxHealth, Texture const* texture );
	static std::array<EnemyDefinition, (size_t)eEnemy::NUM_ENEMIES> s_enemyDefs;
public:
	EnemyDefinition() = default;
	~EnemyDefinition() {}
	static void InitializeEnemyDefinitions();
	static EnemyDefinition const& GetEnemyDefinitionByType( eEnemy enemyType ) { return s_enemyDefs[enemyType]; }

	EnemyMove GetNextMove( RandomNumberGenerator& rng, int moveTurn ) const;
	EnemyMove GetRandomMove( RandomNumberGenerator& rng ) const;
	int GetMaxHealth() const { return m_maxHealth; }
	Texture const* GetTexture() const { return m_enemyTexture; }
private:
	std::vector<EnemyMove> m_orderedMoves;
	std::vector<EnemyMove> m_moves;
	int m_maxHealth = 80;
	Texture const* m_enemyTexture = nullptr;
};