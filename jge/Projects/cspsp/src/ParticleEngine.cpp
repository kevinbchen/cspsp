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


#include <math.h>

#include "ParticleEngine.h"


ParticleEngine::ParticleEngine(int maxParticles): mMaxParticles(maxParticles)
{
	mTimer = 0.0f;
	//mPos = Vector2D(0.0f, 0.0f);

	//mScale = 0.2f;
	//mRotation = 0.0f;
	//mDeltaScale= 0.01f;
	//mDeltaRotation = 0.01f;

	//additiveRender = true;
	/**
	mCirular = true;
	mRadius = 50;

	mEmissionRate = 100.0f;	// 300 particles/second
	mTimeToEmit = 1000.0f / mEmissionRate;
	mTimer = 0;
	**/
	mIndex = 0;

//	mIdleList = NULL;
//	mActiveList = NULL;

	//mActive = false;

	mParticles = new Particle*[mMaxParticles];
	for (int i=0;i<mMaxParticles;i++)
		mParticles[i] = new Particle();

}


ParticleEngine::~ParticleEngine()
{
	
	for (int i=0;i<mMaxParticles;i++)
		if (mParticles[i] != NULL)
			delete mParticles[i];

	delete[] mParticles;

//	Particle *head;
//	//head = idleList;
//	while (mIdleList != NULL)
//	{
//		head = mIdleList;
//		mIdleList = mIdleList->mNext;
//		head->mPrev = NULL;
//		head->mNext = NULL;
//		delete head;
//		//head = idleList;
//	}
//	//head = activeList;
//	while (mActiveList != NULL)
//	{
//		head = mActiveList;
//		mActiveList = mActiveList->mNext;
//		head->mPrev = NULL;
//		head->mNext = NULL;
//		delete head;
//		//head = activeList;
//	}
}

void ParticleEngine::GenerateParticles(int type, float x, float y, int amount) {

	for (int i=0;i<amount;i++) {
		Particle *temp = mParticles[mIndex];

	//			if (mCount < MAX_PARTICLES)
		if (temp != NULL && !temp->mActive)
		{
			mTimer = 0.0f;
			temp->mActive = true;
			mIndex ++;
			if (mIndex>mMaxParticles-1) {
				mIndex = 0;
			}
					
			temp->SetPosition(x, y);
			temp->SetQuad(mQuad);
			temp->mSpeed = abs(rand()%50)/1000.0f;
			temp->mAngle = abs(rand()%360)*DEG2RAD;
			//mQuad->mBlend = GU_TFX_MODULATE;
			if (type == BULLETIMPACT) {
				//temp->ResetVelocity();
				temp->mCircularAcceleration = -0.004f;
							
				float lifetime = 250.0f;
				temp->mLifetime = lifetime;
				temp->mScale = 0.3;
				temp->mScaleEnd = 0.1f;
				temp->mScaleDelta = -0.001f;//(0.1f-temp->mScale)/lifetime;
				temp->mRotation = 0.0f;
				temp->mRotationDelta = 0.002f;
				temp->mAlpha = 255.0f;
				temp->mAlphaEnd = 0.0f;
				temp->mAlphaDelta = -0.2f;//.0f/lifetime);
						
				//float r = 0.0f;//abs(RAND%256);
				//float g = 0.0f;//213.1f;//abs(RAND%256);
				//float b = 0.0f;//55.0f;//abs(RAND%256);
				//temp->mColor.a = 255.0f;
				temp->mColor[0] = 0.0f;
				temp->mColor[1] = 0.0f;
				temp->mColor[2] = 0.0f;

				//r = 255.0f;
				//g = 128.0f;
				//b = 35.0f;
				//temp->mColorEnd.a= 255.0f;
				temp->mColorEnd[0] = 255.0f;
				temp->mColorEnd[1] = 128.0f;;
				temp->mColorEnd[2] = 35.0f;
			}
			else if (type == BLOOD) {
				//temp->ResetVelocity();
				temp->mCircularAcceleration = -0.004f;
							
				float lifetime = 200.0f;
				temp->mLifetime = lifetime;
				temp->mScale = 0.5f;
				temp->mScaleEnd = 0.3f;
				temp->mScaleDelta = -0.001f;//(0.1f-temp->mScale)/lifetime;
				temp->mRotation = 0.0f;
				temp->mRotationDelta = 0.002f;
				temp->mAlpha = 255.0f;
				temp->mAlphaEnd = 0.0f;
				temp->mAlphaDelta = -0.3f;//.0f/lifetime);
						
				//float r = 0.0f;//abs(RAND%256);
				//float g = 0.0f;//213.1f;//abs(RAND%256);
				//float b = 0.0f;//55.0f;//abs(RAND%256);
				//temp->mColor.a = 255.0f;
				temp->mColor[0] = 128.0f;
				temp->mColor[1] = 0.0f;
				temp->mColor[2] = 0.0f;

				//r = 255.0f;
				//g = 128.0f;
				//b = 35.0f;
				//temp->mColorEnd.a= 255.0f;
				temp->mColorEnd[0] = 255.0f;
				temp->mColorEnd[1] = 128.0f;;
				temp->mColorEnd[2] = 35.0f;
			}
			else if (type == BULLETSHELL) {
				//temp->ResetVelocity();
				temp->mCircularAcceleration = -0.004f;
							
				float lifetime = 500.0f;
				temp->mLifetime = lifetime;
				temp->mScale = 0.1f;
				temp->mScaleEnd = 0.1f;
				temp->mScaleDelta = 0;//(0.1f-temp->mScale)/lifetime;
				temp->mRotation = 0.0f;
				temp->mRotationDelta = 0.002f;
				temp->mAlpha = 255.0f;
				temp->mAlphaEnd = 0.0f;
				temp->mAlphaDelta = -0.01f;//.0f/lifetime);
						
				//float r = 0.0f;//abs(RAND%256);
				//float g = 0.0f;//213.1f;//abs(RAND%256);
				//float b = 0.0f;//55.0f;//abs(RAND%256);
				//temp->mColor.a = 255.0f;
				temp->mColor[0] = 255.0f;
				temp->mColor[1] = 255.0f;
				temp->mColor[2] = 0.0f;

				//r = 255.0f;
				//g = 128.0f;
				//b = 35.0f;
				//temp->mColorEnd.a= 255.0f;
				temp->mColorEnd[0] = 255.0f;
				temp->mColorEnd[1] = 255.0f;;
				temp->mColorEnd[2] = 0.0f;
			}						
			for (int i=0;i<3;i++) {
				temp->mColorDelta[i] = (temp->mColorEnd[i]-temp->mColor[i])/3000.0f;
			}
		}
	}
}

