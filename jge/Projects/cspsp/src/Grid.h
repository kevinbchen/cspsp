#ifndef _GRID_H_
#define _GRID_H_

#include <vector>
#include "Person.h"
#include "GunObject.h"
#include "Person.h"
#include "TileMap.h"
#include "Collision.h"

struct Cell {
	std::vector<CollisionLine*> mCollisionLines;
	std::vector<Person*> mPeople;
	std::vector<GunObject*> mGunObjects;
	std::vector<Bullet*> mBullets;
};

//------------------------------------------------------------------------------------------------
class Grid
{
private:

protected:

public:
	int mCellSize;
	int mWidth;
	int mHeight;
	float mConversion;
	std::vector<Cell> mCells;

	Grid();
	~Grid();
	void ClearCells(bool lines = false);
	void AddPerson(Person* person, int cell);
	void AddGunObject(GunObject* gunobject, int cell);
	void AddBullet(Bullet* bullet, int cell);
	void AddCollisionLine(CollisionLine* line, int cell);

	void HashPerson(Person* person);
	void HashGunObject(GunObject* gunobject);
	void HashBullet(Bullet* bullet);
	void HashCollisionLine(CollisionLine* line);

	int Hash(float x, float y);
	int Hash(float t);

	void Rebuild(float maxX, float maxY, int cellsize);

	bool LineOfSight(float x1, float y1, float x2, float y2, bool bullets = true, bool people = false);
};


//------------------------------------------------------------------------------------------------
inline int Grid::Hash(float x, float y)
{
	int cx = (int)(x*mConversion);
	if (cx >= mWidth) cx = mWidth-1;
	int index = ((int)(y*mConversion))*mWidth + cx;

	return index;
}

//------------------------------------------------------------------------------------------------
inline int Grid::Hash(float t)
{
	return (int)(t*mConversion);
}

#endif
