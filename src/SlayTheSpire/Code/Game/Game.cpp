#pragma once
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState.hpp"
#include "Game/PlayerBoard.hpp"
#include "Game/Enemy.hpp"
#include "Game/MoveTypeDefinition.hpp"
#include "Game/CardDefinition.hpp"
#include "Game/StatusDefinition.hpp"
#include "Game/EnemyDefinition.hpp"
#include "Game/RewardDefinition.hpp"
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
#include "Engine/UI/WidgetAnimation.hpp"
#include "Engine/UI/UIManager.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"


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

	g_theUIManager = new UIManager( Vec2( 16.f, 9.f ), g_theRenderer );
	g_theUIManager->Startup();

	InitializeDefinitions();
	StartupCardGame();
	StartupUI();
	MatchUIToGameState();

	g_theEventSystem->SubscribeMethodToEvent( "endTurn", NOCONSOLECOMMAND, this, &Game::EndTurn );

	g_theEventSystem->SubscribeMethodToEvent( "checkFightOver", NOCONSOLECOMMAND, this, &Game::FightOver );
	g_theEventSystem->SubscribeMethodToEvent( "restartFight", NOCONSOLECOMMAND, this, &Game::RestartGame );
	g_theEventSystem->SubscribeMethodToEvent( "loadNextFight", NOCONSOLECOMMAND, this, &Game::LoadNextFight );
}

void Game::Shutdown()
{
	if( g_theUIManager )
	{
		delete g_theUIManager;
		g_theUIManager = nullptr;
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
	g_theUIManager->Update( dt );
	m_currentGamestate->Update( dt );

	//UpdateCamera( dt );
	g_theUIManager->EndFrame();
}

void Game::Render()
{
	RenderUI();

	DebugRenderBeginFrame();
	DebugRenderWorldToCamera( &m_camera );
	DebugRenderScreenTo( g_theRenderer->GetBackBuffer() );
	DebugRenderEndFrame();
}

bool Game::RestartGame( EventArgs const& args )
{
	UNUSED( args );

	ClearEndFightWidgets();
// 	m_endFightWidget->SetIsVisible( false );
// 	m_endFightWidget->SetCanHover( false );

	m_currentGamestate->m_player.Reset();
	m_currentGamestate->m_enemy.Reset();

	m_isUIDirty = true;


	//m_endFightWidget->m_releaseDelegate.UnsubscribeObject( this );
	return true;
}

bool Game::LoadNextFight( EventArgs const& args )
{
	UNUSED( args );

	ClearEndFightWidgets();
// 	m_endFightWidget->SetIsVisible( false );
// 	m_endFightWidget->SetCanHover( false );
// 
// 	m_endFightTextWidget->MarkGarbage();
// 	m_endFightCard1Widget->MarkGarbage();
// 	m_endFightCard2Widget->MarkGarbage();
// 	m_endFightCard3Widget->MarkGarbage();

	//m_currentGamestate->m_player.Reset();
	m_currentGamestate->m_player.ResetNoHealth();
	EnemyDefinition const& nextEnemyDef = EnemyDefinition::GetEnemyDefinitionByType( JawWorm );
	m_currentGamestate->m_enemy.SetEnemyDef( &nextEnemyDef );
	m_currentGamestate->m_enemy.Reset();

	m_isUIDirty = true;

	return true;
}

bool Game::StartPlayerEndTurn( EventArgs const& args )
{
 	//PlayerBoard const& playerBoard = m_currentGamestate->m_player.m_playerBoard;
 	std::vector<Widget*> childWidgets = m_handWidget->GetChildWidgets();

	float animationSpeed = 1.f;
	for( Widget* childWidget : childWidgets )
	{
		childWidget->SetCanHover( false );

		Transform toDiscardPileTransform;
		toDiscardPileTransform.m_position = Vec3( 7.f, 0.f, 0.f );
		toDiscardPileTransform.m_rotationPitchRollYawDegrees = Vec3( 0.f, -180.f, 0.f );
		toDiscardPileTransform.m_scale = Vec3( 0.1f, 0.1f, 1.f );

		Delegate<EventArgs const&>& endAnimationDelegate = childWidget->StartAnimation( toDiscardPileTransform, 0.5f * animationSpeed, eSmoothingFunction::SMOOTHSTART3 );
		if( childWidget == childWidgets.back() )
		{
			endAnimationDelegate.SubscribeMethod( this, &Game::EndPlayerEndTurn );
		}

		animationSpeed *= 1.1f;
	}

	return true;
}

bool Game::EndPlayerEndTurn( EventArgs const& args )
{
	UNUSED( args );
	Player& player = m_currentGamestate->m_player;

	PlayerBoard& playerBoard = player.m_playerBoard;
	playerBoard.DiscardHand();
// 	playerBoard.DrawHand();
// 	playerBoard.m_playerEnergy = playerBoard.m_playerMaxEnergy;
// 	DoEnemyTurn();
// 	player.ResetBlock();
	m_isUIDirty = true;

	StartEnemyTurn( EventArgs() );
	return true;
}

bool Game::StartEnemyTurn( EventArgs const& args )
{
	m_currentGamestate->m_enemy.BeginAttack( EventArgs() );

	return true;
}

bool Game::EnemyDealDamage( EventArgs const& args )
{
	Player& player = m_currentGamestate->m_player;
	Enemy& enemy = m_currentGamestate->m_enemy;

	enemy.ResetBlock();

	EnemyMove const& move = enemy.GetEnemyMove();

	enemy.UpdateStatuses();
	
	eStatus status = move.m_statusDef->m_statusType;
	if( status == Ritual )
	{
		enemy.AddStatus( status );
	}
	else if( status != INVALID_STATUS )
	{
		player.AddStatus( status );
	}

	int damage = move.m_damage;
	damage = enemy.GetDamagePostStrength( damage );
	int block = move.m_block;
	int strength = move.m_strength;
	player.TakeDamage( damage );
	enemy.GainBlock( block );
	enemy.AddStrength( strength );

	return true;
}

bool Game::EndEnemyTurn( EventArgs const& args )
{
	DoEnemyTurn();

	return true;

}

bool Game::StartStartPlayerTurn( EventArgs const& args )
{
	Player& player = m_currentGamestate->m_player;
	PlayerBoard& playerBoard = player.m_playerBoard;

	player.ResetBlock();
	playerBoard.DrawHand();

	//PlayerBoard const& playerBoard = m_currentGamestate->m_player.m_playerBoard;
	m_handWidget->ClearChildren();

	AABB2 handBounds = m_handWidget->GetLocalAABB2();
	std::vector<AABB2> cardSlots = handBounds.GetBoxAsColumns( playerBoard.GetHandSize() );
	std::vector<eCard> playerHand = playerBoard.GetHandAsVector();

	std::vector<Transform> handTransforms = GetGoalHandTransforms( (int)playerHand.size() );

	for( size_t handIndex = 0; handIndex < playerHand.size(); handIndex++ )
	{
		//Create card
		Vec2 slotCenter = cardSlots[handIndex].GetCenter();
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		Transform startTransform = cardWidget->GetTransform();
		startTransform.m_position = Vec2( -6.f, 0.f );
		startTransform.m_rotationPitchRollYawDegrees = Vec3( 0.f, 90.f, 0.f );
		//cardWidget->SetPosition( Vec2( -6.f, 0.f ) );
		cardWidget->SetTransform( startTransform );
		//cardWidget->SetPosition( slotCenter );
		CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( playerHand[handIndex] );
		cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );

		//Add Play Card event to card
		EventArgs& releaseArgs = cardWidget->m_releaseArgs;
		eCard cardType = cardDef.GetCardType();
		releaseArgs.SetValue( "cardType", (int)cardType );
		releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
		Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
		releaseDelegate.SubscribeMethod( this, &Game::ReleaseTargeting );
		releaseDelegate.SubscribeMethod( this, &Game::StartPlayCard );

		Delegate<EventArgs const&>& selectedDelegate = cardWidget->m_selectedDelegate;
		selectedDelegate.SubscribeMethod( this, &Game::UpdateTargeting );

		m_handWidget->AddChild( cardWidget );

		Transform finalPositionTransform = m_baseCardWidget->GetTransform();
		//finalPositionTransform.m_position = slotCenter;
		finalPositionTransform = handTransforms[handIndex];
		cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTEP3 );
		//cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTART3 );
	}

	EndStartPlayerTurn( EventArgs() );

	return true;
}

