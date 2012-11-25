
#include "PersonOnline.h"
#include "Globals.h"

//------------------------------------------------------------------------------------------------
PersonOnline::PersonOnline(JQuad* quads[], JQuad* deadquad, std::vector<Bullet*>* bullets, std::vector<GunObject*>* guns, int team, char* name, int movementstyle) : Person(quads, deadquad, bullets, guns, team, name, movementstyle)
{
	mId = -1;
	mIsPlayerOnline = false;
	mSX = 0.0f;
	mSY = 0.0f;
	mOldSX = 0.0f;
	mOldSY = 0.0f;
	mSSpeed = 0.0f;
	mSAngle = 0.0f;
	mSFacingAngle = M_PI_2;
	mSPing = 0.0f;
	mCurrentTime = 0.0f;
	mLastMoveTime = 0.0f;
	mLastDropTime = 0.0f;
	mLastFireTime = 0.0f;

	mLastSX = 0.0f;
	mLastSY = 0.0f;

	mIconTexture = mRenderer->CreateTexture(10,10);
	mIconQuad = new JQuad(mIconTexture,0,0,10,10);
}

//------------------------------------------------------------------------------------------------
PersonOnline::~PersonOnline()
{
	delete mIconQuad;
	delete mIconTexture;
	//JGERelease();
}


//------------------------------------------------------------------------------------------------
void PersonOnline::Update(float dt)
{
	mPing = mPing*0.95f + mSPing*0.05f;

	if (!mIsPlayerOnline) {
		if (mIsFiring) {
			bool fire = true;
			if (mGunIndex == SECONDARY) {
				if (mHasFired) {
					fire = false;
				}
			}
			if (fire) {
				Fire();
				//mHasFired = true;
			}
			mNumDryFire = 0; //make sure person doesn't autoreload;
		}
	}

	if (mIsPlayerOnline && mState != DEAD) {

		mOldX = mX;
		mOldY = mY;

		float speed = 0.1f*mGuns[mGunIndex]->mGun->mSpeed;

		if (fabs(mLastSX-mSX) > 100 || fabs(mLastSY-mSY) > 100) {
			mSX = mLastSX;
			mSY = mLastSY;
			mOldSX = mLastSX;
			mOldSY = mLastSY;
		}

		if (fabs(mSX-mX) > 20) {
			mX = mX*0.95f+mSX*0.05f;
			//mOldX = mSX;
		}
		
		/*float v = (mSX-mX)*0.01f;
		if (fabs(v) > 0.1f*mGuns[mGunIndex]->mGun->mSpeed) {
			if (v > 0.0f) {
				v = 0.1f*mGuns[mGunIndex]->mGun->mSpeed;
			}
			else if (v < 0.0f) {
				v = -0.1f*mGuns[mGunIndex]->mGun->mSpeed;
			}
		}
		mX += v*dt;*/
		if (mX < mSX) {
			mX += (fabs(speed*cosf(mAngle))+0.01f)*dt;
			if (mX > mSX) {
				mX = mSX;
			}
		}
		else if (mX > mSX) {
			mX -= (fabs(speed*cosf(mAngle))+0.01f)*dt;
			if (mX < mSX) {
				mX = mSX;
			}
		}

		if (fabs(mSY-mY) > 20) {
			mY = mY*0.95f+mSY*0.05f;
			//mOldY = mSY;
		}
		/*float v = (mSY-mY)*0.01f;
		if (fabs(v) > 0.1f*mGuns[mGunIndex]->mGun->mSpeed) {
			if (v > 0.0f) {
				v = 0.1f*mGuns[mGunIndex]->mGun->mSpeed;
			}
			else if (v < 0.0f) {
				v = -0.1f*mGuns[mGunIndex]->mGun->mSpeed;
			}
		}
		mY += v*dt;*/
		if (mY < mSY) {
			mY += (fabs(speed*sinf(mAngle))+0.01f)*dt;
			if (mY > mSY) {
				mY = mSY;
			}
		}
		else if (mY > mSY) {
			mY -= (fabs(speed*sinf(mAngle))+0.01f)*dt;
			if (mY < mSY) {
				mY = mSY;
			}
		}
	}
	else if (!mIsPlayerOnline && mState != DEAD) {
		mSpeed = mSSpeed;
		mAngle = mSAngle;

		mSX += mSpeed * cosf(mAngle) * dt;
		mSY += mSpeed * sinf(mAngle) * dt;

		if (fabs(mSX-mX) > 75) {
			mX = mSX;
			mY = mSY;
			mOldX = mSX;
			mOldY = mSY;
		}
		else {
			mX += (mSX-mX)*0.006f*dt;
		}

		if (fabs(mSY-mY) > 75) {
			mX = mSX;
			mY = mSY;
			mOldX = mSX;
			mOldY = mSY;
		}
		else {
			mY += (mSY-mY)*0.006f*dt;
		}

		/*if (fabs(mSSpeed-mSpeed) > 0.05f) {
			mSpeed += (mSSpeed-mSpeed)*0.01f*dt;
		}
		else {
			//SetSpeed(GetSpeed()+(mSSpeed-GetSpeed())*0.2f);
		}*/
		//SetSpeed(GetSpeed()+(mSSpeed-GetSpeed())*0.01f*dt);


		/*if (fabs(mSAngle-GetAngle()) > 0.05f) {

			//SetAngle(GetAngle()+(mSAngle-GetAngle())*0.01f*dt);
		}
		else {
			//SetAngle(GetAngle()+(mSAngle-GetAngle())*0.2f);
		}*/


		float difftheta = mFacingAngle-mSFacingAngle;

		float speed = 0.005f*dt*1.1f;
		//float anglediff = fabs(fabs(fabs(mSFacingAngle-mFacingAngle)-M_PI)-M_PI);

		if (difftheta > M_PI) {
			difftheta -= 2*M_PI;
		}
		if (difftheta < -M_PI) {
			difftheta += 2*M_PI;
		}
		if (difftheta >= 0) {
			if (difftheta < speed) {
				mFacingAngle = mSFacingAngle;
			}
			else {
				mFacingAngle -= speed;
			}
		}
		else {
			if (difftheta > -speed) {
				mFacingAngle = mSFacingAngle;
			}
			else {
				mFacingAngle += speed;
			}
		}

		mRotation = mFacingAngle-M_PI_2;
	}

	Person::Update(dt);
	/*if (mState == RELOADING) {

	}*/
}

