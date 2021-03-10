#pragma once
#include <array>

enum eMoveType
{
	INVALID_MOVETYPE = -1,
	Attack,
	Buff,
	Curse,
	NUM_MOVETYPES
};

class Texture;

class MoveTypeDefinition
{
private:
	MoveTypeDefinition( eMoveType moveType, Texture const* moveTypeTexture = nullptr );
public:
	MoveTypeDefinition() {}
	static std::array<MoveTypeDefinition, NUM_MOVETYPES> s_moveTypeDefs;
	static void InitializeMoveTypeDefs();
	static MoveTypeDefinition const& GetMoveTypeDefinitionByType( eMoveType moveType );

public:
	eMoveType m_moveType = INVALID_MOVETYPE;
	Texture const* m_moveTypeTexture = nullptr;

};