bool Game::EndStartPlayerTurn( EventArgs const& args )
{
	Player& player = m_currentGamestate->m_player;
	PlayerBoard& playerBoard = player.m_playerBoard;


	playerBoard.m_playerEnergy = playerBoard.m_playerMaxEnergy;

	//MatchUIToGameState();

	return true;
}

bool Game::EndTurn( EventArgs const& args )
{
	return StartPlayerEndTurn( args );
}

bool Game::StartPlayCard( EventArgs const& args )
{
	Player& player = m_currentGamestate->m_player;
	PlayerBoard& playerBoard = player.m_playerBoard;
	Enemy& enemy = m_currentGamestate->m_enemy;

	eCard cardType = (eCard)args.GetValue( "cardType", (int)eCard::Strike );
	//Start death of card animation
	CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( cardType );
	Widget* invalidWidget = nullptr;
	Widget* cardWidget = (Widget*)args.GetValue<std::uintptr_t>( "cardWidget", (std::uintptr_t)invalidWidget );

	int cardCost = cardDef.GetCost();
	int cardAttack = cardDef.GetAttack();
	cardAttack = player.GetDamagePostStrength( cardAttack );
	int cardBlock = cardDef.GetBlock();
	bool isSpotWeakness = cardDef.GetIsSpotWeakness();

	if( playerBoard.CanConsumeEnergy( cardCost ) )
	{
		if( playerBoard.TryMoveCardFromHandToDiscardPile( cardType ) )
		{
			enemy.TakeDamage( cardAttack );
			player.GainBlock( cardBlock );
 			playerBoard.ConsumeEnergy( cardCost );

			if( isSpotWeakness )
			{
				MoveTypeDefinition const* enemyIntent= enemy.m_currentIntent;
				if( enemyIntent )
				{
					eMoveType enemyMoveType = enemyIntent->m_moveType;
					if( enemyMoveType == Attack || enemyMoveType == AttackDefend )
					{
						player.AddStrength( 3 );
					}
				}
			}
			if( cardDef.m_cardDraw > 0 )
			{
				for( int cardCount = 0; cardCount < cardDef.m_cardDraw; cardCount++ )
				{
					StartDrawCard( EventArgs() );
				}
			}
			if( cardDef.m_enemyStrengthModifier )
			{
				enemy.AddStrength( cardDef.m_enemyStrengthModifier );
			}

			if( cardDef.m_isLimitBreak )
			{
				player.AddStrength( player.GetStrength() );
			}
			
			if( cardDef.m_maxHealthIncrease != 0 && enemy.GetHealth() <= 0 )
			{
				player.AddMaxHealth( cardDef.m_maxHealthIncrease );
			}

			Transform toDiscardPileTransform;
			toDiscardPileTransform.m_position = Vec3( 6.f, 0.f, 0.f );
			toDiscardPileTransform.m_rotationPitchRollYawDegrees = Vec3( 0.f, -180.f, 0.f );
			toDiscardPileTransform.m_scale = Vec3( 0.1f, 0.1f, 1.f );

			Delegate<EventArgs const&>& endAnimationDelegate = cardWidget->StartAnimation( toDiscardPileTransform, 0.5f, eSmoothingFunction::SMOOTHSTART3 );
			endAnimationDelegate.SubscribeMethod( this, &Game::EndPlayCard );
			
			enemy.UpdateIntentWidget();

			return true;
		}
	}
	else
	{
		m_isUIDirty = true;
	}

	return false;
}

