#include "Game/RewardDefinition.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"


std::array<RewardDefinition, (size_t)eReward::NUM_REWARDS> RewardDefinition::s_rewardDefs;

RewardDefinition::RewardDefinition( eReward reward, Texture const* texture ) :
	m_reward( reward ),
	m_texture( texture )
{}

void RewardDefinition::InitializeRewardDefinitions()
{
	Texture const* plusTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/plusIcon.png" );
	Texture const* minusTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/minusIcon.png" );

	s_rewardDefs[ADD_CARD] = RewardDefinition( ADD_CARD, plusTexture );

}

RewardDefinition const& RewardDefinition::GetRewardDefinitionByType( eReward reward )
{
	return s_rewardDefs[reward];
}

RewardDefinition const& RewardDefinition::GetRandomRewardDefinition( RandomNumberGenerator& rng )
{
	int rewardIndex = rng.RollRandomIntLessThan( NUM_REWARDS );

	return s_rewardDefs[rewardIndex];
}