//------------------------------------------------------------------------------------------------
void PersonOnline::Render(float x, float y)
{
	Person::Render(x,y);

	/*if (mState != DEAD) {
		mRenderer->DrawCircle(mOldSX- x + SCREEN_WIDTH_2,mOldSY- y + SCREEN_HEIGHT_2,16,ARGB(255,255,0,0));
		mRenderer->DrawCircle(mSX- x + SCREEN_WIDTH_2,mSY- y + SCREEN_HEIGHT_2,16,ARGB(255,0,0,255));

		mRenderer->DrawCircle(mLastSX- x + SCREEN_WIDTH_2,mLastSY- y + SCREEN_HEIGHT_2,16,ARGB(255,0,0,0));

		//mQuad->SetColor(ARGB(100,255,255,255));
		//mEngine->RenderQuad(mQuad, mSX, mSY, mRotation);
		//mQuad->SetColor(ARGB(255,255,255,255));

	}*/
}

//------------------------------------------------------------------------------------------------
std::vector<Bullet*> PersonOnline::Fire()
{
	if (mIsPlayerOnline) {
		return Person::Fire();
	}

	std::vector<Bullet*> bullets;
	if (!mIsActive) return bullets;
	if (mState == NORMAL) {
		mIsFiring = true;
		mHasFired = true;
		if (mGunIndex == KNIFE) {
			SetState(ATTACKING);
			gSfxManager->PlaySample(mGuns[mGunIndex]->mGun->mFireSound,mX,mY);
			//return true;
		}
		else if (mGunIndex == GRENADE) {
			if (mGuns[mGunIndex]->mClipAmmo != 0)  {
				SetState(ATTACKING);
				gSfxManager->PlaySample(mGuns[mGunIndex]->mGun->mFireSound,mX,mY);
			}
		}
		else {
			if (mGuns[mGunIndex]->mClipAmmo > 0)  {
				//gParticleEngine->GenerateParticles(BULLETSHELL,mX,mY,1);*/
				SetState(ATTACKING);
				gSfxManager->PlaySample(mGuns[mGunIndex]->mGun->mFireSound,mX,mY);

				mMuzzleFlashTime = 50.0f;
				mMuzzleFlashAngle = mFacingAngle;
				mMuzzleFlashIndex = mGuns[mGunIndex]->mGun->mType*3 + rand()%3;
				
				mRadarTime = 2000.0f;
				mRadarX = mX;
				mRadarY = mY;
				//return true;
			}
			else {
				SetState(DRYFIRING);
				gSfxManager->PlaySample(mGuns[mGunIndex]->mGun->mDryFireSound,mX,mY);
				//return;
			}
		}
	}
	return bullets;
	//return false;
}

