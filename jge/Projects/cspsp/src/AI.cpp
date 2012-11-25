
#include "AI.h"
#include "Globals.h"


//------------------------------------------------------------------------------------------------
AI::AI(JQuad* quads[], JQuad* deadquad, std::vector<Bullet*>* bullets, std::vector<GunObject*>* guns, std::vector<Node*> nodes, int team, char* name, int movementstyle) : Person(quads, deadquad, bullets, guns, team, name, movementstyle)
{
	mPathTime = 0.0f;
	mMoveTime = 0.0f;
	mFireTime = 0.0f;
	mNewAngle = 0;
	mNodes = nodes;
	SetAIState(AI_SEARCHING);
	mTarget = NULL;
	//mTargetNode = NULL;
	mFaceTargetX = 0.0f;
	mFaceTargetY = 0.0f;
	mMoveTargetX = 0.0f;
	mMoveTargetY = 0.0f;
	mAIStateTime = 0.0f;
	mCanSeeEnemy = false;
	mStuckTimer = 0.0f;
	mBuyGun = NULL;
}

//------------------------------------------------------------------------------------------------
AI::~AI()
{
	//JGERelease();
}


//------------------------------------------------------------------------------------------------
void AI::Update(float dt)
{
	//return;
	if (mState == DEAD) {
		Person::Update(dt);
		return;
	}

	//if (mGunIndex != KNIFE) Switch(KNIFE);
	float dx = mOldX-mX;
	float dy = mOldY-mY;
	if (fabs(dx) >= EPSILON || fabs(dy) >= EPSILON) {
		if (dx*dx + dy*dy < 0.0016f) {
			mStuckTimer += dt;
			if (mStuckTimer > 3000.0f) {
				mPath.clear();
				mTargetNode = mNode;
			}
		}
		else {
			mStuckTimer = 0.0f;
		}
	}
	else {
		mStuckTimer += dt;
		if (mStuckTimer > 3000.0f) {
			mPath.clear();
			mTargetNode = mNode;
		}
	}

	if (mIsFlashed) {
		SetAIState(AI_FLASHED);
	}

	mAIStateTime += dt;

	switch (mAIState) {
		case AI_IDLE: {

			mMoveTargetX = mX;
			mMoveTargetY = mY;

			if (mAIStateTime > 100.0f) {
				mTarget = GetClosestPerson();

				if (mTarget != NULL) {
					float dx = mTarget->mX-mX;
					float dy = mTarget->mY-mY;
					float distance = dx*dx + dy*dy;

					if (distance < 50000) {
						mPath.clear();
						SetAIState(AI_ATTACKING);
						break;
					}
				}

				SetAIState(AI_SEARCHING);

				mAIStateTime = 0.0f;
			}
			break;
		}
		case AI_RANDOM: {
			if (mTargetNode == NULL) break;

			float dx = mTargetNode->mX-mX;
			float dy = mTargetNode->mY-mY;
			float distance = dx*dx + dy*dy;
			if (distance < 1000) {
				Node* mTempNode;
				for (int i=0;i<50;i++) {
					mTempNode = mTargetNode->mConnections[rand()%mTargetNode->mConnections.size()];
					if (mTargetNode->mConnections.size() == 1) break;
					if (mTempNode != mNode) break;
				}
				mNode = mTargetNode;
				mTargetNode = mTempNode;
			}

			if (mAIStateTime > 100.0f) {
				mTarget = GetClosestPerson();

				if (mTarget != NULL) {
					float dx = mTarget->mX-mX;
					float dy = mTarget->mY-mY;
					float distance = dx*dx + dy*dy;

					if (distance < 50000) {
						mPath.clear();
						SetAIState(AI_ATTACKING);
						break;
					}
				}
				SetAIState(AI_SEARCHING);

				mAIStateTime = 0.0f;
			}

			mMoveTargetX = mTargetNode->mX;
			mMoveTargetY = mTargetNode->mY;

			mFaceTargetX = mTargetNode->mX;
			mFaceTargetY = mTargetNode->mY;

			break;
		}
		case AI_SEARCHING: {

			if (mTarget == NULL) {
				mTarget = GetClosestPerson();
				if (mTarget == NULL) {
					SetAIState(AI_RANDOM);
					break;
				}
			}
			if (mTargetNode == NULL) {
				float dx = mTarget->mX-mX;
				float dy = mTarget->mY-mY;
				float distance = dx*dx + dy*dy;

				if (mNode == NULL) {
					mNode = mAStar->GetClosestNode(this);
				}
				if (mNode == NULL) {
					SetAIState(AI_IDLE);
					break;
				}

				Node* endnode = mTarget->mTargetNode; //mAStar->GetClosestNode(mTarget);
				if (endnode == NULL) {
					endnode = mAStar->GetClosestNode(mTarget);
				}
				if (endnode == NULL) break;

				mPath = mAStar->GetPath(mNode,endnode,(int)sqrtf(distance)*15);
				mTargetNode = mPath.back();
				mPath.pop_back();
			}

			if (mAIStateTime > 100.0f) {
				if (mTargetNode != NULL) {
					float dx = mTargetNode->mX-mX;
					float dy = mTargetNode->mY-mY;
					float distance = dx*dx + dy*dy;
					if (distance < 1000) {
						mNode = mTargetNode;
						if (mPath.size() > 0) {
							mTargetNode = mPath.back();
							mPath.pop_back();
						}
						else {
							float dx = mTarget->mX-mX;
							float dy = mTarget->mY-mY;
							float distance = dx*dx + dy*dy;

							Node* endnode = mTarget->mTargetNode; //mAStar->GetClosestNode(mTarget);
							if (endnode == NULL) {
								endnode = mAStar->GetClosestNode(mTarget);
							}
							if (endnode == NULL) break;

							mPath = mAStar->GetPath(mNode,endnode,(int)sqrtf(distance)*15);
							mTargetNode = mPath.back();
							mPath.pop_back();
						}
					}

					mFaceTargetX = mTargetNode->mX;
					mFaceTargetY = mTargetNode->mY;
				}

				mTarget = GetClosestPerson();

				if (mTarget != NULL) {
					float dx = mTarget->mX-mX;
					float dy = mTarget->mY-mY;
					float distance = dx*dx + dy*dy;

					if (distance < 50000) {
						mPath.clear();
						SetAIState(AI_ATTACKING);
						break;
					}
				}

				mAIStateTime = 0.0f;
			}

			mMoveTargetX = mTargetNode->mX;
			mMoveTargetY = mTargetNode->mY;

			break;
		}
		case AI_ATTACKING: {
			if (mTarget == NULL) {
				SetAIState(AI_SEARCHING);
				break;
			}
			if (mTargetNode != NULL) {
				float dx = mTargetNode->mX-mX;
				float dy = mTargetNode->mY-mY;
				float distance = dx*dx + dy*dy;
				if (distance < 500) {
					mNode = mTargetNode;
					if (mPath.size() > 0) {
						mTargetNode = mPath.back();
						mPath.pop_back();
					}
					else {
						Node* endnode = mTarget->mTargetNode; //mAStar->GetClosestNode(mTarget);
						if (endnode == NULL) {
							endnode = mAStar->GetClosestNode(mTarget);
						}
						if (endnode == NULL) break;

						mPath = mAStar->GetPath(mNode,endnode);
						mTargetNode = mPath.back();
						mPath.pop_back();
					}
				}
				mMoveTargetX = mTargetNode->mX;
				mMoveTargetY = mTargetNode->mY;
			}

			mFaceTargetX = mTarget->mX;
			mFaceTargetY = mTarget->mY;

			float dx = mTarget->mX-mX;
			float dy = mTarget->mY-mY;
			float distance = dx*dx + dy*dy;

			if (mCanSeeEnemy) {
				mFaceTargetX = mTarget->mX+mTarget->mSpeed*cosf(mTarget->mAngle)*(sqrtf(distance)/0.25f);
				mFaceTargetY = mTarget->mY+mTarget->mSpeed*sinf(mTarget->mAngle)*(sqrtf(distance)/0.25f);
			}
			if (distance > 50000) {
				SetAIState(AI_SEARCHING);
				break;
			}
			if (mTarget->mState == DEAD) {
				SetAIState(AI_SEARCHING);
				mTarget = NULL;
				break;
			}
			if (mAIStateTime > 100.0f) {
				if (GetClosestPerson() != mTarget) {
					mTarget = GetClosestPerson();
				}

				//Vector2D A(mX,mY);
				//Vector2D B(mTarget->mX,mTarget->mY);

				//Line line1(A,B);
				mCanSeeEnemy = true;
				mCanSeeEnemy = mGrid->LineOfSight(mX,mY,mTarget->mX,mTarget->mY);

				/*for (unsigned int i=0;i<mCollisionPoints->size()-1;i++) {
					if ((*mCollisionPoints)[i].bullets == false) continue;
					if ((*mCollisionPoints)[i].x == -1 || (*mCollisionPoints)[i+1].x == -1) continue;
					Vector2D C((*mCollisionPoints)[i].x,(*mCollisionPoints)[i].y);
					Vector2D D((*mCollisionPoints)[i+1].x,(*mCollisionPoints)[i+1].y);
					if (C == D) continue;
					Line line2(C,D);

					Vector2D d;
					if (LineLineIntersect(line1,line2,d)) {
						mCanSeeEnemy = false;
						break;
					}
				}*/
				mAIStateTime = 0.0f;
			}

			break;
		}
		case AI_FLASHED: {
			if (!mIsFlashed) {
				mTarget = NULL;
				mTargetNode = NULL;
				mNode = NULL;
				SetAIState(AI_SEARCHING);
			}
			if (mAIStateTime > 1000.0f+rand()%1000-500) {
				float angle = 2*M_PI*(rand()%100)/100.0f;
				mMoveTargetX = mX+cosf(angle)*1000;
				mMoveTargetY = mY+sinf(angle)*1000;

				angle = 2*M_PI*(rand()%100)/100.0f;
				mFaceTargetX = mX+cosf(angle);
				mFaceTargetY = mY+sinf(angle);
				mAIStateTime = 0.0f;
			}


			break;
		}
	}

	if (fabs(mFaceTargetX-mX) >= EPSILON || fabs(mFaceTargetY-mY) >= EPSILON) {
		float e = atan2f(mFaceTargetY-mY,mFaceTargetX-mX);//+((double)rand()/((double)RAND_MAX*2))-0.25;
		float diffangle = e-mFacingAngle;
		if (diffangle < -M_PI) {
			diffangle += M_PI*2;
		}
		else if (diffangle > M_PI) {
			diffangle -= M_PI*2;
		}
		RotateFacing(diffangle*(0.003f*dt));
	}


	if (fabs(mMoveTargetX-mX) >= EPSILON || fabs(mMoveTargetY-mY) >= EPSILON) {
		float e = atan2f(mMoveTargetY-mY,mMoveTargetX-mX);//+((double)rand()/((double)RAND_MAX*2))-0.25;
		float diffangle = e-mAngle;
		if (diffangle < -M_PI) {
			diffangle += M_PI*2;
		}
		else if (diffangle > M_PI) {
			diffangle -= M_PI*2;
		}

		mAngle += diffangle*(0.005f*dt);
		//SetAngle(GetAngle()+(((float)rand()/RAND_MAX)-0.5f)/50*dt);
		//SetRotation(GetRotation()+(((float)rand()/RAND_MAX)-0.5f)/50*dt);
		Move(.08f,mAngle+M_PI_2);
		//SetSpeed((float)rand()/RAND_MAX/10);
	}

	if (mAIState == AI_ATTACKING && mCanSeeEnemy) {
		mFireTime += dt;
		if (mFireTime >= 500+rand()%500-250) {
			if (mState != ATTACKING) {
				Fire();
			}
			if (mFireTime >= 1000+rand()%1000-500) {
				mFireTime = 0.0f;
			}
		}
	}
	else {
		StopFire();
	}

	if (mState != RELOADING) {
		if (mGuns[mGunIndex]->mClipAmmo == 0) {
			if (!Reload()) {
				Drop(mGunIndex);
			}
		}
	}

	if (mIsInBuyZone && mBuyGun != NULL && mMoney >= mBuyGun->mCost) {
		if (mBuyGun->mType == PRIMARY) {
			mMoney -= mBuyGun->mCost;
			Drop(PRIMARY);
			GunObject *gun = new GunObject(mBuyGun,mBuyGun->mClip,0);
			PickUp(gun);
			//mPlayer->mGuns[PRIMARY] = gun;
			//mPlayer->mGunIndex = PRIMARY;
			gSfxManager->PlaySample(gPickUpSound, mX, mY);
			mBuyGun = NULL;
		}
	}

	Person::Update(dt);
}


