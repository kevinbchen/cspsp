#include "Grenade.h"

//------------------------------------------------------------------------------------------------
Grenade::Grenade(float x, float y, float px, float py, float angle, float speed, Person *parent, int type) : Bullet(x,y,px,py,angle,speed,0,parent)
{
	mType = TYPE_GRENADE;
	mGrenadeType = type;
	mTimer = 1500.0f;
	mSpinAngle = 0.0f;
}

//------------------------------------------------------------------------------------------------
Grenade::~Grenade()
{
}


//------------------------------------------------------------------------------------------------
void Grenade::Update(float dt)
{
	Bullet::Update(dt);
	mTimer -= dt;
	if (mTimer < 0.0f) {
		mState = BULLET_DEAD;
	}
	mSpinAngle += 0.01f*dt;
}



//------------------------------------------------------------------------------------------------
void Grenade::Render(float x, float y)
{
	float offsetX = (x-SCREEN_WIDTH_2);
	float offsetY = (y-SCREEN_HEIGHT_2);

	mRenderer->RenderQuad(mParentGun->mGroundQuad,mX-offsetX,mY-offsetY,mSpinAngle);
	
}