//------------------------------------------------------------------------------------------------
std::vector<Bullet*> PersonOnline::StopFire()
{
	if (mIsPlayerOnline) {
		return Person::StopFire();
	}

	std::vector<Bullet*> bullets;
	if (!mIsActive) return bullets;
	
	mIsFiring = false;
	mHasFired = false;

	if (mState == ATTACKING) {
		if (mGunIndex == GRENADE) {
			if (mGuns[mGunIndex]->mClipAmmo != 0)  {
				SetState(NORMAL);
				gSfxManager->PlaySample(gFireInTheHoleSound,mX,mY);
				mGuns[mGunIndex]->mClipAmmo--;

				delete mGuns[mGunIndex];
				mGuns[mGunIndex] = NULL;
				SwitchNext();
			}
		}
	}

	return bullets;
}

//------------------------------------------------------------------------------------------------
void PersonOnline::Switch(int index)
{
	Person::Switch(index);

	/*Packet sendpacket = Packet();
	sendpacket.WriteInt16(SWITCHGUN);
	sendpacket.WriteInt16(mGunIndex);
	mUdpManager->SendReliable(sendpacket);*/
}

//------------------------------------------------------------------------------------------------
void PersonOnline::SwitchNext()
{
	Person::SwitchNext();

	/*Packet sendpacket = Packet();
	sendpacket.WriteInt8(SWITCHGUN);
	sendpacket.WriteInt8(mGunIndex);
	mUdpManager->SendReliable(sendpacket);*/
}

//------------------------------------------------------------------------------------------------
bool PersonOnline::Drop(int index, float speed)
{
	if (index >= 5) return false;
	if (index == KNIFE) return false;

	GunObjectOnline* gunobject = (GunObjectOnline*)mGuns[index];
	if (gunobject == NULL) return false;

	/*if (mState == RELOADING) {
		SetState(NORMAL);
		if (mSoundId != -1) {
			mSoundSystem->StopSample(mSoundId);
			mSoundId = -1;
		}
	}*/
	gunobject->mX = mX;
	gunobject->mY = mY;
	gunobject->mOldX = mX;
	gunobject->mOldY = mY;
	gunobject->mSX = mX;
	gunobject->mSY = mY;
	gunobject->mRotation = mRotation;
	gunobject->mAngle = mFacingAngle;
	gunobject->mSAngle = mFacingAngle;
	gunobject->mSpeed = speed;
	gunobject->mOnGround = false;
	mGunObjects->push_back(gunobject);

	mGuns[index] = NULL;
	
	if (mGuns[PRIMARY] != NULL) {
		//mGunIndex = PRIMARY;
		Switch(PRIMARY);
	}
	else if (mGuns[SECONDARY] != NULL) {
		//mGunIndex = SECONDARY;
		Switch(SECONDARY);
	}
	else if (mGuns[GRENADE] != NULL) {
		//mGunIndex = GRENADE;
		Switch(GRENADE);
	}
	else if (mGuns[KNIFE] != NULL) {
		//mGunIndex = KNIFE;
		Switch(KNIFE);
	}

	//mRecoilAngle = 0.0f;
	//mNumDryFire = 0;
	return true;
}

