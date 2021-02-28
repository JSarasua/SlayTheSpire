#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState.hpp"
#include "Game/PlayerBoard.hpp"
#include "Game/Enemy.hpp"
#include "Game/CardDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/UI/WidgetSlider.hpp"
#include "Engine/UI/UIManager.hpp"
#include "Engine/Math/AABB2.hpp"


Game::Game()
{}

Game::~Game(){}

void Game::Startup()
{
	EnableDebugRendering();
 	//m_camera = Camera();
	m_camera.CreateMatchingDepthStencilTarget( g_theRenderer );
	//m_camera.SetOutputSize( Vec2( 16.f, 9.f ) );
	//m_camera.SetProjectionOrthographic( m_camera.m_outputSize, 0.f, 100.f );

	m_gameClock = new Clock();
	m_gameClock->SetParent( Clock::GetMaster() );

	g_theRenderer->Setup( m_gameClock );
	m_screenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::BLACK, IntVec2(1920,1080) );

	StartupCardGame();
	StartupUI();

	g_theEventSystem->SubscribeMethodToEvent( "endTurn", NOCONSOLECOMMAND, this, &Game::EndTurn );
}

void Game::Shutdown()
{
	if( m_UIManager )
	{
		delete m_UIManager;
		m_UIManager = nullptr;
	}

	if( m_gameClock )
	{
		delete m_gameClock;
		m_gameClock = nullptr;
	}

	if( m_currentGamestate )
	{
		delete m_currentGamestate;
		m_currentGamestate = nullptr;
	}

	if( m_baseCardWidget )
	{
		delete m_baseCardWidget;
		m_baseCardWidget = nullptr;
	}
}

void Game::RunFrame(){}

void Game::Update()
{
	float dt = (float)m_gameClock->GetLastDeltaSeconds();

	if( !g_theConsole->IsOpen() )
	{
		CheckButtonPresses( dt );
	}

	UpdateUI();
	m_UIManager->Update( dt );

	//UpdateCamera( dt );
}

void Game::Render()
{
	RenderUI();

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

bool Game::EndTurn( EventArgs const& args )
{
	UNUSED( args );

	PlayerBoard& playerBoard = m_currentGamestate->m_playerBoard;
	playerBoard.DiscardHand();
	playerBoard.DrawHand();
	playerBoard.m_playerEnergy = playerBoard.m_playerMaxEnergy;

	MatchUIToGameState();
	return true;
}

bool Game::PlayCard( EventArgs const& args )
{
	PlayerBoard& playerBoard = m_currentGamestate->m_playerBoard;
	Enemy& enemy = m_currentGamestate->m_enemy;

	eCard cardType = (eCard)args.GetValue( "cardType", (int)eCard::Strike );
	//Start death of card animation
	CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( cardType );
	Widget* invalidWidget = nullptr;
	Widget* cardWidget = (Widget*)args.GetValue<std::uintptr_t>( "cardWidget", (std::uintptr_t)invalidWidget );

	int cardCost = cardDef.GetCost();
	int cardAttack = cardDef.GetAttack();

	if( playerBoard.CanConsumeEnergy( cardCost ) )
	{
		if( playerBoard.TryMoveCardFromHandToDiscardPile( cardType ) )
		{
			enemy.TakeDamage( cardAttack );


			m_isUIDirty = true;
			playerBoard.ConsumeEnergy( cardCost );
			cardWidget->SetIsVisible( false );
			cardWidget->SetCanHover( false );
			cardWidget->SetCanSelect( false );
			cardWidget->SetCanDrag( false );
			
			return true;
		}
	}
	m_isUIDirty = true;

	return false;
}

void Game::UpdateUI()
{
	if( m_isUIDirty )
	{
		m_isUIDirty = false;
		MatchUIToGameState();
	}
	PlayerBoard const& playerBoard  = m_currentGamestate->m_playerBoard;
	int playerHealth = playerBoard.m_playerHealth;
	int playerMaxHealth = playerBoard.m_playerMaxHealth;
	float playerHealthScale = (float)playerHealth/(float)playerMaxHealth;
	m_playerHealthWidget->SetSliderValue( playerHealthScale );

	Enemy const& enemy = m_currentGamestate->m_enemy;
	int enemyHealth = enemy.m_enemyHealth;
	int enemyMaxHealth = enemy.m_enemyMaxHealth;
	float enemyHealthScale = (float)enemyHealth/(float)enemyMaxHealth;
	m_enemyHealthWidget->SetSliderValue( enemyHealthScale );

	int deckSize = playerBoard.GetDeckSize();
	m_deckWidget->SetText( Stringf( "%i", deckSize ) );

	int discardPileSize = playerBoard.GetDiscardPileSize();
	m_discardPileWidget->SetText( Stringf( "%i", discardPileSize ) );

	int energy = playerBoard.GetEnergy();
	int maxEnergy = playerBoard.GetMaxEnergy();

	m_energyWidget->SetText( Stringf( "%i/%i", energy, maxEnergy ) );
}

void Game::MatchUIToGameState()
{
	PlayerBoard const& playerBoard = m_currentGamestate->m_playerBoard;
	m_handWidget->ClearChildren();

	AABB2 handBounds = m_handWidget->GetLocalAABB2();
	std::vector<AABB2> cardSlots = handBounds.GetBoxAsColumns( playerBoard.GetHandSize() );
	std::vector<eCard> playerHand = playerBoard.GetHandAsVector();
	for( size_t handIndex = 0; handIndex < playerHand.size(); handIndex++ )
	{
		Vec2 slotCenter = cardSlots[handIndex].GetCenter();
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		cardWidget->SetPosition( slotCenter );
		CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( playerHand[handIndex] );
		cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );
	
		EventArgs& releaseArgs = cardWidget->m_releaseArgs;
		eCard cardType = cardDef.GetCardType();
		releaseArgs.SetValue( "cardType", (int)cardType );
		releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
		Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
		releaseDelegate.SubscribeMethod( this, &Game::PlayCard );

		m_handWidget->AddChild( cardWidget );
	}
}