bool Game::StartDrawCard( EventArgs const& args )
{
	Player& player = m_currentGamestate->m_player;
	PlayerBoard& playerBoard = player.m_playerBoard;

	eCard drawnCard = playerBoard.DrawCard();
	CardDefinition const& drawnCardDef = CardDefinition::GetCardDefinitionByType( drawnCard );



	AABB2 handBounds = m_handWidget->GetLocalAABB2();
	std::vector<AABB2> cardSlots = handBounds.GetBoxAsColumns( playerBoard.GetHandSize() );
	std::vector<eCard> playerHand = playerBoard.GetHandAsVector();

	std::vector<Transform> handTransforms = GetGoalHandTransforms( (int)playerHand.size() );


	std::vector<Widget*> cardWidgetsInHand = m_handWidget->GetChildWidgets();

	for( size_t handIndex = 0; handIndex < playerHand.size(); handIndex++ )
	{
		if( handIndex == 0 )
		{
			Vec2 slotCenter = cardSlots[handIndex].GetCenter();
			Widget* cardWidget = new Widget( *m_baseCardWidget );
			Transform startTransform = cardWidget->GetTransform();
			startTransform.m_position = Vec2( -6.f, 0.f );
			startTransform.m_rotationPitchRollYawDegrees = Vec3( 0.f, 90.f, 0.f );
			cardWidget->SetTransform( startTransform );
			CardDefinition const& cardDef = drawnCardDef;
			cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );

			//Add Play Card event to card
			EventArgs& releaseArgs = cardWidget->m_releaseArgs;
			eCard cardType = cardDef.GetCardType();
			releaseArgs.SetValue( "cardType", (int)cardType );
			releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
			Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
			releaseDelegate.SubscribeMethod( this, &Game::ReleaseTargeting );
			releaseDelegate.SubscribeMethod( this, &Game::StartPlayCard );

			Delegate<EventArgs const&>& selectedDelegate = cardWidget->m_selectedDelegate;
			selectedDelegate.SubscribeMethod( this, &Game::UpdateTargeting );

			m_handWidget->AddChild( cardWidget );

			Transform finalPositionTransform = m_baseCardWidget->GetTransform();
			finalPositionTransform = handTransforms[handIndex];
			cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTEP3 );
		}
		else
		{
			//Create card
			Vec2 slotCenter = cardSlots[handIndex].GetCenter();
			Widget* cardWidget = cardWidgetsInHand[handIndex-1];
			Transform startTransform = cardWidget->GetTransform();

			CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( playerHand[handIndex-1] );
			cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );

			Transform finalPositionTransform = m_baseCardWidget->GetTransform();
			finalPositionTransform = handTransforms[handIndex];
			cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTEP3 );
		}
