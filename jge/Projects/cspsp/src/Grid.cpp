#include "Grid.h"


//------------------------------------------------------------------------------------------------
Grid::Grid()
{
	// initialize with some reasonable values (so shit doesn't crash during map dling)
	Rebuild(100,100,100);
}

//------------------------------------------------------------------------------------------------
Grid::~Grid()
{
	ClearCells(true);
}

//------------------------------------------------------------------------------------------------
void Grid::ClearCells(bool lines)
{
	for (unsigned int i=0;i<mCells.size();i++) {
		mCells[i].mPeople.clear();
		mCells[i].mGunObjects.clear();
		mCells[i].mBullets.clear();
		if (lines) {
			mCells[i].mCollisionLines.clear();
		}
	}
}

//------------------------------------------------------------------------------------------------
void Grid::AddPerson(Person* person, int cell)
{
	if (cell < mCells.size()) {
		if (mCells[cell].mPeople.size() > 0) {
			if (person == mCells[cell].mPeople.back()) {  //quick hack to make sure object isn't added twice
				return;
			}
		}
		mCells[cell].mPeople.push_back(person);
	}
}

//------------------------------------------------------------------------------------------------
void Grid::AddGunObject(GunObject* gunobject, int cell)
{
	if (cell < mCells.size()) {
		if (mCells[cell].mGunObjects.size() > 0) {
			if (gunobject == mCells[cell].mGunObjects.back()) {  //quick hack to make sure object isn't added twice
				return;
			}
		}
		mCells[cell].mGunObjects.push_back(gunobject);
	}
}

//------------------------------------------------------------------------------------------------
void Grid::AddBullet(Bullet* bullet, int cell)
{
	if (cell < mCells.size()) {
		if (mCells[cell].mBullets.size() > 0) {
			if (bullet == mCells[cell].mBullets.back()) {  //quick hack to make sure object isn't added twice
				return;
			}
		}
		mCells[cell].mBullets.push_back(bullet);
	}
}

//------------------------------------------------------------------------------------------------
void Grid::AddCollisionLine(CollisionLine* line, int cell)
{
	if (cell < mCells.size()) {
		if (mCells[cell].mCollisionLines.size() > 0) {
			if (line == mCells[cell].mCollisionLines.back()) {  //quick hack to make sure object isn't added twice
				return;
			}
		}
		mCells[cell].mCollisionLines.push_back(line);
	}
}



//------------------------------------------------------------------------------------------------
void Grid::HashPerson(Person* person)
{
	float radius = 16.0f;
	float minx = person->mX-radius;//((person->mX < person->mOldX) ? person->mX:person->mOldX) - radius;
	float miny = person->mY-radius;//((person->mY < person->mOldY) ? person->mY:person->mOldY) - radius;
	float maxx = person->mX+radius;//((person->mX > person->mOldX) ? person->mX:person->mOldX) + radius;
	float maxy = person->mY+radius;//((person->mY > person->mOldY) ? person->mY:person->mOldY) + radius;

	int maxi = (int)(floorf(maxx*mConversion)-floorf(minx*mConversion));
	int maxj = (int)(floorf(maxy*mConversion)-floorf(miny*mConversion));
	for (int i=0; i<=maxi; i++) {
		for (int j=0; j<=maxj; j++) {
			int index = Hash(minx+i*mCellSize,miny+j*mCellSize);
			AddPerson(person,index);
		}
	}
}

//------------------------------------------------------------------------------------------------
void Grid::HashGunObject(GunObject* gunobject)
{
	float radius = 8.0f;
	float minx = gunobject->mX-radius;//((gunobject->mX < gunobject->mOldX) ? gunobject->mX:gunobject->mOldX) - radius;
	float miny = gunobject->mY-radius;//((gunobject->mY < gunobject->mOldY) ? gunobject->mY:gunobject->mOldY) - radius;
	float maxx = gunobject->mX+radius;//((gunobject->mX > gunobject->mOldX) ? gunobject->mX:gunobject->mOldX) + radius;
	float maxy = gunobject->mY+radius;//((gunobject->mY > gunobject->mOldY) ? gunobject->mY:gunobject->mOldY) + radius;

	int maxi = (int)(floorf(maxx*mConversion)-floorf(minx*mConversion));
	int maxj = (int)(floorf(maxy*mConversion)-floorf(miny*mConversion));
	for (int i=0; i<=maxi; i++) {
		for (int j=0; j<=maxj; j++) {
			int index = Hash(minx+i*mCellSize,miny+j*mCellSize);
			AddGunObject(gunobject,index);
		}
	}
}

//------------------------------------------------------------------------------------------------
void Grid::HashBullet(Bullet* bullet)
{
	/*float radius = 1.0f; //just in case line is exactly on grid line
	float minx = ((bullet->mX < bullet->pX) ? bullet->mX:bullet->pX) - radius;
	float miny = ((bullet->mY < bullet->pY) ? bullet->mY:bullet->pY) - radius;
	float maxx = ((bullet->mX > bullet->pX) ? bullet->mX:bullet->pX) + radius;
	float maxy = ((bullet->mY > bullet->pY) ? bullet->mY:bullet->pY) + radius;

	int maxi = (int)(floorf(maxx*mConversion)-floorf(minx*mConversion));
	int maxj = (int)(floorf(maxy*mConversion)-floorf(miny*mConversion));
	for (int i=0; i<=maxi; i++) {
		for (int j=0; j<=maxj; j++) {
			int index = Hash(minx+i*mCellSize,miny+j*mCellSize);
			AddBullet(bullet,index);
		}
	}*/
}