void ParticleEngine::Update(float dt)
{
//	int i;

	mTimer += dt;
	//if (mTimer > mTimeToEmit)
	//{
			//bool found = false;
			//for (i=0;i<MAX_PARTICLES && !found;i++)



		
	//}

	//mCount = 0;
	for (int i=0;i<mMaxParticles;i++)
	{
		if (mParticles[i]->mActive)
		{
			mParticles[i]->Update(dt);
			//mCount++;
		}
	}

//	Particle *head = mActiveList;
//	
//	while (head != NULL)
//	{
//		//if (particles[i]->active)
//		//	particles[i]->update(dTime);
//		if (!head->Update(dt))
//		{
//			Particle *prev;
//			Particle *next;
//			prev = head->mPrev;
//			next = head->mNext;
//
//			head->mPrev = NULL;
//			head->mNext = mIdleList;
//			if (mIdleList != NULL)
//				mIdleList->mPrev = head;
//			mIdleList = head;
//
//			if (prev != NULL)
//				prev->mNext = next;
//			if (next != NULL)
//				next->mPrev = prev;
//			
//			head = next;
//		}
//		else
//			head = head->mNext;
//	}
	
}


void ParticleEngine::Render(float x, float y)
{


	//Particle *head = mActiveList;
	
	//while (head != NULL)
	for (int i=0;i<mMaxParticles;i++)
	{
		if (mParticles[i]->mActive)
		{
			mParticles[i]->Render(x, y);
			//head = head->mNext;
		}
	}

//	for (int i=0;i<count;i++)
//	{
//		if (particles[i]->active)
//			particles[i]->render(g);
//	}


}


void ParticleEngine::SetPosition(float x, float y)
{
	mPos = Vector2D(x, y);
}


void ParticleEngine::SetQuad(JQuad *quad) 
{ 
	mQuad = quad; 
}
