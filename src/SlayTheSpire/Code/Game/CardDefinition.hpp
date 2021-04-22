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
	ShrugItOff,
	Bludgeon,
	Impervious,
 	Feed,
 	Disarm,
// 	Anger,
 	IronWave,
 	LimitBreak,
	HeavyBlade,
	Hemokinesis,
	PommelStrike,
	SeeingRed,
	PerfectedStrike,
	WildStrike,
	Bloodletting,
	Wound,
	NUM_CARDS
};

class CardDefinition
{
private:
	CardDefinition( eCard cardType, int costToPlay, int attack, int block, Texture const* texture, 
		bool isSpotWeakness = false, bool isExhaust = false, int cardDraw = 0, int maxHealthIncrease = 0, 
		int enemyStrengthModifier = 0, int strengthPerTurn = 0, bool isLimitBreak = false );
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

	Texture const* m_texture = nullptr;
public:
	bool m_isSpotWeakness = false;
	bool m_isExhaust = false;
	int m_cardDraw = 0;
	int m_maxHealthIncrease = 0;
	int m_enemyStrengthModifier = 0;
	int m_strengthPerTurn = 0;
	bool m_isLimitBreak = false;
	int m_healthIncrease = 0;
	bool m_isPerfectedStrike = false;
	int m_woundsToAdd = 0;
	int m_strengthMultiplier = 1;
	int m_energyGain = 0;
};