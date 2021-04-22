#pragma once
#include <array>

class Texture;
class RandomNumberGenerator;

enum eReward
{
	INVALID_REWARD = -1,
	ADD_CARD,
	//REMOVE_CARD,
	//UPGRADE_CARD,
	NUM_REWARDS
};


class RewardDefinition
{
private:
	RewardDefinition( eReward reward, Texture const* texture );
	static std::array<RewardDefinition, (size_t)eReward::NUM_REWARDS> s_rewardDefs;

public:
	RewardDefinition() = default;
	~RewardDefinition() {}
	static void InitializeRewardDefinitions();
	static RewardDefinition const& GetRewardDefinitionByType( eReward reward );
	static RewardDefinition const& GetRandomRewardDefinition( RandomNumberGenerator& rng );

	Texture const* GetTexture() const { return m_texture; }
	eReward GetReward() const { return m_reward; }

private:
	Texture const* m_texture = nullptr;
	eReward m_reward = ADD_CARD;
};