// 		//Create card
// 		Vec2 slotCenter = cardSlots[handIndex].GetCenter();
// 		Widget* cardWidget = new Widget( *m_baseCardWidget );
// 		Transform startTransform = cardWidget->GetTransform();
// 		startTransform.m_position = Vec2( -6.f, 0.f );
// 		startTransform.m_rotationPitchRollYawDegrees = Vec3( 0.f, 90.f, 0.f );
// 		//cardWidget->SetPosition( Vec2( -6.f, 0.f ) );
// 		cardWidget->SetTransform( startTransform );
// 		//cardWidget->SetPosition( slotCenter );
// 		CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( playerHand[handIndex] );
// 		cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );
// 
// 		//Add Play Card event to card
// 		EventArgs& releaseArgs = cardWidget->m_releaseArgs;
// 		eCard cardType = cardDef.GetCardType();
// 		releaseArgs.SetValue( "cardType", (int)cardType );
// 		releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
// 		Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
// 		releaseDelegate.SubscribeMethod( this, &Game::ReleaseTargeting );
// 		releaseDelegate.SubscribeMethod( this, &Game::StartPlayCard );
// 
// 		Delegate<EventArgs const&>& selectedDelegate = cardWidget->m_selectedDelegate;
// 		selectedDelegate.SubscribeMethod( this, &Game::UpdateTargeting );
// 
// 		m_handWidget->AddChild( cardWidget );
// 
// 		Transform finalPositionTransform = m_baseCardWidget->GetTransform();
// 		//finalPositionTransform.m_position = slotCenter;
// 		finalPositionTransform = handTransforms[handIndex];
// 		cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTEP3 );
// 		//cardWidget->StartAnimation( finalPositionTransform, 0.5f, eSmoothingFunction::SMOOTHSTART3 );
	}

//	EndStartPlayerTurn( EventArgs() );

	return true;
}

bool Game::EndPlayCard( EventArgs const& args )
{
	m_isUIDirty = true;

	return false;
}

bool Game::FightOver( EventArgs const& args )
{
	UNUSED( args );

	int playerHealth = m_currentGamestate->m_player.GetHealth();
	int enemyHealth = m_currentGamestate->m_enemy.GetHealth();

	if( playerHealth <= 0 )
	{
		if( m_endFightWidget )
		{
			m_endFightWidget->SetText( "You Lose! Click to restart." );
			m_endFightWidget->SetIsVisible( true );
			m_endFightWidget->SetCanHover( true );
			m_endFightWidget->SetCanSelect( true );
			m_endFightWidget->m_releaseDelegate.SubscribeMethod( this, &Game::RestartGame );
		}
		else
		{
			Texture const* darkGreyTexture = g_theRenderer->CreateTextureFromColor( Rgba8( 128, 128, 128, 255 ) );

			Transform endFightTransform;
			endFightTransform.m_scale = Vec3( 16.f, 9.f, 1.f );
			m_endFightWidget = new Widget( endFightTransform );
			m_endFightWidget->SetTexture( darkGreyTexture, darkGreyTexture, darkGreyTexture );
			m_endFightWidget->SetText( "You Lose! Click to restart." );
			m_endFightWidget->SetTextSize( 0.2f );
			m_endFightWidget->SetCanHover( true );
			m_endFightWidget->SetCanDrag( false );
			m_endFightWidget->SetIsVisible( true );
			g_theUIManager->GetRootWidget()->AddChild( m_endFightWidget );

			m_endFightWidget->m_releaseDelegate.SubscribeMethod( this, &Game::RestartGame );
			//m_endFightWidget->SetEventToFire( "restartFight" );
		}
		//You win!
	}
	else if( enemyHealth <= 0 )
	{
		if( m_endFightWidget )
		{
			m_endFightWidget->SetText( "You Win!" );
			m_endFightWidget->SetIsVisible( true );
			m_endFightWidget->SetCanHover( true );
			m_endFightWidget->SetCanSelect( true );

			GenerateAndDisplayEndFightAddCardsWidgets();
		}
		else
		{
			Texture const* darkGreyTexture = g_theRenderer->CreateTextureFromColor( Rgba8( 128, 128, 128, 255 ) );
			Transform endFightTransform;
			endFightTransform.m_scale = Vec3( 16.f, 9.f, 1.f );
			m_endFightWidget = new Widget( endFightTransform );
			m_endFightWidget->SetTexture( darkGreyTexture, darkGreyTexture, darkGreyTexture );
			m_endFightWidget->SetTextSize( 0.2f );
			m_endFightWidget->SetCanHover( true );
			m_endFightWidget->SetCanDrag( false );
			m_endFightWidget->SetIsVisible( true );
			g_theUIManager->GetRootWidget()->AddChild( m_endFightWidget );

			GenerateAndDisplayEndFightAddCardsWidgets();
		}
	}

	return true;
}

bool Game::ChooseNextFight( EventArgs const& args )
{
	if( !m_endFightWidget )
	{
		Texture const* darkGreyTexture = g_theRenderer->CreateTextureFromColor( Rgba8( 128, 128, 128, 255 ) );
		Transform endFightTransform;
		endFightTransform.m_scale = Vec3( 16.f, 9.f, 1.f );
		m_endFightWidget = new Widget( endFightTransform );
		m_endFightWidget->SetTexture( darkGreyTexture, darkGreyTexture, darkGreyTexture );
		m_endFightWidget->SetTextSize( 0.2f );
		m_endFightWidget->SetCanHover( true );
		m_endFightWidget->SetCanDrag( false );
		m_endFightWidget->SetIsVisible( true );
		g_theUIManager->GetRootWidget()->AddChild( m_endFightWidget );


	}

	GenerateAndDisplayChooseFightWidgets();

	return true;
}

