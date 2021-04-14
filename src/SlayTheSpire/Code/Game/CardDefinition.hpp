#pragma once
#include <array>
class Texture;

enum eCard
{
	Strike,
	//StrikePlus,
	Defend,
	//DefendPlus,
	SpotWeakness,
	//SpotWeaknessPlus,
	NUM_CARDS
};

class CardDefinition
{
private:
	CardDefinition( eCard cardType, int costToPlay, int attack, int block, Texture const* texture );
public:
	CardDefinition() = default;
	~CardDefinition() {}

	static void InitializeCardDefinitions();
	static std::array<CardDefinition,(size_t)eCard::NUM_CARDS> s_cardDefs;
	static CardDefinition const& GetCardDefinitionByType( eCard cardType );
	static CardDefinition const& GetRandomCardDefinition();
	static eCard GetRandomCardType();

	eCard GetCardType() const { return m_cardType; }
	int GetCost() const { return m_costToPlay; }
	int GetAttack() const { return m_attack; }
	int GetBlock() const { return m_block; }
	bool GetIsSpotWeakness() const { return m_isSpotWeakness; }
	Texture const* GetCardTexture() const { return m_texture; }
private:
	eCard m_cardType;
	int m_costToPlay = 0;
	int m_attack = 0;
	int m_block = 0;

	bool m_isSpotWeakness = false;

	Texture const* m_texture = nullptr;
};