//-------------------------------------------------------------------------------------
//
// This is part of StarBugz, a Space Invader clone developed with JGE++.
// 
// Version: 0.0.1 (Prototype)
//
// Released Date: 5th December 2005
//
// Copyright (C) 2005 James Hui (a.k.a. Dr.Watson)
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option) any
// later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA 02111-1307 USA
// 
// Bugs and comments can be forwarded to jhkhui@yahoo.com. 
// 
//-------------------------------------------------------------------------------------

#ifndef _GAME_STATES_SPLASH_H_
#define _GAME_STATES_SPLASH_H_

#include "GameState.h"


#define SPLASH_DELAY		1500.0f

class GameStateSplash: public GameState
{
private:
	JTexture* mSplashTex;
	JQuad* mSplashQuad;
	
	int mMode;
	float mAlpha;
	float mTimer;

public:
	GameStateSplash(GameApp* parent);
	~GameStateSplash();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();

};


#endif