bool Game::UpdateTargeting( EventArgs const& args )
{
	Vec2 startPos = args.GetValue( "currentPos", Vec2() );
	Vec2 endPos = args.GetValue( "mousePos", Vec2() );
	Vec2 startTangent = Vec2( 0.f, 1.f );

	if( m_isTargeting )
	{
		UpdateTargetingWidgets( endPos );
	}
	else
	{
		DebuggerPrintf( "Creating TargetingWidgets\n" );
		m_isTargeting = true;
		CreateTargetingWidgets( startPos, endPos, startTangent, 10 );
	}

	return true;
}

bool Game::ReleaseTargeting( EventArgs const& args )
{
	UNUSED( args );
	DebuggerPrintf( "Releasing TargetingWidgets\n" );
	ClearTargetingWidgets();
	m_isTargeting = false;

	return false;
}

bool Game::AddCardToPlayerPermanentDeck( EventArgs const& args )
{
	int cardIndex = args.GetValue( "cardIndex", 0 );
	m_currentGamestate->m_player.m_playerBoard.AddCardToPermanentDeck( (eCard)cardIndex );

	return false;
}

void Game::InitializeDefinitions()
{
	MoveTypeDefinition::InitializeMoveTypeDefs();
	StatusDefinition::InitializeStatusDefinitions();
	EnemyDefinition::InitializeEnemyDefinitions();
	CardDefinition::InitializeCardDefinitions();
}

void Game::UpdateUI()
{
	if( m_isUIDirty )
	{
		m_isUIDirty = false;
		MatchUIToGameState();
	}
	PlayerBoard const& playerBoard  = m_currentGamestate->m_player.m_playerBoard;

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
	PlayerBoard const& playerBoard = m_currentGamestate->m_player.m_playerBoard;
	m_handWidget->ClearChildren();

	AABB2 handBounds = m_handWidget->GetLocalAABB2();
	std::vector<AABB2> cardSlots = handBounds.GetBoxAsColumns( playerBoard.GetHandSize() );
	std::vector<eCard> playerHand = playerBoard.GetHandAsVector();

	std::vector<Transform> handSlotTransforms = GetGoalHandTransforms( (int)playerHand.size() );

	for( size_t handIndex = 0; handIndex < playerHand.size(); handIndex++ )
	{
		//Create card
		Vec2 slotCenter = cardSlots[handIndex].GetCenter();
		Widget* cardWidget = new Widget( *m_baseCardWidget );
		cardWidget->SetPosition( slotCenter );
		cardWidget->SetTransform( handSlotTransforms[handIndex] );
		CardDefinition const& cardDef = CardDefinition::GetCardDefinitionByType( playerHand[handIndex] );
		cardWidget->SetTexture( cardDef.GetCardTexture(), m_cyanTexture, m_redTexture );
	
		//Add Play Card event to card
		EventArgs& releaseArgs = cardWidget->m_releaseArgs;
		eCard cardType = cardDef.GetCardType();
		releaseArgs.SetValue( "cardType", (int)cardType );
		releaseArgs.SetValue( "cardWidget", (std::uintptr_t)cardWidget );
		Delegate<EventArgs const&>& releaseDelegate = cardWidget->m_releaseDelegate;
		releaseDelegate.SubscribeMethod( this, &Game::ReleaseTargeting );
		releaseDelegate.SubscribeMethod( this, &Game::StartPlayCard );

		Delegate<EventArgs const&>& selectedDelegate = cardWidget->m_selectedDelegate;
		selectedDelegate.SubscribeMethod( this, &Game::UpdateTargeting );

		m_handWidget->AddChild( cardWidget );
	}
}

void Game::StartupCardGame()
{
	m_currentGamestate = new GameState();

	EnemyDefinition const* cultistDef = &EnemyDefinition::GetEnemyDefinitionByType( Cultist );
	m_currentGamestate->m_enemy = Enemy( cultistDef );

	Enemy& enemy = m_currentGamestate->m_enemy;
	Player& player = m_currentGamestate->m_player;
	PlayerBoard& playerBoard = player.m_playerBoard;
	playerBoard.InitializePlayerBoard();
	playerBoard.DrawHand();

	Widget* rootWidget = g_theUIManager->GetRootWidget();
	player.SetParentWidget( rootWidget );

	AABB2 screenBounds = g_theUIManager->GetScreenBounds();
	Vec2 playerPosition = screenBounds.GetPointAtUV( Vec2( 0.3f, 0.45f ) );
	player.SetEntityPositionRelativeToParent( playerPosition );

	Vec2 enemyPosition = screenBounds.GetPointAtUV( Vec2( 0.7f, 0.45f ) );
	enemy.SetParentWidget( rootWidget );
	enemy.SetEntityPositionRelativeToParent( enemyPosition );
	
	enemy.UpdateEnemyMove( m_rand );

}