//------------------------------------------------------------------------------------------------
void Grid::HashCollisionLine(CollisionLine* line)
{
	float radius = 1.0f; //just in case line is exactly on grid line
	float minx = ((line->line.x1 < line->line.x2) ? line->line.x1:line->line.x2) - radius;
	float miny = ((line->line.y1 < line->line.y2) ? line->line.y1:line->line.y2) - radius;
	float maxx = ((line->line.x1 > line->line.x2) ? line->line.x1:line->line.x2) + radius;
	float maxy = ((line->line.y1 > line->line.y2) ? line->line.y1:line->line.y2) + radius;

	int maxi = (int)(floorf(maxx*mConversion)-floorf(minx*mConversion));
	int maxj = (int)(floorf(maxy*mConversion)-floorf(miny*mConversion));
	for (int i=0; i<=maxi; i++) {
		for (int j=0; j<=maxj; j++) {
			int index = Hash(minx+i*mCellSize,miny+j*mCellSize);
			AddCollisionLine(line,index);
			/*if ((int)(miny+j)%mGrid->mCellSize == 0) {
				int index = mGrid->Hash(minx+j,miny+k-1);
				mGrid->AddCollisionLine(collisionLine,index);
			}*/
		}
	}
}


//------------------------------------------------------------------------------------------------
void Grid::Rebuild(float maxX, float maxY, int cellsize)
{
	ClearCells(true);

	mCellSize = cellsize;
	mWidth = ceilf(maxX/(float)cellsize);
	mHeight = ceilf(maxY/(float)cellsize);
	mConversion = 1.0f/cellsize;
	for (int y=0; y<maxY; y+=cellsize) {
		for (int x=0; x<maxX; x+=cellsize) {
			mCells.push_back(Cell());
		}
	}
}

//------------------------------------------------------------------------------------------------
bool Grid::LineOfSight(float x1, float y1, float x2, float y2, bool bullets, bool people) {

	Vector2D p1(x2,y2);
	Vector2D p2(x1,y1);

	Vector2D normal = (p1-p2);
	normal.Normalize();

	Line l1(p2,p1);
	if (fabs(p1.x-p2.x) < EPSILON && fabs(p1.y-p2.y) < EPSILON) return true; //underflow

	int gridWidth = mWidth;
	int gridHeight = mHeight;
	int cellSize = mCellSize;

	int cellX = p2.x*mConversion;///mGrid->mCellSize;
	int cellY = p2.y*mConversion;///mGrid->mCellSize;

	int cellX2 = p1.x*mConversion;
	int cellY2 = p1.y*mConversion;

	float vX = normal.x;
	float vY = normal.y;

	int dirX = (vX >= 0.0f) ? 1 : -1;
	int dirY = (vY >= 0.0f) ? 1 : -1;
	float tX = 0.0f;
	float tY = 0.0f;

	float stepX = 0.0f;
	float stepY = 0.0f;

	if (fabs(vX) > EPSILON) {
		if (dirX >= 0.0f) {
			tX = ((cellX+1)*cellSize-p2.x)/vX;
		}
		else {
			tX = ((cellX)*cellSize-p2.x)/vX;
		}
		stepX = cellSize/fabs(vX);
	}
	else {
		tX = 100000;
		stepX = 100000;
	}

	if (fabs(vY) > EPSILON) {
		if (dirY >= 0.0f) {
			tY = ((cellY+1)*cellSize-p2.y)/vY;
		}
		else {
			tY = ((cellY)*cellSize-p2.y)/vY;
		}
		stepY = cellSize/fabs(vY);
	}
	else {
		tY = 1000000;
		stepY = 100000;
	}

	
	while (true) {
		if (cellX < 0 || cellX >= gridWidth || cellY < 0 || cellY >= gridHeight) break;

		int cell = cellY*gridWidth + cellX;

		//int intersected = 0;
		//float maxT = (tX < tY) ? tX*tX : tY*tY;
		//Line line(0,0,0,0);
		//Vector2D d;

		for (unsigned int j=0; j<mCells[cell].mCollisionLines.size(); j++) {
			if (mCells[cell].mCollisionLines[j]->bullets == bullets || mCells[cell].mCollisionLines[j]->people == people) {

				Line l2 = mCells[cell].mCollisionLines[j]->line;
				if (fabs(l2.x1-l2.x2) < EPSILON && fabs(l2.y1-l2.y2) < EPSILON) continue;

				Vector2D d2;
				if (LineLineIntersect(l1,l2,d2,false)) {
					return false;
				}
			}
		}

		if (cellX == cellX2 && cellY == cellY2) {
			break;
		}
		if (tX < tY) {
			tX += stepX;
			cellX += dirX;
		}
		else {
			tY += stepY;
			cellY += dirY;
		}
	}
	return true;
}