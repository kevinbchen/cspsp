#ifndef _BULLET_H_
#define _BULLET_H_

#ifdef WIN32
#include <math.h>
#else
#include <fastmath.h>
#endif
#include "JRenderer.h"
#include "GunObject.h"

#define TYPE_BULLET 0
#define TYPE_GRENADE 1
#define BULLET_DEAD 5

//------------------------------------------------------------------------------------------------
class Person;

class Bullet
{
private:
	float limit(float a, float b, int c);

protected:
	static JRenderer* mRenderer;
	bool mIsFirstUpdate;

public:
	float cosAngle;
	float sinAngle;

	float mX;
	float mY;
	float pX;
	float pY;
	float mStartX;
	float mStartY;
	float mEndX;
	float mEndY;
	float mAngle;
	float mSpeed;
	int mDamage;
	int mState;
	Person* mParent;
	Gun* mParentGun;
	int mId;
	int mType;

	Bullet(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent);
	virtual ~Bullet();

	virtual void Update(float dt);
	virtual void Render(float x, float y);
	void SetAngle(float angle);
	void Reset(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent);

};

inline float Bullet::limit(float a, float b, int c) {
	return (a*c > b*c) ? a:b;
}

#endif
