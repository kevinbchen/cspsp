#ifndef _PersonOnline_H_
#define _PersonOnline_H_

#include "Person.h"
//#include "UdpManager.h"

#define NORMAL2 0
#define ATTACKING2 1

struct Input {
	int x;
	int y;
	float facingangle;
};

struct State {
	float x;
	float y;
	float speed;
	float angle;
};

struct Move {
	Input input;
	State state;
	float time;
};
//------------------------------------------------------------------------------------------------
class PersonOnline : public Person
{
private:

protected:

public:
	float mSX;
	float mSY;
	float mOldSX;
	float mOldSY;
	float mSSpeed;
	float mSAngle;
	float mSFacingAngle;
	float mSPing;
	int mId;

	float mCurrentTime;

	float mLastMoveTime;
	float mLastDropTime;
	float mLastFireTime;

	float mLastSX;
	float mLastSY;

	JTexture* mIconTexture;
	JQuad* mIconQuad;

	//UdpManager* mUdpManager;

	PersonOnline(JQuad* quads[], JQuad* deadquad, std::vector<Bullet*>* bullets, std::vector<GunObject*>* guns, int team, char* name, int movementstyle);
	~PersonOnline();

	void Update(float dt);
	void Render(float x, float y);
	std::vector<Bullet*> Fire();
	std::vector<Bullet*> StopFire();
	void Switch(int index);
	void SwitchNext();
	bool Drop(int index, float speed = 0.35f);
	void Die();
	void Reset();
	void ReceiveInput(Input input, float time);
	void Teleport(float x, float y);

};
#endif

