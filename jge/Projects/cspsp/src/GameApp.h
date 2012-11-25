//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _GAMEAPP_H_
#define _GAMEAPP_H_

#include <JApp.h>
#include "JGE.h"
#include "JRenderer.h"
#include "JLBFont.h"
#include "GameState.h"
#include "Globals.h"

#ifdef WIN32
#include <ctime>
#else
#include "mp3player.h"

/*extern "C" {
	extern void gprof_cleanup();
}*/
#endif

class GameState;

#define VERSION 1.92f
#define NETVERSION 9

#define MAX_STATE			10

class GameApp:	public JApp
{
private:
	static JRenderer* mRenderer;

	GameState* mCurrentState;
	GameState* mNextState;
	GameState* mGameStates[MAX_STATE];

public:
	GameApp();
	virtual ~GameApp();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

	void LoadGameStates();
	void SetNextState(int state);
};

#endif
