//-------------------------------------------------------------------------------------
//
// This is part of JGE++, a hardware accelerated 2D game engine for PSP/Windows.
// 
// Copyright (C) 2006 James Hui (a.k.a. Dr.Watson)
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


#ifndef __PARTICLE1_H__
#define __PARTICLE1_H__

#include "JRenderer.h"
#include "Vector2D.h"

class Particle
{
private:
	static JRenderer* mRenderer;

public:
	Particle *mPrev;
	Particle *mNext;
	
	bool mActive;
	
	Vector2D mPos;
	
	//Image *texture;	
	JQuad* mQuad;

	float mLifetime;
	
	//Vector2D mVelocity;
	Vector2D mGravity;
	
	//float radialAcceleration;	
	float mCircularAcceleration;

	float mAngle;
	float mSpeed;

	float mScale;			// Defines the starting and ending size of a specific particle and it's starting size variation. Note that bigger particles require more time to render. 
	float mScaleEnd;
	float mScaleDelta;

	float mRotation;
	//float rotationEnd;
	float mRotationDelta;
	float mAlpha;			// Defines the starting and ending opacity of a specific particle and it's starting opacity variation. 
	float mAlphaEnd;
	float mAlphaDelta;
	//Color colorStart;
	//Color colorEnd;
	//Color colorDelta;
	float mColor[3];		// ARGB as in array 0,1,2,3 respectively
	float mColorEnd[3];
	float mColorDelta[3];


	Particle();//JQuad* texture, float x , float y);
	~Particle();

	bool Update(float dtt);
	void Render(float x, float y);
	void ResetVelocity();

	void SetPosition(float x, float y);
	void SetQuad(JQuad *quad);

};


#endif
