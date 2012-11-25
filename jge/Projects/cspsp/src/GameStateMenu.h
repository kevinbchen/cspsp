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


#ifndef _GAME_STATE_MENU_H_
#define _GAME_STATE_MENU_H_

#include "JGui.h"

#include "GameState.h"
#include "MenuItem.h"


#define STAGE_INIT			0
#define STAGE_MENU			1
#define STAGE_CREDITS		2

static char *mCategories[] =
{
	"CSPSP v1.92",
	"2011",
	"",
	"Programming",
	"",
	"Graphics",
	"",
	"",
	"",
	"Sound",
	"",
	"",
	"Map Editor",
	"",
	"Maps",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Testers",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Other",
	"",
	"",
	"",
};


static char *mCredits[] =
{
	"",
	"",
	"",
	"Kevin Chen",
	"",
	"Kevin Chen",
	"Some gun graphics - CS2D",
	"Background - Crisp",
	"",
	"Effects - CS 1.6",
	"Raindrops - Charvin Kessler",
	"",
	"coolguy5678",
	"",
	"Louismap - Crisp",
	"Winter - coolguy5678",
	"Castle Vonbrown, Guano, Small Train Yard - ataxy",
	"Silent Forest & Small Forest - RaiderX",
	"Circle, Lasertag, Lasertag2 - rookie42",
	"fy_poolday - SyntheticChaos",
	"Office2D - Dream_Team",
	"fy_dodgeball - eggbom",
	"fy_nade & de_dust2 - Xiphirx",
	"cs_italy - Doublehawk",
	"",
	"x@xXxNightfoxxXx@x",
	"Chrono",
	"xXxSpectre@",
	"Xiphirx",
	"Doublehawk",
	"Spartan",
	"muhu",
	"",
	"James Hui for his JGE++ Engine",
	"Danzel and Jeff Chen for their input system",
	"PS2DEV forums for help and info",
	"Google for its App Engine",
};


class GameStateMenu:	public GameState,
	 					public JGuiListener

{
private:
	int mStage;
	float mAlpha;
	JGuiController* mGuiController;
	float mInfoX;

	JMusic* mMusic;

	float mCreditsYPos;

public:

	GameStateMenu(GameApp* parent);
	~GameStateMenu();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();
	void ButtonPressed(int controllerId, int controlId);
};

#endif
