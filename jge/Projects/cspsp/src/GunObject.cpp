
#include "GunObject.h"

JRenderer* GunObject::mRenderer = NULL;

//------------------------------------------------------------------------------------------------
GunObject::GunObject(Gun *gun, int clipammo, int remainingammo)
{
	mRenderer = JRenderer::GetInstance();
	mAngle = 0.0f;
	mRotation = 0.0f;
	mGun = gun;
	mClipAmmo = clipammo;
	mRemainingAmmo = remainingammo;
	mOnGround = true;
	mX = 0.0f;
	mY = 0.0f;
	mOldX = 0.0f;
	mOldY = 0.0f;
	mIsOnline = false;
}

//------------------------------------------------------------------------------------------------
GunObject::~GunObject()
{
	
}

//------------------------------------------------------------------------------------------------
void GunObject::Update(float dt) 
{ 
	mOldX = mX;
	mOldY = mY;
	if (!mOnGround) {
		mSpeed -= 0.001f*dt;
		if (mSpeed < 0.0f) {
			mSpeed = 0.0f;
			mOnGround = true;
		}
		mX += mSpeed * cosf(mAngle) * dt;
		mY += mSpeed * sinf(mAngle) * dt;
	}
}

//------------------------------------------------------------------------------------------------
void GunObject::Render(float x, float y) 
{ 
	float xtemp = mX;
	float ytemp = mY;
	mX = mX - x + SCREEN_WIDTH_2;
	mY = mY - y + SCREEN_HEIGHT_2;
	mRenderer->RenderQuad(mGun->mGroundQuad, mX, mY, mRotation);
	mX = xtemp;
	mY = ytemp;
}

//------------------------------------------------------------------------------------------------
void GunObject::SetTotalRotation(float theta)
{
	mRotation = theta-M_PI_2;
	mAngle = theta;	
}

//------------------------------------------------------------------------------------------------
/**void GunObject::Reset()
{
	if (mSpawned) {
		mClipAmmo = mGun->mClip;
		mRemainingAmmo = mGun->mClip*(mGun->mNumClips-1);
		mX = oldx;
		mY = oldy;
	}
	else {
		delete this;
	}
}**/
