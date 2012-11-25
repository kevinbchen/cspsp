//-------------------------------------------------------------------------------------
//
// JGE is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#include "../include/JSoundSystem.h"
#include "../include/JAudio.h"
#include "../include/JCooleyesMP3.h"


#ifdef __cplusplus
extern "C" {
#endif


// Mikmod specific variables
extern int _mm_errno;
extern BOOL _mm_critical;
extern char *_mm_errmsg[];
extern UWORD md_mode;
extern UBYTE md_reverb;
extern UBYTE md_pansep;

#ifdef __cplusplus
}
#endif

void my_error_handler(void)
{
//	printf("_mm_critical %d\n", _mm_critical);
//	printf("_mm_errno %d\n", _mm_errno);
//	printf("%s\n", _mm_errmsg[_mm_errno]);
	return;
}

/////////////////////

JMusic::JMusic()
{
	mTrack = NULL;
}

JMusic::~JMusic()
{
	JSoundSystem::GetInstance()->StopMusic(this);

	if (mTrack)
		delete mTrack;
}

JSample::JSample()
{
	mSample = NULL;
	mVoice = -1;
	mVolume = 255;
	mPanning = 127;
}


JSample::~JSample()
{
	/*if (mSample)
		releaseWaveData(mSample);*/

	if (mSample)
		WAV_Free(mSample);

}

JSoundSystem* JSoundSystem::mInstance = NULL;

JSoundSystem* JSoundSystem::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JSoundSystem();
		mInstance->InitSoundSystem();
	}
	
	return mInstance;
}


void JSoundSystem::Destroy()
{
	if (mInstance)
	{
		mInstance->DestroySoundSystem();
		delete mInstance;
		mInstance = NULL;
	}
}


JSoundSystem::JSoundSystem()
{

}


JSoundSystem::~JSoundSystem()
{

}


void JSoundSystem::InitSoundSystem()
{

	//audioInit();

	//mMaxChannel = MAX_CHANNEL;

	_mm_RegisterErrorHandler(my_error_handler);
	MikMod_RegisterAllLoaders();
	MikMod_RegisterAllDrivers();
	md_mode = DMODE_16BITS|DMODE_STEREO|DMODE_SOFT_SNDFX|DMODE_SOFT_MUSIC; 
	md_reverb = 0;
	md_pansep = 128;
	MikMod_Init();
	MikMod_SetNumVoices(-1, 32);

	//pspAudioSetChannelCallback(2, MP3AudioOutCallback, NULL);
	//InitMP3Decoder();

	Player_Start(NULL);
}


void JSoundSystem::DestroySoundSystem()
{
	//audioDestroy();

	//ReleaseMP3Decoder();

	//Player_Stop();
	MikMod_Exit();
}


JMusic *JSoundSystem::LoadMusic(const char *fileName)
{
	//char s[strlen(fileName)+1];
	//strcpy(s, fileName);

	JMusic *music = new JMusic();
	if (music)
	{
		music->mTrack = new JCooleyesMP3();
		if (music->mTrack)
			music->mTrack->Load(fileName);
	}

	return music;
}

// void JSoundSystem::FreeMusic(JMusic *music)
// {
// 	if (music)
// 	{
// 		if (music->mTrack)
// 			delete music->mTrack;
// 
// 	}
// }

JSample *JSoundSystem::LoadSample(const char *fileName)
{
	/*char s[strlen(fileName)+1];
	strcpy(s, fileName);

	JSample *sample = new JSample();
	if (sample)
	{
		sample->mSample = new WAVDATA;
		loadWaveData(sample->mSample, s, 1);
	}

	return sample;*/

	JSample *sample = new JSample();
	if (sample)
		sample->mSample = WAV_LoadFN((char*)fileName);

	return sample;
}

// 
// void JSoundSystem::FreeMusic(JMusic *music)
// {
// 	if (music)
// 	{
// 		if (music->mTrack)
// 		{
// 			music->mTrack->Release();
// 			delete music->mTrack;
// 		}
// 		delete music;
// 	}
// }


// void JSoundSystem::FreeSample(JSample *sample)
// {
// 	if (sample)
// 	{
// 		releaseWaveData(sample->mSample);
// 		delete sample;
// 	}
// }


void JSoundSystem::PlayMusic(JMusic *music, bool looping)
{

	if (music->mTrack)
		PlayMP3(music->mTrack, looping);
}


void JSoundSystem::PlaySample(JSample *sample)
{

	//playWaveMem(sample->mSample, 0);

	sample->mVoice = MikMod_PlaySample(sample->mSample, 0, 0);
	Voice_SetVolume(sample->mVoice,sample->mVolume);
	Voice_SetPanning(sample->mVoice,sample->mPanning);
	//return sample->mVoice;
}

void JSoundSystem::StopSample(int voice)
{
	Voice_SetVolume(voice,0);
}

void JSoundSystem::SetVolume(int volume)
{

	mVolume = volume;
}


void JSoundSystem::StopMusic(JMusic *music)
{
	StopMP3();

}


void JSoundSystem::ResumeMusic(JMusic *music)
{
	ResumeMP3();

}

