#ifndef _PERSON_H_
#define _PERSON_H_

#include "JRenderer.h"
#include "JSoundSystem.h"
#include "Bullet.h"
#include "Grenade.h"
#include "GunObject.h"
#include "SfxManager.h"
#include <vector>
#include "Node.h"
#include "GunObjectOnline.h"
#include "Animation.h"

#define RELATIVE1 0
#define ABSOLUTE1 1

#define MOVING 0
#define NOTMOVING 1
#define NORMAL 0
#define DEAD 1
#define ATTACKING 2
#define RELOADING 3
#define DRYFIRING 4
#define SWITCHING 5

#define PRIMARY 0
#define SECONDARY 1
#define KNIFE 2
#define GRENADE 3

#define BODY 0
#define RIGHTARM 1
#define RIGHTHAND 2
#define LEFTARM 3
#define LEFTHAND 4
#define GUN 5
#define HEAD 5
#define LEGS 6

#define NUM_QUADS 7
#define WALKTIME 160.0f

enum {
	ANIM_PRIMARY = 0,
	ANIM_SECONDARY,
	ANIM_KNIFE,
	ANIM_GRENADE,
	ANIM_BOMB,
	ANIM_PRIMARY_FIRE,
	ANIM_SECONDARY_FIRE,
	ANIM_KNIFE_SLASH,
	ANIM_GRENADE_PULLBACK,
	ANIM_PRIMARY_RELOAD,
	ANIM_SECONDARY_RELOAD
};


enum {
	WALK1 = 0,
	WALK2,
	WALK3,
	WALK4
};


//------------------------------------------------------------------------------------------------

class Person
{
private:

protected:
	static JRenderer* mRenderer;
	static JSoundSystem* mSoundSystem;
	int mSoundId;
	int mNumDryFire;
	KeyFrame mKeyFrame;
	Animation* mAnimations[11];
	Animation* mCurrentAnimation;

	int mMuzzleFlashIndex;
	float mMuzzleFlashAngle;
	float mMuzzleFlashTime;

	float mWalkX;
	float mWalkY;

	void UpdateAngle(float &angle, float targetangle, float speed);

public:
	JQuad* mQuads[NUM_QUADS];
	//JQuad* mPartQuads[6];
	JQuad* mDeadQuad;

	float mAngle;
	float mSpeed;
	float mRotation;

	float mX;
	float mY;
	float mOldX;
	float mOldY;

	std::vector<Person*>* mPeople;
	std::vector<Bullet*>* mBullets;
	std::vector<GunObject*>* mGunObjects;	

	float mMaxSpeed;
	int mState;
	int mMoveState;
	float mStateTime;
	float mFadeTime;
	float mStepTime;
	int mHealth;
	int mMoney;
	float mFacingAngle;
	float mRecoilAngle;
	float mLastFireAngle;
	bool mIsActive;
	GunObject* mGuns[5];
	int mGunIndex;
	//GunObject* mPrimaryGun;
	//GunObject* mSecondaryGun;
	//GunObject* mKnife;

	int mTeam;
	char mName[32];
	int mMovementStyle;
	int mNumKills;
	int mNumDeaths;

	float mPing;

	bool mIsPlayerOnline;

	bool mIsFiring;
	bool mHasFired;

	bool mIsFlashed;
	float mFlashTime;
	float mFlashIntensity;

	bool mIsInBuyZone;

	std::vector<Node*> mNodes;

	Node* mNode;
	Node* mTargetNode;

	int mWalkState;
	float mWalkTime;
	float mWalkAngle;

	float mRadarTime;
	float mRadarX;
	float mRadarY;

	bool mHasFlag;

	float mInvincibleTime;

	Person(JQuad* quads[], JQuad* deadquad, std::vector<Bullet*>* bullets, std::vector<GunObject*>* guns, int team, char* name, int movementstyle);
	virtual ~Person();

	void PreUpdate(float dt);
	virtual void Update(float dt);
	virtual void Render(float x, float y);
	virtual void Move(float speed, float angle);
	virtual std::vector<Bullet*> Fire();
	virtual std::vector<Bullet*> StopFire();
	virtual bool Reload();
	virtual void Switch(int index);
	void SwitchNext();
	bool PickUp(GunObject* gunobject);
	virtual bool Drop(int index, float speed = 0.35f);
	void RotateFacing(float theta);
	void SetMoveState(int state);
	void SetState(int state);
	void SetAnimation(int animation);
	void SetTotalRotation(float theta);
	virtual void Die();
	virtual void Reset();
	void TakeDamage(int damage);
	void ReceiveFlash(float intensity);
	void SetQuads(JQuad* quads[], JQuad* deadquad);
	GunObject* GetCurrentGun();
	virtual void Teleport(float x, float y);
};

inline GunObject* Person::GetCurrentGun() {
	return mGuns[mGunIndex];
}

#endif
