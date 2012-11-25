#ifndef _SFXMANAGER_H_
#define _SFXMANAGER_H_

#include "JSoundSystem.h"
#include "JGE.h"
//------------------------------------------------------------------------------------------------
class SfxManager
{
private:
	static JSoundSystem* mSoundSystem;

protected:

public:
	float mX;
	float mY;


	SfxManager();
	~SfxManager();
	int PlaySample(JSample *sample);
	int PlaySample(JSample *sample, float x, float y);
	//Voice_Stop(wavfile->voice);
};

#endif
