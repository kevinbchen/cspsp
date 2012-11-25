#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <vector>
#include "JGE.h"

struct KeyFrame {
	float angles[6];
	float duration;
};
struct KeyFrameAnim {
	char mName[64];
    std::vector<KeyFrame> mKeyFrames;
};

class Animation
{
private:
	KeyFrameAnim* mKeyFrameAnim;
	float mTimer;
	int mIndex;
	float mSpeed;
	bool mLoop;
	
public:
	bool mIsPlaying;

	Animation(KeyFrameAnim* mKeyFrameAnim, bool isplaying = true, bool loop = true);
	~Animation();

	void Update(float dt, KeyFrame& frame);
	//void UpdateReset(std::vector<Particle*>* joints);

	void Add(KeyFrame keyframe);
	void Reset();
	void SetSpeed(float speed);
	void Play();
	int GetIndex();

	KeyFrame* GetKeyFrame(int index);

	static KeyFrameAnim* LoadKeyFrames(char* name);
};


#endif
