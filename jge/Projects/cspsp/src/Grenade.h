#ifndef _GRENADE_H_
#define _GRENADE_H_

#include "Bullet.h"

#define FLASH 0
#define HE 1
#define SMOKE 2
//------------------------------------------------------------------------------------------------

class Grenade : public Bullet
{
private:

protected:
	float mSpinAngle;

public:
	float mTimer;
	int mGrenadeType;

	Grenade(float x, float y, float px, float py, float angle, float speed, Person *parent, int type);
	~Grenade();

	void Update(float dt);
	void Render(float x, float y);

};
#endif
