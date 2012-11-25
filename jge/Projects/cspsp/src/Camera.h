#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "JGE.h"
#include "Collision.h"

#ifdef WIN32
#include <math.h>
#else
#include <fastmath.h>
#endif

class Camera
{
private:


public:
	float mX;
	float mY;
	float mTX;
	float mTY;
	float mA;
	float mMinX;
	float mMinY;
	float mMaxX;
	float mMaxY;

	float mShakeX;
	float mShakeY;
	int mShakeMagnitude;
	float mShakeTime;
	float mShakeTimeTotal;
	float mShakeFreqX;
	float mShakeFreqY;

	bool mRecoilState;
	float mRecoilAngleCos;
	float mRecoilAngleSin;
	float mRecoilMagnitude;
	float mRecoilMagnitudeTotal;
	float mRecoilTime;
	float mRecoilTimeTotal;

	Camera(float x, float y, float minx = 0, float miny = 0, float maxx = 480, float maxy = 272);
	~Camera();

	void Update(float dt);
	float GetX();
	float GetY();
	void SetTX(float tx);
	void SetTY(float ty);
	void SetTPos(float tx, float ty);
	void SetBounds(float minx, float miny, float maxx, float maxy);
	void Shake(int magnitude, float time);
	void Recoil(float angle, float magnitude, float time);
};


#endif
