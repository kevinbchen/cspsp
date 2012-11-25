//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include <stdio.h>

#include "GameApp.h"

#include "GameStateSplash.h"
#include "GameStateLoading.h"
#include "GameStateMenu.h"
#include "GameStateOptions.h"
#include "GameStateUpdate.h"
#include "GameStateNewGame.h"
#include "GameStatePlay.h"
#include "GameStateConnect.h"
#include "GameStateLobby.h"
#include "GameStateOnline.h"

/*#ifdef WIN32
#else
#include <pspfpu.h>
#endif*/

JRenderer* GameApp::mRenderer = NULL;
//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameApp::GameApp()
{
	mRenderer = JRenderer::GetInstance();
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameApp::~GameApp()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Create()
{
		//FILE *file = fopen("log.txt","w+");
		//fputs("0",file);
		//fclose(file);

	mGameStates[GAME_STATE_SPLASH] = new GameStateSplash(this);
	mGameStates[GAME_STATE_SPLASH]->Create();

	// Create() of the following will be called at 'loading' phase
	mGameStates[GAME_STATE_LOADING] = new GameStateLoading(this);
	mGameStates[GAME_STATE_MENU] = new GameStateMenu(this);
	mGameStates[GAME_STATE_OPTIONS] = new GameStateOptions(this);
	mGameStates[GAME_STATE_UPDATE] = new GameStateUpdate(this);
	mGameStates[GAME_STATE_NEW_GAME] = new GameStateNewGame(this);
	mGameStates[GAME_STATE_PLAY] = new GameStatePlay(this);
	mGameStates[GAME_STATE_CONNECT] = new GameStateConnect(this);
	mGameStates[GAME_STATE_LOBBY] = new GameStateLobby(this);
	mGameStates[GAME_STATE_ONLINE] = new GameStateOnline(this);

	mCurrentState = NULL;
	mNextState = mGameStates[GAME_STATE_SPLASH];

		//file = fopen("log.txt","a");
		//fputs("1",file);
		//fclose(file);
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Destroy()
{
	#ifdef WIN32
	#else
	//gprof_cleanup();
	#endif

	/*SAFE_DELETE(mBg);

	SAFE_DELETE(mBgQuad);

	SAFE_DELETE(mLogo);

	SAFE_DELETE(mLogoQuad);

	SAFE_DELETE(gFont);

	SAFE_DELETE(gHudFont);

	SAFE_DELETE(mPlayersTexture);

	SAFE_DELETE(mPlayersDeadTexture);

	SAFE_DELETE(mGunsTexture);

	SAFE_DELETE(mGunsGroundTexture);

	SAFE_DELETE(mRadarTexture);

	SAFE_DELETE(mParticlesQuad);

	SAFE_DELETE(mParticlesTex);

	for (int i=GAME_STATE_LOADING;i<=GAME_STATE_ONLINE;i++)
	{
		mGameStates[i]->Destroy();
		delete mGameStates[i];
	}*/
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameApp::Update()
{
	JGE* engine = JGE::GetInstance();	
	
	float dt = engine->GetDelta()*1000;
	//if (dt < 0.000001f) dt = 35.0f;
	if (dt > 35.0f)		// min 30 FPS ;)
		dt = 35.0f;
	else if (dt < 1.0f)
		dt = 16.0f;		// avoid division by 0

	#ifdef WIN32
		clock_t start_time = clock();
	#endif

	if (mCurrentState != NULL)
		mCurrentState->Update(dt);

	if (mNextState != NULL)
	{
		if (mCurrentState != NULL)
			mCurrentState->End();

		mCurrentState = mNextState;
		mCurrentState->Start();

		mNextState = NULL;
	}

	#ifdef WIN32
		/*while((clock() - start_time) < 15)
		{

		}*/
		while (12.66666f-(clock()-start_time) > 0.0f) {
			Sleep(12.66666f-(clock()-start_time));
		}
	#endif
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameApp::Render()
{

	if (mCurrentState != NULL)
	{
		mCurrentState->Render();
	}
	
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameApp::Pause()
{
	
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameApp::Resume()
{
	
}


void GameApp::LoadGameStates()
{

	mGameStates[GAME_STATE_MENU]->Create();
	mGameStates[GAME_STATE_OPTIONS]->Create();
	mGameStates[GAME_STATE_UPDATE]->Create();
	mGameStates[GAME_STATE_NEW_GAME]->Create();
	mGameStates[GAME_STATE_PLAY]->Create();
	mGameStates[GAME_STATE_CONNECT]->Create();
	mGameStates[GAME_STATE_LOBBY]->Create();
	mGameStates[GAME_STATE_ONLINE]->Create();
}


void GameApp::SetNextState(int state)
{
	mNextState = mGameStates[state];
}