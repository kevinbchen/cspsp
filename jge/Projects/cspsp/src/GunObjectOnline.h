#ifndef _GunObjectOnline_H_
#define _GunObjectOnline_H_

#include "GunObject.h"

//------------------------------------------------------------------------------------------------

class GunObjectOnline : public GunObject
{
private:

protected:

public:
	int mId;
	float mSX;
	float mSY;
	float mSAngle;

	GunObjectOnline(Gun *gun, int clipammo, int remainingammo);
	~GunObjectOnline();

	void Update(float dt);
};
#endif
