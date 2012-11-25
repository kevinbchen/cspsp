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


#include "MenuItem.h"
#include "Globals.h"

JRenderer* MenuItem::mRenderer = NULL;

MenuItem::MenuItem(int id, JLBFont *font, char* text, int x, int y, int type, int align, bool hasFocus): JGuiObject(id,hasFocus), mFont(font), mX(x), mY(y)
{
	mRenderer = JRenderer::GetInstance();
	strcpy(mText,text);
	mType = type;
	//mHasFocus = hasFocus;
	mAlign = align;
	mScale = 1.0f;
	mTargetScale = 1.0f;
	
	if (hasFocus)
		Entering();
	
}

	
void MenuItem::Render(float x, float y)
{
	x += mX;
	y += mY;
	if (mType == TYPE_MAIN) {
		mFont->SetScale(mScale);

		if (mHasFocus) {
			mFont->SetColor(ARGB(255,0,128,255));
		}
		else {
			mFont->SetColor(ARGB(255,255,255,255));
		}
		mFont->DrawShadowedString(mText,x,y,mAlign);

		mFont->SetScale(1.0f);
	}
	else if (mType == TYPE_NORMAL) {
		mFont->SetScale(mScale);

		if (mHasFocus)
		{
			mFont->SetColor(ARGB(255,255,255,0));
			mFont->DrawString(mText, x, y, mAlign);
			//mEngine->DrawLine(
		}
		else
		{
			mFont->SetColor(ARGB(255,255,255,255));
			mFont->DrawString(mText, x, y, mAlign);
		}

		mFont->SetScale(1.0f);
	}
	else if (mType == TYPE_OPTION) {
		mFont->SetScale(0.75f);

		if (mHasFocus)
		{
			mFont->SetColor(ARGB(255,0,128,255));
			mFont->DrawShadowedString(mText, x, y, mAlign);
			float w = mFont->GetStringWidth(mText);
			mRenderer->DrawLine(x,y+15,x+w,y+15,ARGB(255,0,128,255));
			mRenderer->DrawLine(x+1,y+15+1,x+w+1,y+15+1,ARGB(200,0,0,0));
			//mEngine->DrawLine(
		}
		else
		{
			mFont->SetColor(ARGB(255,255,255,255));
			mFont->DrawShadowedString(mText, x, y, mAlign);
		}

		mFont->SetScale(1.0f);
	}
	else if (mType == TYPE_BUTTON) {
		mFont->SetScale(0.75f);
		mFont->SetColor(ARGB(255,255,200,0));
		if (mHasFocus) {
			mRenderer->FillRect(x-5,y-3,150,20,ARGB(128,255,200,0));
			mFont->DrawString(mText, x, y, mAlign);
		}
		else {
			mRenderer->FillRect(x-5,y-3,150,20,ARGB(128,0,0,0));
			mFont->DrawString(mText, x, y, mAlign);
		}
		mRenderer->DrawLine(x-5,y-3,x+145,y-3,ARGB(128,255,200,0));
		mRenderer->DrawLine(x+145,y-3,x+145,y+17,ARGB(128,255,200,0));
		mRenderer->DrawLine(x+145,y+17,x-5,y+17,ARGB(128,255,200,0));
		mRenderer->DrawLine(x-5,y+17,x-5,y-3,ARGB(128,255,200,0));
	}
}

void MenuItem::Update(float dt)
{
	/*if (mType == TYPE_MAIN) {
		if (mScale < mTargetScale)
		{
			mScale += 0.0008f*dt;
			if (mScale > mTargetScale)
				mScale = mTargetScale;
		}
		else if (mScale > mTargetScale)
		{
			mScale -= 0.0008f*dt;
			if (mScale < mTargetScale)
				mScale = mTargetScale;
		}
	}*/
}

void MenuItem::Entering()
{
	mHasFocus = true;
	mTargetScale = 1.2f;
}


bool MenuItem::Leaving(u32 key)
{
	mHasFocus = false;
	mTargetScale = 1.0f;
	return true;
}


bool MenuItem::ButtonPressed()
{
	return true;
}
