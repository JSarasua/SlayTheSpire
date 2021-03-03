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

	bool EndTurn( EventArgs const& args );
	bool PlayCard( EventArgs const& args );

private:
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
	WidgetSlider* m_playerHealthWidget = nullptr;
	WidgetSlider* m_enemyHealthWidget = nullptr;

	bool m_isUIDirty = false;

public:
	Rgba8 m_clearColor = Rgba8::BLACK;
	RandomNumberGenerator m_rand;

private:
};