//------------------------------------------------------------------------------------------------
void AI::Render(float x, float y)
{
	Person::Render(x,y);
}


//------------------------------------------------------------------------------------------------
void AI::Reset()
{
	std::vector<Person*> mValidTargets;
	for (unsigned int i=0; i<mPeople->size(); i++) {
		if ((*mPeople)[i]->mTeam == mTeam || (*mPeople)[i]->mTeam == NONE) continue;
		mValidTargets.push_back((*mPeople)[i]);
	}

	mTarget = mValidTargets[rand()%mValidTargets.size()];

	mTargetNode = NULL;
	mNode = NULL;
	mPath.clear();
	SetAIState(AI_SEARCHING);
	mCanSeeEnemy = false;
	
	mStuckTimer = 0.0f;

	if (mBuyGun == NULL) {
		mBuyGun = &(*mGameGuns)[7+rand()%18];
	}

	Person::Reset();
}


//------------------------------------------------------------------------------------------------
void AI::SetAIState(int state)
{
	if (mAIState != state) {
		mAIState = state;
		mAIStateTime = 0.0f;
	}
}


//------------------------------------------------------------------------------------------------
Person* AI::GetClosestPerson()
{
	Person* person = NULL;
	float dist = -1.0f;
	for (unsigned int i=0; i<mPeople->size(); i++) {
		if ((*mPeople)[i]->mState != DEAD && (*mPeople)[i]->mTeam != mTeam) {
			float dx = (*mPeople)[i]->mX-mX;
			float dy = (*mPeople)[i]->mY-mY;
			float disttemp = dx*dx+dy*dy;
			if (disttemp < dist || dist < 0.0f) {
				person = (*mPeople)[i];
				dist = disttemp;
			}
		}
	}
	return person;
}
