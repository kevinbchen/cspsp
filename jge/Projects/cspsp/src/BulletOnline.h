#ifndef _BULLETONLINE_H_
#define _BULLETONLINE_H_

#include "Bullet.h"
//------------------------------------------------------------------------------------------------

class BulletOnline : public Bullet
{
private:

protected:

public:
	float mSX;
	float mSY;

	BulletOnline(float x, float y, float px, float py, float angle, float speed, int damage, Person *parent);
	~BulletOnline();

	virtual void Update(float dt);
	virtual void Render(float x, float y);

	void AddLatency(float latency);

};
#endif
