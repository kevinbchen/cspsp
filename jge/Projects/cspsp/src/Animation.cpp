#include "Animation.h"

Animation::Animation(KeyFrameAnim* keyframeanim, bool isplaying, bool loop)//JQuad* texture, float x, float y)
{
	mKeyFrameAnim = keyframeanim;
	mIsPlaying = isplaying;
	mLoop = loop;
	mTimer = 0;
	mIndex = 0;
	mSpeed = 1.0f;
	//strcpy(mName,"blank");
	//this->joints = joints;
}
	

Animation::~Animation()
{

}


void Animation::Update(float dt, KeyFrame& frame)
{
	if (mIsPlaying) {
		dt *= mSpeed;
		if (dt > mKeyFrameAnim->mKeyFrames[mIndex].duration) {
			dt = mKeyFrameAnim->mKeyFrames[mIndex].duration;
		}
		mTimer += dt;

		if (mTimer > mKeyFrameAnim->mKeyFrames[mIndex].duration) {
			mTimer = mTimer - mKeyFrameAnim->mKeyFrames[mIndex].duration;
			mIndex++;
			if (mIndex >= mKeyFrameAnim->mKeyFrames.size()) {
				if (mLoop) {
					mIndex = 0;
				}
				else {
					mIndex = mKeyFrameAnim->mKeyFrames.size()-1;
					mIsPlaying = false;
				}
			}
		}

		float timeleft = mKeyFrameAnim->mKeyFrames[mIndex].duration - mTimer;
		if (timeleft <= dt) {
			timeleft = dt;
		}
		
		for (int i=0; i<6; i++) {
			float targetangle = mKeyFrameAnim->mKeyFrames[mIndex].angles[i];
			float diffangle = targetangle-frame.angles[i];
			if (diffangle > M_PI) {
				diffangle -= 2*M_PI;
			}
			if (diffangle < -M_PI) {
				diffangle += 2*M_PI;
			}

			float add = diffangle*(dt/timeleft);

			if (diffangle >= 0) {
				if (diffangle < add) {
					frame.angles[i] = targetangle;
				}
				else {
					frame.angles[i] += add;
				}
			}
			else {
				if (diffangle > add) {
					frame.angles[i] = targetangle;
				}
				else {
					frame.angles[i] += add;
				}
			}
		}
	}
	else {
	}
}


void Animation::Add(KeyFrame keyframe) {
	mKeyFrameAnim->mKeyFrames.push_back(keyframe);
}

void Animation::Reset() {
	mIsPlaying = false;
	mTimer = 0;
	//mSpeed = 1.0f;
}

void Animation::SetSpeed(float speed) {
	mSpeed = speed;
}

KeyFrameAnim* Animation::LoadKeyFrames(char* name) {
	KeyFrameAnim* anim = new KeyFrameAnim();

    FILE *file;

	file = fopen("data/animations.txt", "r"); 

	char buffer[256];
	float duration;
	char line[4096];
	char* s;
	float angles[6];

	bool isReading = false;
	while(fgets(line,4096,file)) {

		if (sscanf(line,"%s %*s",buffer) != EOF) {
			if (strcmp(buffer,name) == 0) {
				isReading = true;
				continue;
			}
		}

		if (isReading) {
			if (sscanf(line,"%s",buffer) != EOF) {
				if (strncmp(buffer,"}",1) == 0) {
					isReading = false;
					break;
				}
			}
			if (sscanf(line,"%f %f %f %f %f %f %f",&duration,&angles[0],&angles[1],&angles[2],&angles[3],&angles[4],&angles[5]) != EOF) {
				KeyFrame keyframe = keyframe = KeyFrame();
				keyframe.duration = duration;

				for (int i=0; i<6; i++) {
					angles[i] *= M_PI/180.0f;
					if (angles[i] < 0.0f) {
						angles[i] += 2*M_PI;
					}
					keyframe.angles[i] = angles[i];
				}

				/*while(fgets(line,4096,file)) {
					sscanf(line,"%s",buffer);
					if (strncmp(buffer,"}",1) == 0) {
						break;
					}
					sscanf(buffer,"(%d,%f)", &p, &theta);
					theta *= M_PI/180.0f;
					if (theta < 0.0f) {
						theta += 2*M_PI;
					}
					keyframe->angles[p] = theta;
				}*/
				anim->mKeyFrames.push_back(keyframe);
			}		
		}
	}

	fclose(file);

	return anim;
}

void Animation::Play() {
	if (!mIsPlaying) {
		Reset();
		mIndex = 0;
		mIsPlaying = true;
	}
}

int Animation::GetIndex() {
	return mIndex;
}

KeyFrame* Animation::GetKeyFrame(int index) {
	return &(mKeyFrameAnim->mKeyFrames[index]);
}