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


#ifndef __PARTICLE_SYSTEM1_H__
#define __PARTICLE_SYSTEM1_H__

#include "JGE.h"
#include "Particle.h"

//#define MAX_PARTICLES	500

#define RAD2DEG		57.29577951f
#define DEG2RAD		0.017453293f
#define BULLETIMPACT 0
#define BLOOD 1
#define BULLETSHELL 2

class ParticleEngine
{
private:
	Vector2D mPos;
	
	//Particle *mIdleList;		// wow, link list!
	//Particle *mActiveList;
	
	int mMaxParticles;
	Particle **mParticles;

	bool mCirular;
	int mRadius;
	int mMaxSpeed;
	
	float mScale;
	float mRotation;
	float mDeltaScale;
	float mDeltaRotation;
	
	float mParticleLifetimeMin;	// Defines the period of time during which a specific particle is alive. 
	float mParticleLifetimeMax;
	bool mAdditiveRender;		// Specifies the particles' blending mode: additive - a particle color is added to the graphics already on the screen resulting in lighting effect, blend - a particle color is blended with the graphics already on the screen. 
	float mEmissionDirection;	// This is an angle, defining particles emitting direction. If the relative check box is activated - the angle is treated relative to the current particle system's movement direction. 
	float mEmissionSpread;		// This angle defines the sector within which the particles are emitted. For example, if it is 40, the emitted particles direction lies within Direction-20 and Direction+20. If the Spread is 360, the Direction parameter becomes meaningless as the whole circle is covered. 
	float mStartSpeedMin;			// Defines the range of starting speed for the particles. 
	float mStartSpeedMax;
	float mGravityMin;					// Defines the range of gravity values for the particles (basically, it is just an acceleration along the Y-axis). 
	float mGravityMax;
	float mRadialAccelerationMin;		// Defines the range of values for the acceleration directed towards the particle system's center. Use this to create particle system that attracts the particles to it's center. 
	float mRadialAccelerationMax;
	float mTangentialAccelerationMin;	//Defines the range of values for the acceleration directed perpendicularly to the radial acceleration. Use this to make particles rotate around the particle system's center. 
	float mTangentialAccelerationMax;

	float mParticleSizeStart;	// Defines the starting and ending size of a specific particle and it's starting size variation. Note that bigger particles require more time to render. 
	float mParticleSizeEnd;
	float mParticleSizeDelta;
	float mParticleSpinStart;	// This parameter defines the speed of a specific particle rotation around it's anchor point. Set the starting and ending rotation speed and it's starting rotation speed variation. 
	float mParticleSpinEnd;
	float mParticleSpinDelta;
	float mParticleAlphaStart;	// Defines the starting and ending opacity of a specific particle and it's starting opacity variation. 
	float mParticleAlphaEnd;
	float mParticleAlphaAlpha;
	float mParticleColorStart[4];	// ARGB as in array 0,1,2,3 respectively
	float mParticleColorEnd[4];
	float mParticleColorDelta[4];


	//Image *texture;

public:
	bool mActive;
	int mCount;
	int mIndex;
	float mTimer;
	JQuad* mQuad;

	
	ParticleEngine(int maxParticles);

	~ParticleEngine();

	void Update(float dt);
	void Render(float x, float y);

	void GenerateParticles(int type, float x, float y, int amount);

	void SetPosition(float, float);
	
	void SetQuad(JQuad *quad);

};



#endif
