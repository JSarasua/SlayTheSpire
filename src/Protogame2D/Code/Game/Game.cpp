#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/AABB2.hpp"
#include "App.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/World.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/UI/Widget.hpp"

extern App* g_theApp;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


Game::Game()
{
	//m_world = new World(this);
}

Game::~Game(){}

void Game::Startup()
{
	EnableDebugRendering();
	m_camera = Camera();
	m_camera.SetColorTarget(nullptr); // we use this
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	//m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_camera.SetProjectionOrthographic( m_camera.m_outputSize, 0.f, 100.f );

	m_UICamera = Camera();
	m_UICamera.SetColorTarget( nullptr ); // we use this
	m_UICamera.CreateMatchingDepthStencilTarget( g_theRenderer );
	m_UICamera.SetOutputSize( Vec2( 16.f, 9.f ) );
	//m_camera.SetProjectionPerspective( 60.f, -0.09f, -100.f );
	m_UICamera.SetProjectionOrthographic( m_UICamera.m_outputSize, 0.f, 100.f );

	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );
	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );

	m_world = new World( this );
	m_world->Startup();

	StartupUI();
}

void Game::Shutdown()
{
	delete m_rootWidget;
	m_rootWidget = nullptr;

	delete m_UIMesh;
	m_UIMesh = nullptr;

	m_world->Shutdown();
	delete m_world;
	m_world = nullptr;
}

void Game::RunFrame(){}

void Game::Update()
{
	float dt = (float)m_gameClock->GetLastDeltaSeconds();

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}
	Vec2 mousePos = g_theInput->GetMouseNormalizedPos();

	AABB2 screenBounds = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	mousePos = screenBounds.GetPointAtUV( mousePos );

	m_rootWidget->UpdateHovered( mousePos );
	m_rootWidget->CheckInput();
	m_rootWidget->UpdateDrag();
	m_world->Update( dt );
	UpdateCamera( dt );
}

void Game::Render()
{
	Texture* backbuffer = g_theRenderer->GetBackBuffer();
	Texture* colorTarget = g_theRenderer->AcquireRenderTargetMatching( backbuffer );
	m_camera.SetColorTarget( 0, colorTarget );
	m_UICamera.SetColorTarget( 0, colorTarget );

	g_theRenderer->BeginCamera( m_camera );
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindShader( (Shader*)nullptr );

	RenderGame();
	g_theRenderer->EndCamera( m_camera );

	RenderUI();

	g_theRenderer->CopyTexture( backbuffer, colorTarget );
	m_camera.SetColorTarget( nullptr );
	g_theRenderer->ReleaseRenderTarget( colorTarget );
	GUARANTEE_OR_DIE( g_theRenderer->GetTotalRenderTargetPoolSize() < 8, "Created too many render targets" );

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

void Game::StartupUI()
{
	m_UIMesh = new GPUMesh( g_theRenderer );
	std::vector<Vertex_PCU> uiVerts;
	std::vector<uint> uiIndices;
	AABB2 uiBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
	AABB2 defaultUVs = AABB2( Vec2(), Vec2( 1.f, 1.f ) );
	Vertex_PCU::AppendIndexedVertsAABB2D( uiVerts, uiIndices, uiBounds, Rgba8::WHITE, defaultUVs );
	m_UIMesh->UpdateVertices( uiVerts );
	m_UIMesh->UpdateIndices( uiIndices );

	AABB2 screenBounds = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
	std::string testEvent = "help";
	m_rootWidget = new Widget( m_UIMesh, screenBounds );

	Texture* strikeTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Strike_r.png" );
	Texture* highlightTexture = g_theRenderer->CreateTextureFromColor( Rgba8::CYAN );
	Texture* selectTexture = g_theRenderer->CreateTextureFromColor( Rgba8::RED );

	Vec3 scale = Vec3( 2.f, 2.5f, 1.f );
	Transform card1Transform = Transform();
	card1Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.35f, 0.2f ) );
	card1Transform.m_scale = scale;
	card1Transform.m_rotationPitchRollYawDegrees.y = 15.f;
	Widget* card1Widget = new Widget( m_UIMesh, card1Transform );
	card1Widget->SetTexture( strikeTexture, highlightTexture, selectTexture );
	card1Widget->SetEventToFire( testEvent );
	card1Widget->SetCanDrag( true );
	m_rootWidget->AddChild( card1Widget );

	Transform card2Transform = Transform();
	card2Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.425f, 0.245f ) );
	card2Transform.m_scale = scale;
	card2Transform.m_rotationPitchRollYawDegrees.y = 7.5f;
	Widget* card2Widget = new Widget( m_UIMesh, card2Transform );
	card2Widget->SetTexture( strikeTexture, highlightTexture, selectTexture );
	card2Widget->SetEventToFire( testEvent );
	m_rootWidget->AddChild( card2Widget );
	
	Transform card3Transform = Transform();
	card3Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.25f ) );
	card3Transform.m_scale = scale;
	Widget* card3Widget = new Widget( m_UIMesh, card3Transform );
	card3Widget->SetTexture( strikeTexture, highlightTexture, selectTexture );
	card3Widget->SetEventToFire( testEvent );
	m_rootWidget->AddChild( card3Widget );

	Transform card4Transform = Transform();
	card4Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.575f, 0.245f ) );
	card4Transform.m_scale = scale;
	card4Transform.m_rotationPitchRollYawDegrees.y = -7.5f;
	Widget* card4Widget = new Widget( m_UIMesh, card4Transform );
	card4Widget->SetTexture( strikeTexture, highlightTexture, selectTexture );
	card4Widget->SetEventToFire( testEvent );
	m_rootWidget->AddChild( card4Widget );

	Transform card5Transform = Transform();
	card5Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.65f, 0.2f ) );
	card5Transform.m_scale = scale;
	card5Transform.m_rotationPitchRollYawDegrees.y = -15.f;
	Widget* card5Widget = new Widget( m_UIMesh, card5Transform );
	card5Widget->SetTexture( strikeTexture, highlightTexture, selectTexture );
	card5Widget->SetEventToFire( testEvent );
	m_rootWidget->AddChild( card5Widget );
}