void Game::StartupUI()
{
	Widget* rootWidget = g_theUIManager->GetRootWidget();

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
	AABB2 screenBounds = g_theUIManager->GetScreenBounds();

	//Base card
	Vec3 baseCardScale = Vec3( 2.f, 2.5f, 1.f );
	Transform baseTransform = Transform();
	baseTransform.m_scale = baseCardScale;
	m_baseCardWidget = new Widget( baseTransform );
	m_baseCardWidget->SetCanDrag( true );

	//Deck
	Vec3 deckScale = Vec3( 1.f, 1.5f, 1.f );
	Transform deckTransform = Transform();
	deckTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.05f, 0.1f ) );
	deckTransform.m_scale = deckScale;
	Widget* deckWidget = new Widget( deckTransform );
	deckWidget->SetTexture( deckTexture, m_cyanTexture, m_redTexture );
	deckWidget->SetCanHover( true );
	deckWidget->SetText( "Hello" );
	deckWidget->SetTextSize( 0.3f );
	rootWidget->AddChild( deckWidget );
	m_deckWidget = deckWidget;

	//Energy
	m_energyWidget = new Widget( *deckWidget );
	m_energyWidget->SetTextSize( 0.15f );
	m_energyWidget->SetTexture( energyStoneTexture, m_cyanTexture, m_redTexture );
	m_energyWidget->SetPosition( screenBounds.GetPointAtUV( Vec2( 0.05f, 0.3f ) ) );
	m_energyWidget->SetCanHover( false );
	rootWidget->AddChild( m_energyWidget );

	//Discard
	Widget* discardWidget = new Widget( *deckWidget );
	discardWidget->SetPosition( screenBounds.GetPointAtUV( Vec2( 0.95f, 0.1f ) ) );
	rootWidget->AddChild( discardWidget );
	m_discardPileWidget = discardWidget;

	//End Turn
	Transform endTurnTransform = Transform();
	endTurnTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.95f, 0.25f ) );
	endTurnTransform.m_scale = Vec3( 1.5f, 0.75f, 1.f );
	m_endTurnWidget = new Widget( endTurnTransform );
	m_endTurnWidget->SetCanSelect( true );
	m_endTurnWidget->SetEventToFire( "endTurn" );
	m_endTurnWidget->SetText( "End Turn" );
	m_endTurnWidget->SetTextSize( 0.15f );
	m_endTurnWidget->SetTexture( buttonTexture, m_cyanTexture, m_redTexture );
	rootWidget->AddChild( m_endTurnWidget );

	//Hand widget
	Vec3 handScale = Vec3( 12.f, 3.f, 1.f );
	Transform handTransform = Transform();
	handTransform.m_position = screenBounds.GetPointAtUV( Vec2( 0.5f, 0.1f ) );
	handTransform.m_scale = handScale;
	m_handWidget = new Widget( handTransform );
	m_handWidget->SetIsVisible( false );
	rootWidget->AddChild( m_handWidget );
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
	g_theUIManager->Render();
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

std::vector<Transform> Game::GetGoalHandTransforms( int handCount )
{
	std::vector<Transform> handTransforms;

	AABB2 handBounds = m_handWidget->GetLocalAABB2();

	Vec2 handCenter = handBounds.GetCenter();
	Vec2 discCenter = handCenter;


	float discCordLength = handBounds.GetDimensions().x;
	float discChordToTopOfDisc = 0.5f * handBounds.GetDimensions().y;
	float discRadius = ( discCordLength*discCordLength/(4.f * discChordToTopOfDisc) + discChordToTopOfDisc ) / 2.f;
	
	discCenter.y -= discRadius;

	float cardArcLength = 1.75f;
	float cardArcDegrees = 180.f * cardArcLength / ( 3.1415f * discRadius );

	float startingDegrees = (float)handCount / 2.f - 0.5f;
	startingDegrees *= -1.f * cardArcDegrees;

	Vec2 startVec = Vec2( 0.f, discRadius );
	startVec.RotateDegrees( startingDegrees );

	float currentAngle = startingDegrees;
	Vec2 currentVec = startVec;
	int cardIndex = 0;
	while( cardIndex < handCount )
	{
		Vec2 cardPosition = currentVec + discCenter;
		float cardOrientation = currentAngle;

		Transform cardTransform = m_baseCardWidget->GetTransform();
		cardTransform.m_position = cardPosition;
		cardTransform.m_rotationPitchRollYawDegrees.y = cardOrientation;

		handTransforms.push_back( cardTransform );

		currentVec.RotateDegrees( cardArcDegrees );
		currentAngle += cardArcDegrees;

		cardIndex++;
	}

	return handTransforms;
}

void Game::DoEnemyTurn()
{
	Enemy& enemy = m_currentGamestate->m_enemy;
	enemy.UpdateEnemyMove( m_rand );
	StartStartPlayerTurn( EventArgs() );
}