//------------------------------------------------------------------------------------------------
void PersonOnline::Die()
{
	gSfxManager->PlaySample(gDieSounds[rand()%3],mX,mY);
	//Drop(mGunIndex,0);
	//Drop(GRENADE,0);

	if (mSoundId != -1) {
		mSoundSystem->StopSample(mSoundId);
		mSoundId = -1;
	}
	if (mGuns[PRIMARY] != NULL) {
		delete mGuns[PRIMARY];
		mGuns[PRIMARY] = NULL;
	}
	if (mGuns[SECONDARY] != NULL) {
		delete mGuns[SECONDARY];
		mGuns[SECONDARY] = NULL;
	}
	/*if (mGunIndex != KNIFE) {
		mGuns[mGunIndex] = NULL;
	}*/
	if (mGuns[GRENADE] != NULL) {
		delete mGuns[GRENADE];
		mGuns[GRENADE] = NULL;
	}
	mGunIndex = KNIFE;

	mNumDryFire = 0;

	SetState(DEAD);
	mFadeTime = 1000.0f;
	SetMoveState(NOTMOVING);
	mSpeed = 0.0f;
	//Person::Die();
}

void PersonOnline::Reset() {
	Person::Reset();

	mCurrentTime = 0.0f;
	mLastMoveTime = 0.0f;
	mLastDropTime = 0.0f;
	mLastFireTime = 0.0f;
}

void PersonOnline::ReceiveInput(Input input, float time) {
	if (time <= mCurrentTime) return;

	float dt = time - mCurrentTime;
	if (mCurrentTime <= 0.0001f) {
		dt = 16.0f;
	}

	mCurrentTime = time;
        
	if (!mIsActive) return;
	if (mState == DEAD) return;

	int aX = input.x;
	int aY = input.y;
	if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
		float angle = atan2f(aX,-aY);
		float speed = (sqrtf(aX*aX + aY*aY)/127.5f)*0.1f;
		if (speed > 0.1f) {
			speed = 0.1f;
		}
		SetMoveState(MOVING);
		mMaxSpeed = speed*mGuns[mGunIndex]->mGun->mSpeed;
		if (mMovementStyle == RELATIVE1) {
			mAngle = input.facingangle+angle;
		}
		else if (mMovementStyle == ABSOLUTE1) {
			mAngle = angle-M_PI_2;
		}
	}
	else {
		SetMoveState(NOTMOVING);
	}


	if (mIsActive) {
		if (mMoveState == NOTMOVING) {
			mSpeed -= .0005f*dt;
			if (mSpeed < 0) {
				mSpeed = 0.0f;
			}
		}
		else if (mMoveState == MOVING) {
			mSpeed += .0005f*dt;
			if (mSpeed > mMaxSpeed) {
				mSpeed = mMaxSpeed;
			}
		}
	}

	mOldSX = mSX;
	mOldSY = mSY;

	mSX += mSpeed*cosf(mAngle)*dt;
	mSY += mSpeed*sinf(mAngle)*dt;
}

void PersonOnline::Teleport(float x, float y) {
	Person::Teleport(x,y);
	mSX = x;
	mSY = y;
	mOldSX = x;
	mOldSY = y;
	mLastSX = x;
	mLastSY = y;
}