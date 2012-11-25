
#include "BulletOnline.h"

//------------------------------------------------------------------------------------------------
BulletOnline::BulletOnline(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent) : Bullet(x, y, px, py, angle, speed, damage, parent)
{
	mSX = x;
	mSY = y;
}

//------------------------------------------------------------------------------------------------
BulletOnline::~BulletOnline()
{
}


//------------------------------------------------------------------------------------------------
void BulletOnline::Update(float dt)
{
	if (mState != 0) {
		mState++;
		if (mState == BULLET_DEAD) mState = BULLET_DEAD;
		return;
	}
	mSX += cosAngle*mSpeed*dt;
	mSY += sinAngle*mSpeed*dt;

	if (mIsFirstUpdate) {
		mIsFirstUpdate = false;
	}
	else {
		pX = mX;
		pY = mY;
	}
	//use a ray to prevent missing a collision? (nvm fixed)
	if (mSX > mX) {
		mX += cosAngle*mSpeed*2.0f*dt;
		if (mX > mSX) {
			mX = mSX;
		}
	}
	else if (mSX < mX) {
		mX += cosAngle*mSpeed*2.0f*dt;
		if (mX < mSX) {
			mX = mSX;
		}
	}

	if (mSY > mY) {
		mY += sinAngle*mSpeed*2.0f*dt;
		if (mY > mSY) {
			mY = mSY;
		}
	}
	else if (mSY < mY) {
		mY += sinAngle*mSpeed*2.0f*dt;
		if (mY < mSY) {
			mY = mSY;
		}
	}
}



//------------------------------------------------------------------------------------------------
void BulletOnline::Render(float x, float y)
{
	/*float offsetX = (x-SCREEN_WIDTH_2);
	float offsetY = (y-SCREEN_HEIGHT_2);
	mEngine->DrawLine((mX-21*cosf(mAngle))-offsetX,(mY-21*sinf(mAngle))-offsetY,mX-offsetX, mY-offsetY, ARGB(50,255,240,0));
	mEngine->DrawLine((mX-14*cosf(mAngle))-offsetX,(mY-14*sinf(mAngle))-offsetY,mX-offsetX, mY-offsetY, ARGB(100,255,240,0));
	mEngine->DrawLine((mX-7*cosf(mAngle))-offsetX,(mY-7*sinf(mAngle))-offsetY,mX-offsetX, mY-offsetY, ARGB(200,255,240,0));
	mEngine->FillRect(mX-offsetX, mY-offsetY-1, 1, 1, ARGB(255,255,240,0));*/
	Bullet::Render(x,y);
}



//------------------------------------------------------------------------------------------------
void BulletOnline::AddLatency(float latency)
{
	mSX += mSpeed*cosAngle*latency;
	mSY += mSpeed*sinAngle*latency;
	//mUpdateCounter += 0.15f*latency/16.6f;
	//if (mUpdateCounter > 4) mUpdateCounter = 4;
}