void Game::CreateTargetingWidgets( Vec2 const& startPos, Vec2 const& endPos, Vec2 const& startTangent, int countOfWidgets )
{
	Texture* headTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlaceholderArrow.png" );
	Texture* bodyTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlaceholderArrowBody.png" );


	m_startOfTargetChain = startPos;
	m_endOfTargetChain = endPos;
	m_startOfTargetChainTangent = startTangent;

	float interval = 1.f / (float)countOfWidgets;

	Widget* rootWidget = g_theUIManager->GetRootWidget();
	Vec2 previousPosition = m_startOfTargetChain;
	Vec2 currentTangent = m_startOfTargetChainTangent;

	float currentAngle = currentTangent.GetAngleDegrees();
	currentAngle -= 90.f;
	for( size_t widgetIndex = 0; widgetIndex < countOfWidgets; widgetIndex++ )
	{
		float tValue = (float)widgetIndex * interval;

		Vec2 position = QuadraticBezierCurveUsingStartTangent( m_startOfTargetChain, m_startOfTargetChainTangent, 1.f, m_endOfTargetChain, tValue );

		Transform widgetTransform = Transform( position, Vec3( 0.f, currentAngle, 0.f ), Vec3( 0.5f, 0.5f, 1.f ) );
		Widget* widget = new Widget( widgetTransform, nullptr );
		widget->SetCanHover( false );
		widget->SetTexture( bodyTexture, bodyTexture, bodyTexture );
		m_targetBodyWidgets.push_back( widget );

		rootWidget->AddChild( widget );

		currentTangent = position - previousPosition;
		currentTangent.Normalize();
		currentAngle = currentTangent.GetAngleDegrees() - 90.f;
		previousPosition = position;
	}

	Transform widgetTransform = Transform( m_endOfTargetChain, Vec3( 0.f, currentAngle, 0.f ), Vec3( 0.5f, 0.5f, 1.f ) );
	Widget* widget = new Widget( widgetTransform, nullptr );
	widget->SetCanHover( false );
	widget->SetTexture( headTexture, headTexture, headTexture );
	rootWidget->AddChild( widget );
	m_targetHeadWidget = widget;
}

void Game::UpdateTargetingWidgets( Vec2 const& endPos )
{
	m_endOfTargetChain = endPos;
	int countOfWidgets = (int)m_targetBodyWidgets.size();
	//ClearTargetingWidgets();
	//CreateTargetingWidgets( m_startOfTargetChain, m_endOfTargetChain, m_startOfTargetChainTangent, countOfWidgets );

	float interval = 1.f / (float)countOfWidgets;
	Vec2 previousPosition = m_startOfTargetChain;
	Vec2 currentTangent = m_startOfTargetChainTangent;

	float currentAngle = currentTangent.GetAngleDegrees();
	currentAngle -= 90.f;
	for( size_t widgetIndex = 0; widgetIndex < countOfWidgets; widgetIndex++ )
	{
		float tValue = (float)widgetIndex * interval;

		Vec2 position = QuadraticBezierCurveUsingStartTangent( m_startOfTargetChain, m_startOfTargetChainTangent, 1.f, m_endOfTargetChain, tValue );

		Transform widgetTransform = Transform( position, Vec3( 0.f, currentAngle, 0.f ), Vec3( 0.5f, 0.5f, 1.f ) );
		Widget* widget = m_targetBodyWidgets[widgetIndex];
		widget->SetTransform( widgetTransform );

		currentTangent = position - previousPosition;
		currentTangent.Normalize();
		currentAngle = currentTangent.GetAngleDegrees() - 90.f;
		previousPosition = position;
	}

	Transform headTransform = Transform( endPos, Vec3( 0.f, currentAngle, 0.f ), Vec3( 0.5f, 0.5f, 1.f ) );
	m_targetHeadWidget->SetTransform( headTransform );
}

void Game::ClearTargetingWidgets()
{
	for( Widget* widget : m_targetBodyWidgets )
	{
		widget->MarkGarbage();
	}
	m_targetBodyWidgets.clear();

	if( m_targetHeadWidget )
	{
		m_targetHeadWidget->MarkGarbage();
		m_targetHeadWidget = nullptr;
	}

}

