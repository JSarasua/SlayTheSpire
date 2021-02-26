#pragma once
#include <array>
class Texture;

enum eCard
{
	Strike,
	Defend,
	NUM_CARDS
};

class CardDefinition
{
private:
	CardDefinition( int costToPlay, int attack, int block, Texture const* texture );
public:
	CardDefinition() = default;
	~CardDefinition() {}

	static void InitializeCardDefinitions();
	static std::array<CardDefinition,(size_t)eCard::NUM_CARDS> s_cardDefs;
	static CardDefinition const& GetCardDefinitionByType( eCard cardType );

	int GetCost() const { return m_costToPlay; }
	int GetAttack() const { return m_attack; }
	int GetBlock() const { return m_block; }
	Texture const* GetCardTexture() const { return m_texture; }
private:
	int m_costToPlay = 0;
	int m_attack = 0;
	int m_block = 0;

	Texture const* m_texture = nullptr;
};