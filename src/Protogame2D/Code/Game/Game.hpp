#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>


class Clock;
class Widget;
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

private:
	void StartupCardGame();
	void StartupUI();
	void CheckCollisions();
	void UpdateEntities( float deltaSeconds );
	void UpdateCamera( float deltaSeconds );
	void RenderGame();
	void RenderUI();
	void CheckButtonPresses(float deltaSeconds);

private:
	UIManager* m_UIManager = nullptr;
	Clock* m_gameClock = nullptr;
	Camera m_UICamera;
	Camera m_camera;
	Vec2 m_cameraPosition;
	Texture* m_screenTexture = nullptr;

	GameState* m_currentGamestate = nullptr;

public:
	Rgba8 m_clearColor = Rgba8::BLACK;
	RandomNumberGenerator m_rand;

private:
};
