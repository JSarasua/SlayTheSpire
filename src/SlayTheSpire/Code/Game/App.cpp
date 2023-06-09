#include"Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameCommon.hpp"


//InputSystem* g_theInput = nullptr;

const char* APP_NAME = "Slay the Spire";	// ...becomes ??? (Change this per project!)

App::App()
{
	g_currentBases = eYawPitchRollRotationOrder::YXZ;
	g_gameConfigBlackboard = new NamedStrings();
	g_theAudio = new AudioSystem();
	g_theInput = new InputSystem();
	g_theGame = new Game();
	g_theConsole = new DevConsole();
	g_theEventSystem = new EventSystem();
}

App::~App() {}

void App::Startup()
{

	Clock::SystemStartup();

	XmlDocument gameConfigDoc;
	const XmlElement& gameConfigRootElement = GetRootElement( gameConfigDoc, "Data/GameConfig.xml" );
	g_gameConfigBlackboard->PopulateFromXmlElementAttributes( gameConfigRootElement );

	float aspectRatio = g_gameConfigBlackboard->GetValue("windowAspect", 0.f);
	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME, aspectRatio, 0.90f );
	g_theWindow->SetInputSystem(g_theInput);
	g_theWindow->SetEventSystem(g_theEventSystem);
	
	g_theInput->Startup( g_theWindow );
	g_theInput->PushMouseOptions( MOUSE_MODE_ABSOLUTE, true, false );

	g_theRenderer = new RenderContext();
	g_theRenderer->StartUp(g_theWindow);

	DebugRenderSystemStartup( g_theRenderer );
	
	g_theGame->Startup();
	g_theConsole->Startup();

	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget( g_theRenderer->GetBackBuffer() );
	//m_devConsoleCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y));
	m_devConsoleCamera->SetProjectionOrthographic(Vec2(GAME_CAMERA_Y* CLIENT_ASPECT, GAME_CAMERA_Y), 0.f, -100.f );
	m_devConsoleCamera->m_cameraType = SCREENCAMERA;
	g_theRenderer->CreateOrGetBitmapFont( "Fonts/SquirrelFixedFont.png" );


	g_theEventSystem->SubscribeMethodToEvent("quit", CONSOLECOMMAND, this, &App::QuitRequested );

}

void App::Shutdown()
{
	delete m_devConsoleCamera;


	delete g_theAudio;
	delete g_gameConfigBlackboard;
	g_theGame->Shutdown();
	delete g_theGame;
	g_theConsole->Shutdown();
	delete g_theConsole;
	DebugRenderSystemShutdown();
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theInput->Shutdown();
	delete g_theInput;
}


void App::RunFrame()
{
	BeginFrame(); //For all engine systems (Not the game)
	Update();
	Render();
	EndFrame(); //For all engine systems (Not the game)
}



bool App::HandleKeyPressed( unsigned char keyCode )
{
	UNUSED(keyCode);
	return true;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	UNUSED(keyCode);
	return true;
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}

bool App::IsUpArrowPressed()
{
	return m_isUpArrowPressed;
}

bool App::IsLeftArrowPressed()
{
	return m_isLeftArrowPressed;
}

bool App::IsRightArrowPressed()
{
	return m_isRightArrowPressed;
}

bool App::IsSlowed()
{
	return m_isSlowed;
}

bool App::IsNoClipping()
{
	return m_noClip;
}

void App::BeginFrame()
{
	Clock::BeginFrame();
	g_theAudio->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theInput->BeginFrame();
	g_theConsole->BeginFrame();
}

void App::Update()
{
	g_theRenderer->UpdateFrameTime();
	g_theConsole->Update();

	CheckButtonPresses();
	CheckController();

	g_theGame->Update();
}


void App::Render()
{
	g_theGame->Render();

	g_theRenderer->BeginCamera(*m_devConsoleCamera);
	g_theConsole->Render(*g_theRenderer, *m_devConsoleCamera, 0.1f);
	g_theRenderer->EndCamera(*m_devConsoleCamera);
}
void App::EndFrame()
{
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	g_theConsole->EndFrame();
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
}

void App::RestartGame()
{
	g_theGame->Shutdown();
	delete g_theGame;
	g_theGame = new Game();
	g_theGame->Startup();
}

bool App::GetDebugGameMode()
{
	return m_debugGameMode;
}

bool App::GetDebugCameraMode()
{
	return m_debugCameraMode;
}

bool App::QuitRequested( const EventArgs& args )
{
	UNUSED( args );
	g_theApp->HandleQuitRequested();
	return true;
}

void App::CheckButtonPresses()
{
	const KeyButtonState& tildeKey = g_theInput->GetKeyStates( 0xC0 );	//tilde: ~
	if( tildeKey.WasJustPressed() )
	{
		g_theConsole->SetIsOpen( !g_theConsole->IsOpen() );
	}


	if( g_theInput->GetKeyStates( 0x1B ).IsPressed() ) //ESC
	{
		g_theEventSystem->FireEvent("quit", CONSOLECOMMAND, nullptr);
		//HandleQuitRequested();
	}

	if( g_theInput->GetKeyStates( 'P' ).WasJustPressed() )
	{
		m_isPaused = !m_isPaused;
	}

	if( g_theInput->GetKeyStates( 'T' ).IsPressed() )
	{
		m_isSlowed = true;
	}
	else if( g_theInput->GetKeyStates( 'T' ).WasJustReleased() )
	{
		m_isSlowed = false;
	}

	if( g_theInput->GetKeyStates( 'Y' ).IsPressed() )
	{
		m_isSpedUp = true;
	}
	else if( g_theInput->GetKeyStates( 'Y' ).WasJustReleased() )
	{
		m_isSpedUp = false;
	}
}

void App::CheckController()
{
	const XboxController& controller = g_theInput->GetXboxController( 0 );
	if( controller.GetButtonState( XBOX_BUTTON_ID_START ).WasJustPressed() )
	{
		g_theApp->RestartGame();
	}

	if( controller.GetButtonState( XBOX_BUTTON_ID_BACK ).WasJustPressed() )
	{
		m_debugGameMode = !m_debugGameMode;
	}
}
