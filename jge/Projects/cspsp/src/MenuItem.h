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

#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include "JRenderer.h"
#include "JLBFont.h"
#include "JGui.h"

#define TYPE_MAIN		0
#define TYPE_NORMAL		1
#define TYPE_OPTION		2
#define TYPE_BUTTON		3

#define SCALE_SELECTED		1.2f
#define SCALE_NORMAL		1.0f


class MenuItem: public JGuiObject
{
private:
	static JRenderer* mRenderer;
	//bool mHasFocus;
	JLBFont *mFont;
	char mText[32];
	int mX;
	int mY;
	int mAlign;
	int mType;

	float mScale;
	float mTargetScale;


public:
	MenuItem(int id, JLBFont *font, char* text, int x, int y, int type, int align = JGETEXT_CENTER, bool hasFocus = false);
	
	virtual void Render(float x, float y);
	virtual void Update(float dt);

	virtual void Entering();
	virtual bool Leaving(u32 key);
	virtual bool ButtonPressed();
};

#endif

