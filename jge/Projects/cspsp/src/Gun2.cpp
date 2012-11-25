
#include "Gun.h"
JSoundSystem* Gun::mSoundSystem = NULL;

//------------------------------------------------------------------------------------------------
Gun::Gun(JQuad *handquad, JQuad *groundquad, int id, int delay, int damage, float spread, int clip, int numclips, int reloaddelay, float speed, int cost, int type, char* name)
{
	mSoundSystem = JSoundSystem::GetInstance();
	mHandQuad = handquad;
	mGroundQuad = groundquad;
	mId = id;
	mDelay = delay;
	mDamage = damage;
	mSpread = spread;
	mClip = clip;
	mNumClips = numclips;
	mReloadDelay = reloaddelay;
	mSpeed = speed;
	mCost = cost;
	mType = type;
	strcpy(mName,name);
	char fire[25];
	strcpy(fire,"sfx/");
	strcat(fire,name);
	strcat(fire,".wav");	

	char reload[25];
	strcpy(reload,"sfx/");
	strcat(reload,name);
	strcat(reload,"reload.wav");	

	mFireSound = mSoundSystem->LoadSample(fire);
	mReloadSound = mSoundSystem->LoadSample(reload);

}

//------------------------------------------------------------------------------------------------
Gun::~Gun()
{
	SAFE_DELETE(mFireSound);
	SAFE_DELETE(mReloadSound);
}

//------------------------------------------------------------------------------------------------
void Gun::SetQuad(JQuad *quad) 
{ 
	mHandQuad = quad; 
}
