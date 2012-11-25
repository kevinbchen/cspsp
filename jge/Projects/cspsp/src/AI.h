#ifndef _AI_H_
#define _AI_H_

#include "Person.h"
#include "TileMap.h"
#include "Collision.h"
#include "AStar.h"
#include "Grid.h"

#define NONE -1
#define T 0
#define CT 1

#define AI_IDLE 0
#define AI_RANDOM 1
#define AI_SEARCHING 2
#define AI_ATTACKING 3
#define AI_FLASHED 4
//------------------------------------------------------------------------------------------------
class AI : public Person
{
private:
	float mPathTime;
	float mMoveTime;
	float mFireTime;
	float mNewAngle;
	int mAIState;
	float mAIStateTime;
	Person* mTarget;
	std::vector<Node*> mPath;
	
	float mFaceTargetX;
	float mFaceTargetY;
	float mMoveTargetX;
	float mMoveTargetY;
	bool mCanSeeEnemy;

	float mStuckTimer;
	Gun *mBuyGun;

	void SetAIState(int state);
	Person* GetClosestPerson();
protected:

public:
	AStar* mAStar;
	Grid* mGrid;
	Gun (*mGameGuns)[28];
	//std::vector<CollisionPoint>* mCollisionPoints;

	AI(JQuad* quads[], JQuad* deadquad, std::vector<Bullet*>* bullets, std::vector<GunObject*>* guns, std::vector<Node*> nodes, int team, char* name, int movementstyle);
	~AI();

	void Update(float dt);
	void Render(float x, float y);
	void Reset();

};
#endif
