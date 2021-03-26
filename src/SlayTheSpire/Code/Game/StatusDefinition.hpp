#pragma once
#include <array>

enum eStatus
{
	INVALID_STATUS = 0,
	Vulnerable,
	Weak,
	Ritual,
	NUM_STATUS
};

class Texture;

class StatusDefinition
{
public:
	StatusDefinition() = default;
	static StatusDefinition const& GetStatusDefinitionByType( eStatus statusDefinition );
	static void InitializeStatusDefinitions();

private:
	StatusDefinition( eStatus statusType, Texture const* statusTexture, int strengthPerTurn, float damageToOpponentModifier, float damageTakenModifier );


	static std::array<StatusDefinition, (size_t)eStatus::NUM_STATUS> s_statusDefs;

public:
	eStatus m_statusType = INVALID_STATUS;
	Texture const* m_statusTexture = nullptr;
	int m_strengthPerTurn = 0;
	float m_damageToOpponentModifier = 1.f;
	float m_damageTakenModifier = 1.f;
};