void Game::CheckCollisions()
{}

void Game::UpdateEntities( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	Rgba8 clearColor = Rgba8::BLACK;
	m_camera.SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, clearColor, 0.f, 0 );

	//Clamp camera to map bounds centered on player
	Vec2 playerCenter = m_world->GetPlayer()->GetPosition();
	Vec2 cameraOutputSize = m_camera.GetOutputSize();
	Vec2 cameraHalfSize = 0.5f * cameraOutputSize;
	IntVec2 currentMapBounds = GetCurrentMapBounds();
	Vec2 currentMapBoundsVec2 = (Vec2)currentMapBounds;
	Vec2 mapBoundsMax = currentMapBoundsVec2 - cameraHalfSize;
	Vec2 mapBoundsMin = cameraHalfSize;
	AABB2 cameraBoundingBox = AABB2( mapBoundsMin, mapBoundsMax );
	Vec2 newCameraCenter = cameraBoundingBox.GetNearestPoint( playerCenter );
	
	m_camera.SetPosition( newCameraCenter );
}

void Game::RenderGame()
{
	//g_theRenderer->BeginCamera( m_camera );
	m_world->Render();
	//m_rootWidget->Render();
	//g_theRenderer->EndCamera( m_camera );
}

void Game::RenderUI()
{
	Player* player = m_world->GetPlayer();
	int playerHealth = player->GetCurrentHealth();
	std::string playerHealthStr = Stringf( "Player Health: %i", playerHealth );
	DebugAddScreenText( Vec4( 0.01f, 0.95f, 0.f, 0.f ), Vec2( 0.f, 0.f ), 20.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, playerHealthStr.c_str() );

	g_theRenderer->BeginCamera( m_UICamera );
	g_theRenderer->SetModelMatrix( Mat44() );
	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
	g_theRenderer->SetDepth( eDepthCompareMode::COMPARE_ALWAYS, eDepthWriteMode::WRITE_ALL );

	m_rootWidget->Render();

// 	Rgba8 backgroundTint = Rgba8( 128, 128, 128, 128 );
// 	AABB2 gameCamera = AABB2( m_UICamera.GetOrthoBottomLeft(), m_UICamera.GetOrthoTopRight() );
// 	g_theRenderer->BindTexture( nullptr );
// 	g_theRenderer->SetBlendMode( eBlendMode::ALPHA );
// 	g_theRenderer->DrawAABB2Filled( gameCamera, backgroundTint );
	g_theRenderer->EndCamera( m_UICamera );
	//m_rootWidget->Render();
}