void Game::StartupCardGame()
{
	CardDefinition::InitializeCardDefinitions();
	m_currentGamestate = new GameState();

	PlayerBoard& player = m_currentGamestate->m_playerBoard;
	player.DrawHand();
}

void Game::StartupUI()
{
	m_UIManager = new UIManager( Vec2( 16.f, 9.f ), g_theRenderer );
	Widget* rootWidget = m_UIManager->GetRootWidget();
	GPUMesh* uiMesh = m_UIManager->GetUIMesh();

	Texture* backgroundTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/background.jpg" );
	Texture* energyStoneTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnergyStone.png" );
	Texture* deckTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Magic_card_back.jpg" );
	Texture* buttonTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/button-icon.jpg" );
	//Texture* strikeTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Strike_r.png" );
	m_cyanTexture = g_theRenderer->CreateTextureFromColor( Rgba8::CYAN );
	m_redTexture = g_theRenderer->CreateTextureFromColor( Rgba8::RED );
	m_greenTexture = g_theRenderer->CreateTextureFromColor( Rgba8::GREEN );
	//Texture* handTexture = g_theRenderer->CreateTextureFromColor( Rgba8::SandyTan );


	rootWidget->SetTexture( backgroundTexture, nullptr, nullptr );
	rootWidget->SetIsVisible( true );

	std::string testEvent = "help";
	AABB2 screenBounds = m_UIManager->GetScreenBounds();



	Vec3 healthScale = Vec3( 2.f, 0.2f, 1.f );
	Transform playerHealthTransform = Transform();
	playerHealthTransform.m_scale = healthScale;
	playerHealthTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.3f, 0.5f ) );
	m_playerHealthWidget = new WidgetSlider( uiMesh, playerHealthTransform );
	m_playerHealthWidget->SetBackgroundAndFillTextures( m_redTexture, m_greenTexture );
	rootWidget->AddChild( m_playerHealthWidget );

	Transform enemyHealthTransform = playerHealthTransform;
	enemyHealthTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.7f, 0.5f ) );
	m_enemyHealthWidget = new WidgetSlider( uiMesh, enemyHealthTransform );
	m_enemyHealthWidget->SetBackgroundAndFillTextures( m_redTexture, m_greenTexture );
	rootWidget->AddChild( m_enemyHealthWidget );

	//Base card
	Vec3 baseCardScale = Vec3( 2.f, 2.5f, 1.f );
	Transform baseTransform = Transform();
	baseTransform.m_scale = baseCardScale;
	m_baseCardWidget = new Widget( uiMesh, baseTransform );
	m_baseCardWidget->SetCanDrag( true );

	//Hand widget
	Vec3 handScale = Vec3( 12.f, 3.f, 1.f );
	Transform handTransform = Transform();
	handTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.2f ) );
	handTransform.m_scale = handScale;
	m_handWidget = new Widget( uiMesh, handTransform );
	//m_handWidget->SetTexture( handTexture, nullptr, nullptr );
	m_handWidget->SetIsVisible( false );
	rootWidget->AddChild( m_handWidget );

