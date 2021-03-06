#pragma once
#include <array>
#include <vector>

enum eEnemy
{
	Cultist,
	NUM_ENEMIES
};

enum eMoveType
{
	Attack,
	Buff,
	Curse
};

class StatusDefinition;

struct EnemyMove
{
public:
	EnemyMove( eMoveType move, int damage, int block, int strength, bool giveVulnerable, bool giveWeak, StatusDefinition const* statusDef );

	eMoveType m_moveType = Attack;
	int m_damage = 0;
	int m_block = 0;
	int m_strength = 0;
	bool m_giveVulnerable = false;
	bool m_giveWeak = false;
	StatusDefinition const* m_statusDef;
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

	EnemyMove GetRandomMove( RandomNumberGenerator& rng, int moveTurn );
	int GetMaxHealth() const { return m_maxHealth; }
	Texture const* GetTexture() { return m_enemyTexture; }
private:
	std::vector<EnemyMove> m_orderedMoves;
	std::vector<EnemyMove> m_moves;
	int m_maxHealth = 80;
	Texture const* m_enemyTexture = nullptr;
};