void Game::CheckButtonPresses(float deltaSeconds)
{
	const XboxController& controller = g_theInput->GetXboxController(0);
	UNUSED( deltaSeconds );
	UNUSED( controller );

	const KeyButtonState& wKey = g_theInput->GetKeyStates( 'W' );
	const KeyButtonState& aKey = g_theInput->GetKeyStates( 'A' );
	const KeyButtonState& sKey = g_theInput->GetKeyStates( 'S' );
	const KeyButtonState& dKey = g_theInput->GetKeyStates( 'D' );
	//const KeyButtonState& spaceKey = g_theInput->GetKeyStates( SPACE_KEY );
	const KeyButtonState& shiftKey = g_theInput->GetKeyStates( SHIFT_KEY );
	const KeyButtonState& f1Key = g_theInput->GetKeyStates( F1_KEY );
	const KeyButtonState& f5Key = g_theInput->GetKeyStates( F5_KEY );
	const KeyButtonState& f6Key = g_theInput->GetKeyStates( F6_KEY );
	const KeyButtonState& f7Key = g_theInput->GetKeyStates( F7_KEY );
	const KeyButtonState& f8Key = g_theInput->GetKeyStates( F8_KEY );
	const KeyButtonState& f11Key = g_theInput->GetKeyStates( F11_KEY );
	const KeyButtonState& num1Key = g_theInput->GetKeyStates( '1' );
	const KeyButtonState& num2Key = g_theInput->GetKeyStates( '2' );
	const KeyButtonState& num3Key = g_theInput->GetKeyStates( '3' );
	const KeyButtonState& num4Key = g_theInput->GetKeyStates( '4' );
	const KeyButtonState& num5Key = g_theInput->GetKeyStates( '5' );
	const KeyButtonState& num6Key = g_theInput->GetKeyStates( '6' );
	const KeyButtonState& num7Key = g_theInput->GetKeyStates( '7' );
	const KeyButtonState& num8Key = g_theInput->GetKeyStates( '8' );
	const KeyButtonState& num9Key = g_theInput->GetKeyStates( '9' );
	const KeyButtonState& num0Key = g_theInput->GetKeyStates( '0' );
	const KeyButtonState& lBracketKey = g_theInput->GetKeyStates( LBRACKET_KEY );
	const KeyButtonState& rBracketKey = g_theInput->GetKeyStates( RBRACKET_KEY );
	const KeyButtonState& rKey = g_theInput->GetKeyStates( 'R' );
	const KeyButtonState& fKey = g_theInput->GetKeyStates( 'F' );
	const KeyButtonState& tKey = g_theInput->GetKeyStates( 'T' );
	const KeyButtonState& gKey = g_theInput->GetKeyStates( 'G' );
	const KeyButtonState& hKey = g_theInput->GetKeyStates( 'H' );
	//const KeyButtonState& yKey = g_theInput->GetKeyStates( 'Y' );
	const KeyButtonState& qKey = g_theInput->GetKeyStates( 'Q' );
	const KeyButtonState& vKey = g_theInput->GetKeyStates( 'V' );
	const KeyButtonState& bKey = g_theInput->GetKeyStates( 'B' );
	const KeyButtonState& nKey = g_theInput->GetKeyStates( 'N' );
	const KeyButtonState& mKey = g_theInput->GetKeyStates( 'M' );
	const KeyButtonState& uKey = g_theInput->GetKeyStates( 'U' );
	const KeyButtonState& iKey = g_theInput->GetKeyStates( 'I' );
	const KeyButtonState& jKey = g_theInput->GetKeyStates( 'J' );
	const KeyButtonState& kKey = g_theInput->GetKeyStates( 'K' );
	const KeyButtonState& zKey = g_theInput->GetKeyStates( 'Z' );
	const KeyButtonState& xKey = g_theInput->GetKeyStates( 'X' );
	const KeyButtonState& plusKey = g_theInput->GetKeyStates( PLUS_KEY );
	const KeyButtonState& minusKey = g_theInput->GetKeyStates( MINUS_KEY );
	const KeyButtonState& semiColonKey = g_theInput->GetKeyStates( SEMICOLON_KEY );
	const KeyButtonState& singleQuoteKey = g_theInput->GetKeyStates( SINGLEQUOTE_KEY );
	const KeyButtonState& commaKey = g_theInput->GetKeyStates( COMMA_KEY );
	const KeyButtonState& periodKey = g_theInput->GetKeyStates( PERIOD_KEY );

	if( jKey.WasJustPressed() )
	{

	}
	if( kKey.WasJustPressed() )
	{

	}
	if( zKey.WasJustPressed() )
	{

	}
	if( xKey.WasJustPressed() )
	{

	}
	if( uKey.IsPressed() )
	{

	}
	if( iKey.IsPressed() )
	{

	}
	if( qKey.WasJustPressed() )
	{

	}

	if( f1Key.WasJustPressed() )
	{
		SoundID soundID = g_theAudio->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
		float volume = m_rand.RollRandomFloatInRange( 0.5f, 1.f );
		float balance = m_rand.RollRandomFloatInRange( -1.f, 1.f );
		float speed = m_rand.RollRandomFloatInRange( 0.5f, 2.f );

		g_theAudio->PlayGameSound( soundID, false, volume, balance, speed );
	}

	if( f11Key.WasJustPressed() )
	{
		g_theWindow->ToggleBorder();
	}

	if( num1Key.IsPressed() )
	{

	}
	if( num2Key.IsPressed() )
	{

	}
	if( num3Key.IsPressed() )
	{

	}
	if( num4Key.IsPressed() )
	{

	}
	if( num5Key.IsPressed() )
	{

	}
	if( num6Key.IsPressed() )
	{

	}
	if( num7Key.IsPressed() )
	{

	}
	if( num8Key.IsPressed() )
	{

	}
	if( num9Key.IsPressed() )
	{

	}
	if( num0Key.IsPressed() )
	{

	}
	if( lBracketKey.IsPressed() )
	{

	}
	if( rBracketKey.IsPressed() )
	{

	}
	if( semiColonKey.IsPressed() )
	{

	}
	if( singleQuoteKey.IsPressed() )
	{

	}
	if( commaKey.WasJustPressed() )
	{

	}
	if( periodKey.WasJustPressed() )
	{

	}
	if( vKey.WasJustPressed() )
	{

	}
	if( bKey.WasJustPressed() )
	{

	}
	if( nKey.WasJustPressed() )
	{

	}
	if( mKey.WasJustPressed() )
	{

	}
	if( gKey.IsPressed() )
	{
		float currentGamma = g_theRenderer->GetGamma();
		float newGamma = currentGamma - 2.f * deltaSeconds;
		g_theRenderer->SetGamma( newGamma );
	}
	if( hKey.IsPressed() )
	{
		float currentGamma = g_theRenderer->GetGamma();
		float newGamma = currentGamma + 2.f * deltaSeconds;
		g_theRenderer->SetGamma( newGamma );
	}
	if( f5Key.WasJustPressed() )
	{

	}
	if( f6Key.WasJustPressed() )
	{

	}
	if( f7Key.WasJustPressed() )
	{

	}
	if( f8Key.WasJustPressed() )
	{

	}

	if( rKey.WasJustPressed() )
	{
		Vec3 startPos = m_camera.GetPosition();
		Vec3 endPos = Vec3( 0.f, 0.f, -5.f );
		Mat44 cameraModel = m_camera.GetModelRotationMatrix();

		endPos = cameraModel.TransformPosition3D( endPos );
		endPos += startPos;
		LineSegment3 line = LineSegment3( startPos, endPos );
		DebugAddWorldLine( line, Rgba8::GREEN, Rgba8::BLUE, 10.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( fKey.WasJustPressed() )
	{
		Vec3 startPos = m_camera.GetPosition();
		Vec3 endPos = Vec3( 0.f, 0.f, -5.f );
		Mat44 cameraModel = m_camera.GetModelRotationMatrix();

		endPos = cameraModel.TransformPosition3D( endPos );
		endPos += startPos;
		LineSegment3 line = LineSegment3( startPos, endPos );
		DebugAddWorldArrow( line, Rgba8::GREEN, Rgba8::BLUE, 10.f, DEBUG_RENDER_USE_DEPTH );
	}
	if( tKey.WasJustPressed() )
	{

	}
	if( plusKey.IsPressed() )
	{

	}
	if( minusKey.IsPressed() )
	{

	}

	Vec3 translator;

	if( wKey.IsPressed() )
	{
		//translator.y +=  1.f * deltaSeconds;
	}
	if( sKey.IsPressed() )
	{
		//translator.y -=  1.f * deltaSeconds;
	}
	if( aKey.IsPressed() )
	{
		//translator.x -=  1.f * deltaSeconds;
	}
	if( dKey.IsPressed() )
	{
		//translator.x +=  1.f * deltaSeconds;
	}

	if( shiftKey.IsPressed() )
	{
		//translator *= 2.f;
	}

	//m_camera.TranslateRelativeToViewOnlyYaw( translator );
}

IntVec2 Game::GetCurrentMapBounds() const
{
	return m_world->getCurrentMapBounds();
}