void Game::GenerateAndDisplayEndFightAddCardsWidgets()
{
	Texture const* darkGreyTexture = g_theRenderer->CreateTextureFromColor( Rgba8( 180, 180, 180, 255 ) );

	Transform textTransform = Transform( Vec3(0.f, 3.f, 0.f ), Vec3(), Vec3( 4.f, 1.f, 1.f ) );
	m_endFightTextWidget = new Widget( textTransform, nullptr );
	m_endFightTextWidget->SetText( "Add a card" );
	m_endFightTextWidget->SetTextSize( 0.3f );
	m_endFightTextWidget->SetTexture( darkGreyTexture, nullptr, nullptr );
	m_endFightTextWidget->SetCanHover( false );

	Vec3 cardScale = Vec3( 3.f, 3.75f, 1.f );

	Transform card1Transform = Transform( Vec3( -4.f, -1.f, 0.f ), Vec3(), cardScale );
	Transform card2Transform = Transform( Vec3( 0.f, -1.f, 0.f ), Vec3(), cardScale );
	Transform card3Transform = Transform( Vec3( 4.f, -1.f, 0.f ), Vec3(), cardScale );

	m_endFightCard1Widget = new Widget( card1Transform, nullptr );
	m_endFightCard2Widget = new Widget( card2Transform, nullptr );
	m_endFightCard3Widget = new Widget( card3Transform, nullptr );


	CardDefinition const& cardDef1 = CardDefinition::GetRandomCardDefinition();
	CardDefinition const& cardDef2 = CardDefinition::GetRandomCardDefinition();
	CardDefinition const& cardDef3 = CardDefinition::GetRandomCardDefinition();

	m_endFightCard1Widget->SetTexture( cardDef1.GetCardTexture(), m_cyanTexture, m_redTexture );
	m_endFightCard2Widget->SetTexture( cardDef2.GetCardTexture(), m_cyanTexture, m_redTexture );
	m_endFightCard3Widget->SetTexture( cardDef3.GetCardTexture(), m_cyanTexture, m_redTexture );

	m_endFightCard1Widget->m_releaseArgs.SetValue( "cardIndex", (int)cardDef1.GetCardType() );
	m_endFightCard1Widget->m_releaseDelegate.SubscribeMethod( this, &Game::AddCardToPlayerPermanentDeck );
	m_endFightCard1Widget->m_releaseDelegate.SubscribeMethod( this, &Game::LoadNextFight );


	m_endFightCard2Widget->m_releaseArgs.SetValue( "cardIndex", (int)cardDef2.GetCardType() );
	m_endFightCard2Widget->m_releaseDelegate.SubscribeMethod( this, &Game::AddCardToPlayerPermanentDeck );
	m_endFightCard2Widget->m_releaseDelegate.SubscribeMethod( this, &Game::LoadNextFight );

	m_endFightCard3Widget->m_releaseArgs.SetValue( "cardIndex", (int)cardDef3.GetCardType() );
	m_endFightCard3Widget->m_releaseDelegate.SubscribeMethod( this, &Game::AddCardToPlayerPermanentDeck );
	m_endFightCard3Widget->m_releaseDelegate.SubscribeMethod( this, &Game::LoadNextFight );

	//m_endFightWidget->SetEventToFire( "loadNextFight" );

	m_endFightWidget->AddChild( m_endFightTextWidget );
	m_endFightWidget->AddChild( m_endFightCard1Widget );
	m_endFightWidget->AddChild( m_endFightCard2Widget );
	m_endFightWidget->AddChild( m_endFightCard3Widget );
}

void Game::GenerateAndDisplayChooseFightWidgets()
{
	Texture const* darkGreyTexture = g_theRenderer->CreateTextureFromColor( Rgba8( 180, 180, 180, 255 ) );

	Transform textTransform = Transform( Vec3( 0.f, 3.f, 0.f ), Vec3(), Vec3( 4.f, 1.f, 1.f ) );
	m_chooseFightTextWidget = new Widget( textTransform, nullptr );
	m_chooseFightTextWidget->SetText( "Add a card" );
	m_chooseFightTextWidget->SetTextSize( 0.3f );
	m_chooseFightTextWidget->SetTexture( darkGreyTexture, nullptr, nullptr );
	m_chooseFightTextWidget->SetCanHover( false );

	Vec3 fightScale = Vec3( 3.f, 3.75f, 1.f );

	Transform fight1Transform = Transform( Vec3( -4.f, -1.f, 0.f ), Vec3(), fightScale );
	Transform fight2Transform = Transform( Vec3( 0.f, -1.f, 0.f ), Vec3(), fightScale );
	Transform fight3Transform = Transform( Vec3( 4.f, -1.f, 0.f ), Vec3(), fightScale );

	m_chooseFightOption1Widget = new Widget( fight1Transform, nullptr );
	m_chooseFightOption2Widget = new Widget( fight2Transform, nullptr );
	m_chooseFightOption3Widget = new Widget( fight3Transform, nullptr );

	//FightDefinition
}

void Game::ClearEndFightWidgets()
{
	m_endFightWidget->SetIsVisible( false );
	m_endFightWidget->SetCanHover( false );

	if( m_endFightTextWidget )
	{
		m_endFightTextWidget->MarkGarbage();
	}
	if( m_endFightCard1Widget )
	{
		m_endFightCard1Widget->MarkGarbage();
	}
	if( m_endFightCard2Widget )
	{
		m_endFightCard2Widget->MarkGarbage();
	}
	if( m_endFightCard3Widget )
	{
		m_endFightCard3Widget->MarkGarbage();
	}
}

Fight::Fight( RandomNumberGenerator& rng )
{
	enemyDef = &EnemyDefinition::GetRandomEnemyDefinition( rng );

	rewardDef = &RewardDefinition::GetRandomRewardDefinition( rng );
}
