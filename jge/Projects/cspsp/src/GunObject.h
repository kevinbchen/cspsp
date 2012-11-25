#ifndef _GUNOBJECT_H_
#define _GUNOBJECT_H_

#include "JGE.h"
#include "JRenderer.h"
#include "JSoundSystem.h"
#include <vector>

struct Gun {
	JQuad* mHandQuad;
	JQuad* mGroundQuad;
	int mId;
	int mDelay;
	int mDamage;
	float mSpread;
	int mClip;
	int mNumClips;
	int mReloadDelay;
	float mSpeed;
	float mBulletSpeed;
	float mViewAngle;
	int mCost;
	int mType;
	char mName[15];
	JSample* mFireSound;
	JSample* mReloadSound;
	JSample* mDryFireSound;

	int mAmmoBarWidth;
};

//------------------------------------------------------------------------------------------------

class GunObject
{
private:
	static JRenderer* mRenderer;

protected:

public:
	float mAngle;
	float mSpeed;
	float mRotation;

	float mX;
	float mY;
	float mOldX;
	float mOldY;
	Gun *mGun;
	int mClipAmmo;
	int mRemainingAmmo;
	bool mOnGround;
	//bool mSpawned;

	bool mIsOnline;

	GunObject(Gun *gun, int clipammo, int remainingammo);
	virtual ~GunObject();

	virtual void Update(float dt);
	void Render(float x, float y);

	void SetTotalRotation(float theta);
	//void Reset();
};
#endif
