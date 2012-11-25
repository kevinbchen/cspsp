
#include "SfxManager.h"
JSoundSystem* SfxManager::mSoundSystem = NULL;

//------------------------------------------------------------------------------------------------
SfxManager::SfxManager()
{
	mSoundSystem = JSoundSystem::GetInstance();
	mX = 0;
	mY = 0;
}

//------------------------------------------------------------------------------------------------
SfxManager::~SfxManager()
{
}

//------------------------------------------------------------------------------------------------
int SfxManager::PlaySample(JSample *sample)
{
	sample->mVolume = 256;
	//sample->mVolume *= 255*powf((1*1)/(1*1+distance*distance),0.5);
	sample->mPanning = 127;

	mSoundSystem->PlaySample(sample);
	return sample->mVoice;
}

//------------------------------------------------------------------------------------------------
int SfxManager::PlaySample(JSample *sample, float x, float y)
{
	//sample = test;
	float dx = mX-x;
	float dy = mY-y;
	float distance = sqrtf(dx*dx+dy*dy);
	if (distance <= 500) {
		sample->mVolume = (int)(256-distance*(256.0f/500.0f));
		//sample->mVolume *= 255*powf((1*1)/(1*1+distance*distance),0.5);
		if (distance > 5) {
			sample->mPanning = (int)(127.5f-(127.5f*(dx/distance)));
		}
		else {
			sample->mPanning = 127;
		}
		mSoundSystem->PlaySample(sample);
		return sample->mVoice;
	}	
	return -1;
}

