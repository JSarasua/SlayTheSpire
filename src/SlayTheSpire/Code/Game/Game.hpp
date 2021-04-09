#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>


class Clock;
class Widget;
class WidgetSlider;
class UIManager;
class GameState;

class Game
{
public:
	Game();
	~Game();
	void Startup();
	void Shutdown();
	void RunFrame();

	void Update();
	void Render();

	bool RestartGame( EventArgs const& args );
	bool StartPlayerEndTurn( EventArgs const& args );
	bool EndPlayerEndTurn( EventArgs const& args );
	bool StartEnemyTurn( EventArgs const& args );
	bool EnemyDealDamage( EventArgs const& args );
	bool EndEnemyTurn( EventArgs const& args );
	bool StartStartPlayerTurn( EventArgs const& args );
	bool EndStartPlayerTurn( EventArgs const& args );
	bool EndTurn( EventArgs const& args );
	bool StartPlayCard( EventArgs const& args );
	bool EndPlayCard( EventArgs const& args );
	bool FightOver( EventArgs const& args );
	bool UpdateTargeting( EventArgs const& args );
	bool ReleaseTargeting( EventArgs const& args );

private:
	void InitializeDefinitions();
	void UpdateUI();
	void MatchUIToGameState();

	void StartupCardGame();
	void StartupUI();
	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);
	std::vector<Transform> GetGoalHandTransforms( int handCount );

	void DoEnemyTurn();

	void CreateTargetingWidgets( Vec2 const& startPos, Vec2 const& endPos, Vec2 const& startTangent, int countOfWidgets );
	void UpdateTargetingWidgets( Vec2 const& endPos );
	void ClearTargetingWidgets();


private:
	Clock* m_gameClock = nullptr;
	Camera m_UICamera;
	Camera m_camera;
	Vec2 m_cameraPosition;
	Texture* m_screenTexture = nullptr;
	Texture const* m_cyanTexture = nullptr;
	Texture const* m_redTexture = nullptr;
	Texture const* m_greenTexture = nullptr;

	GameState* m_currentGamestate = nullptr;

	Widget* m_deckWidget = nullptr;
	Widget* m_discardPileWidget = nullptr;
	Widget* m_baseCardWidget = nullptr;
	Widget* m_handWidget = nullptr;
	Widget* m_energyWidget = nullptr;
	Widget* m_endTurnWidget = nullptr;

	Widget* m_endFightWidget = nullptr;

	bool m_isUIDirty = false;

	std::vector<Widget*> m_targetBodyWidgets;
	Widget* m_targetHeadWidget = nullptr;
	Vec2 m_startOfTargetChain;
	Vec2 m_startOfTargetChainTangent;
	Vec2 m_endOfTargetChain;
	bool m_isTargeting = false;

public:
	Rgba8 m_clearColor = Rgba8::BLACK;
	RandomNumberGenerator m_rand;

private:
};
