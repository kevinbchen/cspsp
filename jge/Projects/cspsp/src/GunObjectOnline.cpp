
#include "GunObjectOnline.h"

//------------------------------------------------------------------------------------------------
GunObjectOnline::GunObjectOnline(Gun *gun, int clipammo, int remainingammo): GunObject(gun, clipammo, remainingammo)
{
	mSX = 0;
	mSY = 0;
	mSAngle = 0.0f;
	mIsOnline = true;
}

//------------------------------------------------------------------------------------------------
GunObjectOnline::~GunObjectOnline()
{

}

//------------------------------------------------------------------------------------------------
void GunObjectOnline::Update(float dt) 
{ 
	GunObject::Update(dt);

	mAngle = mSAngle;

	if (!mOnGround) {
		mSX += mSpeed * cosf(mAngle) * dt;
		mSY += mSpeed * sinf(mAngle) * dt;
	}

	//if (!mOnGround) {
		if (fabs(mSX-mX) > 40) {
			mX = mSX;
		}
		else if (fabs(mSX-mX) > 16) {
			mX += (mSX-mX)*0.005f*dt;
		}
		else {
			//mX += (mSX-mX)*0.2f;
		}

		if (fabs(mSY-mY) > 40) {
			mY = mSY;
		}
		else if (fabs(mSY-mY) > 16) {
			mY += (mSY-mY)*0.005f*dt;
		}
		else {
			//mY += (mSY-mY)*0.2f;
		}

	//}


}