// 	Vec3 scale = Vec3( 2.f, 2.5f, 1.f );
// 	Transform card1Transform = Transform();
// 	card1Transform.m_position = Vec2( -3.f, 0.f );
// 	card1Transform.m_scale = scale;
// 	card1Transform.m_rotationPitchRollYawDegrees.y = 0.f;
// 	Widget* card1Widget = new Widget( uiMesh, card1Transform );
// 	card1Widget->SetTexture( strikeTexture, m_highlightTexture, m_selectTexture );
// 	card1Widget->SetEventToFire( testEvent );
// 	card1Widget->SetCanDrag( true );
// 	m_handWidget->AddChild( card1Widget );
// 
// 	Transform card2Transform = Transform();
// 	card2Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.425f, 0.245f ) );
// 	card2Transform.m_scale = scale;
// 	card2Transform.m_rotationPitchRollYawDegrees.y = 7.5f;
// 	Widget* card2Widget = new Widget( uiMesh, card2Transform );
// 	card2Widget->SetTexture( strikeTexture, m_highlightTexture, m_selectTexture );
// 	card2Widget->SetEventToFire( testEvent );
// 	card2Widget->SetCanDrag( true );
// 	rootWidget->AddChild( card2Widget );
// 	
// 	Transform card3Transform = Transform();
// 	card3Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.25f ) );
// 	card3Transform.m_scale = scale;
// 	Widget* card3Widget = new Widget( uiMesh, card3Transform );
// 	card3Widget->SetTexture( strikeTexture, m_highlightTexture, m_selectTexture );
// 	card3Widget->SetEventToFire( testEvent );
// 	rootWidget->AddChild( card3Widget );
// 
// 	Transform card4Transform = Transform();
// 	card4Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.575f, 0.245f ) );
// 	card4Transform.m_scale = scale;
// 	card4Transform.m_rotationPitchRollYawDegrees.y = -7.5f;
// 	Widget* card4Widget = new Widget( uiMesh, card4Transform );
// 	card4Widget->SetTexture( strikeTexture, m_highlightTexture, m_selectTexture );
// 	card4Widget->SetEventToFire( testEvent );
// 	rootWidget->AddChild( card4Widget );
// 
// 	Transform card5Transform = Transform();
// 	card5Transform.m_position = screenBounds.GetPointAtUV( Vec2( 0.65f, 0.2f ) );
// 	card5Transform.m_scale = scale;
// 	card5Transform.m_rotationPitchRollYawDegrees.y = -15.f;
// 	Widget* card5Widget = new Widget( uiMesh, card5Transform );
// 	card5Widget->SetTexture( strikeTexture, m_highlightTexture, m_selectTexture );
// 	card5Widget->SetEventToFire( testEvent );
// 	rootWidget->AddChild( card5Widget );



	Vec3 deckScale = Vec3( 1.f, 1.5f, 1.f );
	Transform deckTransform = Transform();
	deckTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.05f, 0.1f ) );
	deckTransform.m_scale = deckScale;
	Widget* deckWidget = new Widget( uiMesh, deckTransform );
	deckWidget->SetTexture( deckTexture, m_cyanTexture, m_redTexture );
	deckWidget->SetCanHover( true );
	deckWidget->SetText( "Hello" );
	deckWidget->SetTextSize( 0.1f );
	rootWidget->AddChild( deckWidget );
	m_deckWidget = deckWidget;

	m_energyWidget = new Widget( *deckWidget );
	m_energyWidget->SetTexture( energyStoneTexture, m_cyanTexture, m_redTexture );
	m_energyWidget->SetPosition( screenBounds.GetPointAtUV( Vec2( 0.05f, 0.3f ) ) );
	rootWidget->AddChild( m_energyWidget );

	Widget* discardWidget = new Widget( *deckWidget );
	discardWidget->SetPosition( screenBounds.GetPointAtUV( Vec2( 0.95f, 0.1f ) ) );
	rootWidget->AddChild( discardWidget );
	m_discardPileWidget = discardWidget;

	Transform endTurnTransform = Transform();
	endTurnTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.95f, 0.25f ) );
	endTurnTransform.m_scale = Vec3( 1.5f, 0.75f, 1.f );
	m_endTurnWidget = new Widget( uiMesh, endTurnTransform );
	m_endTurnWidget->SetCanSelect( true );
	m_endTurnWidget->SetEventToFire( "endTurn" );
	m_endTurnWidget->SetText( "End Turn" );
	m_endTurnWidget->SetTextSize( 0.1f );
	m_endTurnWidget->SetTexture( buttonTexture, m_cyanTexture, m_redTexture );
	rootWidget->AddChild( m_endTurnWidget );

	MatchUIToGameState();
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

}

void Game::RenderGame()
{
	//g_theRenderer->BeginCamera( m_camera );
	//m_world->Render();
	//m_rootWidget->Render();
	//g_theRenderer->EndCamera( m_camera );
}

void Game::RenderUI()
{
	m_UIManager->Render();
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
	}
	if( hKey.IsPressed() )
	{
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
	}
	if( fKey.WasJustPressed() )
	{
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