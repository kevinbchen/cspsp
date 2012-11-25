#include "Camera.h"

Camera::Camera(float x, float y, float minx, float miny, float maxx, float maxy)
{
	mTX = x;
	mTY = y;
	mX = x;
	mY = y;
	SetBounds(minx,miny,maxx,maxy);

	mA = 0.005f;

	mShakeX = 0.0f;
	mShakeY = 0.0f;
	mShakeMagnitude = 0;
	mShakeTime = 0.0f;
	mShakeTimeTotal = 0.0f;
	mShakeFreqX = 0.1f;
	mShakeFreqY = 0.1f;

	mRecoilState = false;
	mRecoilAngleCos = 0.0f;
	mRecoilAngleSin = 0.0f;
	mRecoilMagnitude = 0.0f;
	mRecoilMagnitudeTotal = 0.0f;
	mRecoilTime = 0.0f;
	mRecoilTimeTotal = 0.0f;
}
	

Camera::~Camera()
{
}


void Camera::Update(float dt)
{
	float dx = fabs(mTX-mX);
	float dy = fabs(mTY-mY);
	float theta = 0.0f;
	if (dx >= EPSILON || dy >= EPSILON) {
		theta = atan2f(dy,dx);
	}

	if (dx > 2) {
		if (mTX > mX) {
			mX += dx*cosf(theta)*mA*dt;
			if (mTX < mX) {
				mX = mTX;
			}
		}
		else if (mTX < mX) {
			mX -= dx*cosf(theta)*mA*dt;
			if (mTX > mX) {
				mX = mTX;
			}
		}
	}

	if (dy > 2) {
		if (mTY > mY) {
			mY += dy*sinf(theta)*mA*dt;
			if (mTY < mY) {
				mY = mTY;
			}
		}
		else if (mTY < mY) {
			mY -= dy*sinf(theta)*mA*dt;
			if (mTY > mY) {
				mY = mTY;
			}
		}
	}

	if (mShakeTimeTotal > 0.0f && mShakeMagnitude > 0) {
		float factor = 1.0f-mShakeTime/mShakeTimeTotal;
		//if (factor < 0.1f) factor = 0.1f;

		mShakeX = factor*mShakeMagnitude*sinf(mShakeTime*mShakeFreqX);//(rand()%mShakeMagnitude-mShakeMagnitude/2)*factor;
		mShakeY = factor*mShakeMagnitude*sinf(mShakeTime*mShakeFreqY);//(rand()%mShakeMagnitude-mShakeMagnitude/2)*factor;
		mShakeTime += dt;
		if (mShakeTime >= mShakeTimeTotal) {
			mShakeTime = 0.0f;
			mShakeTimeTotal = 0.0f;
			mShakeX = 0.0f;
			mShakeY = 0.0f;
		}
	}

	if (mRecoilTime > 0.0f) {
		if (mRecoilState) {
			mRecoilMagnitude += 0.5*dt;
			if (mRecoilMagnitude >= mRecoilMagnitudeTotal) {
				mRecoilMagnitude = mRecoilMagnitudeTotal;
				mRecoilState = false;
			}
		}
		else {
			float factor = mRecoilTime/mRecoilTimeTotal;
			//if (factor < 0.1f) factor = 0.1f;

			mRecoilMagnitude = factor*mRecoilMagnitudeTotal;
			mRecoilTime -= dt;
			if (mRecoilTime < 0.0f) {
				mRecoilTime = 0.0f;
				mRecoilTimeTotal = 0.0f;
			}
		}
	}


	if (mX < mMinX) {
		mX = mMinX;
	}
	else if (mX > mMaxX) {
		mX = mMaxX;
	}
	if (mY < mMinY) {
		mY = mMinY;
	}
	else if (mY > mMaxY) {
		mY = mMaxY;
	}
	//mX += (mTX-mX)*mA*dt;
	//mY += (mTY-mY)*mA*dt;
}

float Camera::GetX()
{
	return mX+mShakeX-mRecoilMagnitude*mRecoilAngleCos;
}

float Camera::GetY()
{
	return mY+mShakeY-mRecoilMagnitude*mRecoilAngleSin;
}

void Camera::SetTX(float tx)
{
	if (tx < mMinX) {
		tx = mMinX;
	}
	else if (tx > mMaxX) {
		tx = mMaxX;
	}
	mTX = tx;
}

void Camera::SetTY(float ty)
{
	if (ty < mMinY) {
		ty = mMinY;
	}
	else if (ty > mMaxY) {
		ty = mMaxY;
	}
	mTY = ty;
}

void Camera::SetTPos(float tx, float ty)
{
	SetTX(tx);
	SetTY(ty);
}

void Camera::SetBounds(float minx, float miny, float maxx, float maxy) {
	mMinX = minx;
	mMinY = miny;
	mMaxX = maxx;
	mMaxY = maxy;
}

void Camera::Shake(int magnitude, float time) {
	mShakeMagnitude = magnitude;
	mShakeTime = 0.0f;
	mShakeTimeTotal = time;
	mShakeX = 0;
	mShakeY = 0;
	mShakeFreqX = (rand()%40-20)*0.001f + 0.05f;
	mShakeFreqY = (rand()%40-20)*0.001f + 0.05f;
}

void Camera::Recoil(float angle, float magnitude, float time) {
	mRecoilState = true;
	mRecoilAngleCos = cosf(angle);
	mRecoilAngleSin = sinf(angle);
	//mRecoilMagnitude = 0.0f; //keep what it is now.
	mRecoilMagnitudeTotal = magnitude;
	mRecoilTime = time;
	mRecoilTimeTotal = time;
}

