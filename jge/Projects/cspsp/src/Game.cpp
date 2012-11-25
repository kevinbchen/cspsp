
#include "Game.h"

//------------------------------------------------------------------------------------------------
Game::Game(GameApp* parent): GameState(parent) 
{
	mSpecState = FREELOOK;
	mSpecDead = false;

	mRespawnTime = 5;
	mRoundTimer = 0.0f;
	mRoundEndTimer = 0.0f;
	mBuyTimer = 0.0f;
	mTimer = 0.0f;
	mSwitchTimer = 0;
	//mMusic = NULL;
	//mFont = NULL;
	mGuiController = NULL;
	mSpec = NULL;
	cross = false;
	mIsOnline = false;
	mSort = false;
	mTimeMultiplier = 1.0f;
	mIsTeamOnlyChat = false;

	mGameType = TEAM;
	mRespawnTimer = 3000.0f;
	mFFAWinner = NULL;
	mChatTimer = 0.0f;
	mIsChatEnabled = true;
}

//------------------------------------------------------------------------------------------------
Game::~Game()
{
	//JGERelease();
}

//------------------------------------------------------------------------------------------------
void Game::Init()
{
	mSpecX = 0;
	mSpecY = 0;

	mMap = new TileMap(&mGunObjects);
	mGrid = new Grid();

	mCamera = new Camera(0,0);

	mHud = new Hud();
	mHud->mIsOnline = mIsOnline;
	mHud->mGameType = &mGameType;

	mWinner = NONE;

	/*mGuiController = new JGuiController(150, this);
	mGuiController->SetActive(false);
	if (mGuiController)
	{	
		mGuiController->Add(new MenuItem(1, gFont, "RESUME", 20, 165, TYPE_EXPANDING, JGETEXT_LEFT, true));
		mGuiController->Add(new MenuItem(2, gFont, "RESTART ROUND", 20, 190, TYPE_EXPANDING, JGETEXT_LEFT));
		mGuiController->Add(new MenuItem(3, gFont, "RETURN TO MENU", 20, 215, TYPE_EXPANDING, JGETEXT_LEFT));
		mGuiController->Add(new MenuItem(4, gFont, "QUIT GAME", 20, 240, TYPE_EXPANDING, JGETEXT_LEFT));
	}*/
	mNumPlayers = 0;
	mNumCTs = 0;
	mNumTs = 0;
	mNumRemainingCTs = 0;
	mNumRemainingTs = 0;

	mNumRounds = 0;
	mNumCTWins = 0;
	mNumTWins = 0;

	mNumFlags[CT] = 0;
	mNumFlags[T] = 0;
	mFlagX[CT] = 0.0f;
	mFlagY[CT] = 0.0f;
	mFlagX[T] = 0.0f;
	mFlagY[T] = 0.0f;
	mIsFlagHome[CT] = true;
	mIsFlagHome[T] = true;
	mFlagHolder[CT] = NULL;
	mFlagHolder[T] = NULL;

	mTeamMenu = new TeamMenu();
	mTeamMenu->cross = &cross;
	mTeamMenu->mGameType = &mGameType;

	mIsMapChanging = false;
	mMapChangeTimer = 5000.0f;

	mScoreState = 0;
	mHitTime = 0;
}

//------------------------------------------------------------------------------------------------
void Game::End()
{
	for (unsigned int i=0;i<mPeople.size();i++) {
		delete mPeople[i];
	}
	mPeople.clear();

	for (unsigned int i=0;i<mBullets.size();i++) {
		delete mBullets[i];
	}
	mBullets.clear();

	for (unsigned int i=0;i<mGunObjects.size();i++) {
		delete mGunObjects[i];
	}
	mGunObjects.clear();

	mMap->Unload();
	mGrid->ClearCells(true);

	if (mBuyMenu != NULL) {
		delete mBuyMenu;
	}

	mRenderer->EnableVSync(false);

	mHud->Reset();

	for (int i=0; i<3; i++) {
		gParticleSystems[i]->Stop(true);
	}

	mGuiController->SetCurr(0);
}

//------------------------------------------------------------------------------------------------
void Game::CheckCollisions()
{
	//for (unsigned int cell=0; cell<mGrid->mCells.size(); cell++) {
	//	std::vector<Person*>* mPeopleTemp = &(mGrid->mCells[cell].mPeople);
	//	std::vector<GunObject*>* mGunObjectsTemp = &(mGrid->mCells[cell].mGunObjects);
	//	std::vector<Bullet*>* mBulletsTemp = &(mGrid->mCells[cell].mBullets);

	//	for(unsigned int i=0; i<mPeopleTemp->size(); i++) {
	//		Person* person1 = (*mPeopleTemp)[i];
	//		if (person1 == NULL) continue;
	//		if (person1->mState == DEAD || person1->mTeam == NONE) continue;
	//		for(unsigned int j=0; j<mPeopleTemp->size(); j++) {
	//			Person* person2 = (*mPeopleTemp)[j];
	//			if (person1 == NULL) continue;
	//			if (person2->mState == DEAD || person2->mTeam == NONE) continue;
	//			if (person1 != person2) {
	//				float x = person1->mX;
	//				float y = person1->mY;
	//				if (person1->mIsPlayerOnline) {
	//					x = ((PersonOnline*)person1)->mSX;
	//					y = ((PersonOnline*)person1)->mSY;
	//				}
	//				float x2 = person2->mX;
	//				float y2 = person2->mY;
	//				if (person2->mIsPlayerOnline) {
	//					x2 = ((PersonOnline*)person2)->mSX;
	//					y2 = ((PersonOnline*)person2)->mSY;
	//				}
	//				float dx = x-x2;
	//				float dy = y-y2;

	//				if (fabs(dx) < EPSILON && fabs(dy) < EPSILON) continue; //underflow

	//				float dist = dx*dx+dy*dy;
	//				float r = 16*2;
	//				if (dist < 35*35) {
	//					if (mFriendlyFire == ON || person2->mTeam != person1->mTeam) {
	//						if (person1->mGunIndex == KNIFE && person1->mState == ATTACKING) {
	//							float angle = atan2f((y2-y),x2-x)+M_PI;
	//							float anglediff = fabs(fabs(angle-person1->mFacingAngle)-M_PI);
	//							if (anglediff <= 0.5f) {
	//								person1->mState = DRYFIRING;
	//								gParticleEngine->GenerateParticles(BLOOD,x2,y2,5);
	//								mMap->AddDecal(x2,y2,DECAL_BLOOD);
	//								gSfxManager->PlaySample(gKnifeHitSound,x,y);
	//								if (!mIsOnline) {
	//									person2->TakeDamage(person1->mGuns[KNIFE]->mGun->mDamage);
	//									if (person2->mState == DEAD) {
	//										UpdateScores(person1,person2,person1->mGuns[KNIFE]->mGun);
	//									}
	//									if (mSpec->mState == DEAD) {
	//										NewSpec(person1,i);
	//									}
	//								}
	//							}
	//						}
	//						if (person2->mState == DEAD) continue; //just in case the person dies in above if statement
	//						if (person2->mGunIndex == KNIFE && person2->mState == ATTACKING) {
	//							float angle = atan2f((y-y2),x-x2)+M_PI;
	//							float anglediff = fabs(fabs(angle-person2->mFacingAngle)-M_PI);
	//							if (anglediff <= 0.5f) {
	//								person2->mState = DRYFIRING;
	//								gParticleEngine->GenerateParticles(BLOOD,x,y,5);
	//								mMap->AddDecal(x,y,DECAL_BLOOD);
	//								gSfxManager->PlaySample(gKnifeHitSound,x2,y2);
	//								if (!mIsOnline) {
	//									person1->TakeDamage(person2->mGuns[KNIFE]->mGun->mDamage);
	//									if (person1->mState == DEAD) {
	//										UpdateScores(person2,person1,person2->mGuns[KNIFE]->mGun);
	//									}
	//									if (mSpec->mState == DEAD) {
	//										NewSpec(person2,j);
	//									}
	//								}
	//							}
	//						}
	//						if (person1->mState == DEAD) break; //just in case the person dies in above if statement
	//					}
	//					if (dist < r*r) {
	//						//if (mPeople[i]->mSpeed != 0.0f) {
	//							//mPeople[i]->SetMoveState(NOTMOVING);
	//							//mPeople[j]->SetMoveState(NOTMOVING);
	//							//mPeople[i]->SetSpeed(0.0f);
	//							//mPeople[j]->SetSpeed(0.0f);
	//							float length = sqrtf(dx*dx + dy*dy); //HERE
	//							dx /= length;
	//							dy /= length;
	//							dx *= r;
	//							dy *= r;
	//							float totalspeed = person1->mSpeed + person2->mSpeed;
	//							person1->mX = x2 + dx;
	//							person1->mY = y2 + dy;
	//							person2->mX = x - dx;
	//							person2->mY = y - dy;

	//							if (person1->mIsPlayerOnline) {
	//								((PersonOnline*)person1)->mSX = x2 + dx;
	//								((PersonOnline*)person1)->mSY = y2 + dy;	
	//							}
	//							if (person2->mIsPlayerOnline) {
	//								((PersonOnline*)person2)->mSX = x - dx;
	//								((PersonOnline*)person2)->mSY = y - dy;	
	//							}

	//							//doesn't work well when more than 2 collisions together
	//							/*
	//							mPeopleTemp.erase(mPeopleTemp.begin()+j);
	//							j--;
	//							*/
	//						//}
	//					}
	//				}
	//			}
	//		}

	//		if (person1->mState == DEAD) continue;//continue loop if person died above^

	//		Vector2D pos(person1->mX,person1->mY);
	//		Circle circle(pos,16);

	//		for(unsigned int j=0; j<mBulletsTemp->size(); j++) {
	//			if (person1->mState == DEAD) break;//break loop if person died above^
	//			Bullet* bullet = (*mBulletsTemp)[j];

	//			if (bullet->mType == TYPE_GRENADE) continue;
	//			if (bullet->dead) continue;
	//			if (bullet->pX == bullet->mX && bullet->pY == bullet->mY) continue;
	//			if (bullet->mParent == NULL) continue;
	//			
	//			Vector2D p1(bullet->mX,bullet->mY);
	//			Vector2D p2(bullet->pX,bullet->pY);
	//			Line l1(p2,p1);
	//			if (fabs(p1.x-p2.x) < EPSILON && fabs(p1.y-p2.y) < EPSILON) continue; //underflow

	//			if (bullet->mParent == person1) continue; // prevent checking a bullet against its parent
	//			if (mFriendlyFire == OFF && bullet->mParent->mTeam == person1->mTeam) continue;

	//			Vector2D d;
	//			float l;
	//			if (LineCircleIntersect2(l1,circle,d,l)) {
	//				gParticleEngine->GenerateParticles(BLOOD,d.x,d.y,5);
	//				mMap->AddDecal(bullet->mX,bullet->mY,DECAL_BLOOD);
	//				gSfxManager->PlaySample(gHitSounds[rand()%3],bullet->mX,bullet->mY);
	//				bullet->dead = true;
	//				if (!mIsOnline) {
	//					person1->TakeDamage(bullet->mDamage);
	//					if (person1->mState == DEAD) {
	//						UpdateScores(bullet->mParent,person1,bullet->mParentGun);
	//					}
	//					if (mSpec->mState == DEAD) {
	//						NewSpec(bullet->mParent,i);
	//					}
	//				}
	//			}
	//		}

	//		if (person1->mState == DEAD) continue;//continue loop if person died above^

	//		if (!mIsOnline) {
	//			for (unsigned int j=0; j<mGunObjectsTemp->size(); j++) {
	//				GunObject* gunobject = (*mGunObjectsTemp)[j];
	//				//if (mGunObjects[j] == NULL) continue;
	//				if (!gunobject->mOnGround) continue;
	//				float x = person1->mX;
	//				float y = person1->mY;
	//				float x2 = gunobject->mX;
	//				float y2 = gunobject->mY;
	//				float dx = x-x2;
	//				float dy = y-y2;
	//				float dist = dx*dx+dy*dy;
	//				float r = 16;
	//				if (dist < r*r) {
	//					if (person1->PickUp(gunobject)) {
	//						for (unsigned int k=0; k<mGunObjects.size(); k++) {
	//							if (mGunObjects[k] == gunobject) {
	//								mGunObjects.erase(mGunObjects.begin()+k);
	//								break;
	//							}
	//						}
	//						mGunObjectsTemp->erase(mGunObjectsTemp->begin()+j);
	//						j--;
	//					}
	//				}
	//			}
	//		}

	//		person1->mIsInBuyZone = false;
	//		std::vector<BuyZone>* buyzones = NULL;
	//		if (person1->mTeam == CT) {
	//			buyzones = &(mMap->mCTBuyZones);
	//		}
	//		else if (person1->mTeam == T) {
	//			buyzones = &(mMap->mTBuyZones);
	//		}
	//		if (buyzones != NULL) {
	//			for (int j=0; j<buyzones->size(); j++) {
	//				if (person1->mX >= (*buyzones)[j].x1 && person1->mX <= (*buyzones)[j].x2 && person1->mY >= (*buyzones)[j].y1 && person1->mY <= (*buyzones)[j].y2) {
	//					person1->mIsInBuyZone = true;
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	for (unsigned int i=0;i<mGrid->mCells[cell].mCollisionLines.size();i++) {
	//		Line l1 = mGrid->mCells[cell].mCollisionLines[i]->line;
	//		//if (l1.x1 == l1.x2 && l1.y1 == l1.y2) continue;
	//		if (fabs(l1.x1-l1.x2) < EPSILON && fabs(l1.y1-l1.y2) < EPSILON) continue;

	//		if (mGrid->mCells[cell].mCollisionLines[i]->people == true) {
	//			for(unsigned int j=0; j<mPeopleTemp->size(); j++) {
	//				Person* person = (*mPeopleTemp)[j];
	//				if (person == NULL) continue;

	//				Vector2D pos(person->mX,person->mY);
	//				Vector2D oldpos(person->mOldX,person->mOldY);
	//				Circle circle(pos,16);
	//				Line l2(pos,oldpos);

	//				Vector2D d;
	//				if (LineLineIntersect(l1,l2,d,true)) {
	//					Vector2D dir = oldpos-pos;
	//					dir.Normalize();
	//					//dir *= 16;
	//					person->mX = d.x+dir.x;
	//					person->mY = d.y+dir.y;
	//					person->mOldX = (d.x+dir.x);
	//					person->mOldY = (d.y+dir.y);
	//					pos.x = person->mX;
	//					pos.y = person->mY;
	//					circle.x = pos.x;
	//					circle.y = pos.y;
	//				}

	//				float l;
	//				if (LineCircleIntersect(l1,circle,d,l,true)) {
	//					pos += d * (circle.radius - l);
	//					//person->mX = pos.x;//-6*cosf(mPeople[j]->mFacingAngle));
	//					//person->mY = pos.y;//-6*sinf(mPeople[j]->mFacingAngle));
	//					//mPeople[j]->mOldX = pos.x;
	//					//mPeople[j]->mOldY = pos.y;
	//				}

	//				if (person->mIsPlayerOnline) {
	//					Vector2D pos(((PersonOnline*)mPlayer)->mSX,((PersonOnline*)mPlayer)->mSY);
	//					Circle circle(pos,16);
	//					Line l2(pos,oldpos);
	//					if (LineLineIntersect(l1,l2,d,true)) {
	//						Vector2D dir = oldpos-pos;
	//						dir.Normalize();
	//						dir *= 16;
	//						((PersonOnline*)mPlayer)->mSX = d.x+dir.x;
	//						((PersonOnline*)mPlayer)->mSY = d.y+dir.y;
	//						//mPeople[j]->mOldX = (d.x+dir.x);
	//						//mPeople[j]->mOldY = (d.y+dir.y);
	//						pos.x = ((PersonOnline*)mPlayer)->mSX;
	//						pos.y = ((PersonOnline*)mPlayer)->mSY;
	//						circle.x = pos.x;
	//						circle.y = pos.y;
	//					}

	//					float l;
	//					if (LineCircleIntersect(l1,circle,d,l,true)) {
	//						pos += d * (circle.radius - l);
	//						((PersonOnline*)mPlayer)->mSX = pos.x;//-6*cosf(mPeople[j]->mFacingAngle));
	//						((PersonOnline*)mPlayer)->mSY = pos.y;//-6*sinf(mPeople[j]->mFacingAngle));
	//						//mPeople[j]->mOldX = pos.x;
	//						//mPeople[j]->mOldY = pos.y;
	//					}
	//				}
	//			}
	//			for(unsigned int j=0; j<mGunObjectsTemp->size(); j++) {
	//				GunObject* gunobject = (*mGunObjectsTemp)[j];

	//				if (gunobject == NULL) continue;
	//				//if (mGunObjects[j]->mOnGround) continue;

	//				Vector2D pos(gunobject->mX,gunobject->mY);
	//				Vector2D oldpos(gunobject->mOldX,gunobject->mOldY);
	//				Circle circle(pos,8);
	//				Line l2(pos,oldpos);

	//				Vector2D d;
	//				if (LineLineIntersect(l1,l2,d,true)) {
	//					Vector2D dir = oldpos-pos;
	//					dir.Normalize();
	//					dir *= 5;
	//					gunobject->mX = d.x+dir.x;
	//					gunobject->mY = d.y+dir.y;
	//					pos.x = gunobject->mX;
	//					pos.y = gunobject->mY;
	//					circle.x = pos.x;
	//					circle.y = pos.y;
	//				}

	//				float l;
	//				if (LineCircleIntersect(l1,circle,d,l,true)) {
	//					pos += d * (circle.radius - l);
	//					gunobject->mX = pos.x;
	//					gunobject->mY = pos.y;
	//				}

	//				if (gunobject->mIsOnline) {
	//					Vector2D pos(((GunObjectOnline*)gunobject)->mSX,((GunObjectOnline*)gunobject)->mSY);
	//					Circle circle(pos,8);
	//					Line l2(pos,oldpos);

	//					Vector2D d;
	//					if (LineLineIntersect(l1,l2,d,true)) {
	//						Vector2D dir = oldpos-pos;
	//						dir.Normalize();
	//						dir *= 5;
	//						((GunObjectOnline*)gunobject)->mSX = d.x+dir.x;
	//						((GunObjectOnline*)gunobject)->mSY = d.y+dir.y;
	//						pos.x = ((GunObjectOnline*)gunobject)->mSX;
	//						pos.y = ((GunObjectOnline*)gunobject)->mSY;
	//						circle.x = pos.x;
	//						circle.y = pos.y;
	//					}

	//					float l;
	//					if (LineCircleIntersect(l1,circle,d,l,true)) {
	//						pos += d * (circle.radius - l);
	//						((GunObjectOnline*)gunobject)->mSX = pos.x;
	//						((GunObjectOnline*)gunobject)->mSY = pos.y;
	//					}
	//				}
	//			}
	//		}
	//		if (mGrid->mCells[cell].mCollisionLines[i]->bullets == true) {
	//			for(unsigned int k=0; k<mBulletsTemp->size(); k++) {
	//				Bullet* bullet = (*mBulletsTemp)[k];

	//				if (bullet == NULL) continue;
	//				if (bullet->dead) continue;

	//				Vector2D pos(bullet->mX,bullet->mY);
	//				Vector2D oldpos(bullet->pX,bullet->pY);
	//				Line l2(pos,oldpos);

	//				Vector2D d;
	//				if (LineLineIntersect(l1,l2,d,true)) {
	//					if (bullet->mType == TYPE_BULLET) {
	//						gParticleEngine->GenerateParticles(BULLETIMPACT,d.x,d.y,2);
	//						gSfxManager->PlaySample(gRicochetSounds[rand()%4],d.x,d.y);
	//						bullet->dead = true;		
	//					}
	//					else if (bullet->mType == TYPE_GRENADE) {
	//						/*float wallangle = atan2f(l1.x2-l1.x1,l1.y2-l1.y1);
	//						float diffangle = bullet->mAngle-wallangle;
	//						bullet->mAngle = M_PI_2-((M_PI_2-wallangle)+diffangle);
	//						bullet->mAngle -= M_PI;*/

	//						Vector2D velocity(cosf(bullet->mAngle),sinf(bullet->mAngle));
	//						Vector2D normal(-(l1.y2-l1.y1),l1.x2-l1.x1);
	//						normal.Normalize();
	//						
	//						if (velocity.Dot(normal) > 0.0f) {
	//							normal *= -1.0f;
	//						}
	//						Vector2D velocity2 = velocity-2*normal*(normal.Dot(velocity));
	//						bullet->SetAngle(atan2f(velocity2.y,velocity2.x));
	//						//bullet->mAngle = atan2f(velocity2.y,velocity2.x);

	//						bullet->mX = d.x+cosf(bullet->mAngle);
	//						bullet->mY = d.y+sinf(bullet->mAngle);
	//						bullet->pX = d.x+cosf(bullet->mAngle);
	//						bullet->pY = d.y+sinf(bullet->mAngle);

	//						gParticleEngine->GenerateParticles(BULLETIMPACT,d.x,d.y,2);
	//						gSfxManager->PlaySample(gGrenadeBounceSound,d.x,d.y);
	//						//mBullets[k]->dead = true;		
	//					}
	//				}
	//			}
	//		}
	//	}

	//}



	for(unsigned int i=0; i<mBullets.size(); i++) {
		Bullet* bullet = mBullets[i];

		if (bullet->mState != 0) continue;
		if (bullet->pX == bullet->mX && bullet->pY == bullet->mY) continue;
		if (bullet->mParent == NULL) continue;
		
		Vector2D p1(bullet->mX,bullet->mY);
		Vector2D p2(bullet->pX,bullet->pY);
		Line l1(p2,p1);
		if (fabs(p1.x-p2.x) < EPSILON && fabs(p1.y-p2.y) < EPSILON) continue; //underflow

		int gridWidth = mGrid->mWidth;
		int gridHeight = mGrid->mHeight;
		int cellSize = mGrid->mCellSize;

		int cellX = p2.x*mGrid->mConversion;///mGrid->mCellSize;
		int cellY = p2.y*mGrid->mConversion;///mGrid->mCellSize;

		int cellX2 = p1.x*mGrid->mConversion;
		int cellY2 = p1.y*mGrid->mConversion;

		float vX = bullet->cosAngle;
		float vY = bullet->sinAngle;

		int dirX = (vX >= 0.0f) ? 1 : -1;
		int dirY = (vY >= 0.0f) ? 1 : -1;
		float tX = 0.0f;
		float tY = 0.0f;

		float stepX = 0.0f;
		float stepY = 0.0f;

		if (fabs(vX) > EPSILON) {
			if (dirX >= 0.0f) {
				tX = ((cellX+1)*cellSize-p2.x)/vX+1;
			}
			else {
				tX = ((cellX)*cellSize-p2.x)/vX+1;
			}
			stepX = cellSize/fabs(vX);
		}
		else {
			tX = 100000;
			stepX = 100000;
		}

		if (fabs(vY) > EPSILON) {
			if (dirY >= 0.0f) {
				tY = ((cellY+1)*cellSize-p2.y)/vY+1;
			}
			else {
				tY = ((cellY)*cellSize-p2.y)/vY+1;
			}
			stepY = cellSize/fabs(vY);
		}
		else {
			tY = 100000;
			stepY = 100000;
		}

		while (true) {
			if (cellX < 0 || cellX >= gridWidth || cellY < 0 || cellY >= gridHeight) break;

			int cell = cellY*gridWidth + cellX;

			int intersected = 0;
			float maxT = (tX < tY) ? tX*tX : tY*tY;
			Line line(0,0,0,0);
			Vector2D d;

			for (unsigned int j=0; j<mGrid->mCells[cell].mCollisionLines.size(); j++) {
				if (mGrid->mCells[cell].mCollisionLines[j]->bullets != !!true) continue;

				Line l2 = mGrid->mCells[cell].mCollisionLines[j]->line;
				if (fabs(l2.x1-l2.x2) < EPSILON && fabs(l2.y1-l2.y2) < EPSILON) continue;

				Vector2D d2;
				if (LineLineIntersect(l1,l2,d2,true)) {
					float t = (p2-d2).LengthSquared();
					if (t < maxT) {
						maxT = t;
						line = l2;
						d = d2;
						intersected = 1;
						//break;
					}
				}
			}
			

			Person *p = NULL;

			if (bullet->mType == TYPE_BULLET) {
				for (unsigned int j=0; j<mGrid->mCells[cell].mPeople.size(); j++) {
					Person *person = mGrid->mCells[cell].mPeople[j];

					if (person->mState == DEAD) continue;
					if (bullet->mParent == person) continue; // prevent checking a bullet against its parent
					if (mGameType != FFA && mFriendlyFire == OFF && bullet->mParent->mTeam == person->mTeam) continue;

					Circle circle(person->mX,person->mY,16);
					Vector2D d2;
					float l;

					float dx = circle.x-p2.x;
					float dy = circle.y-p2.y;

					if (dx*dx+dy*dy < 16*16) {
						d2.x = person->mX;
						d2.y = person->mY;
						float t = (p2-d2).LengthSquared();
						if (t < maxT) {
							maxT = t;
							d = d2;
							intersected = 2;
							p = person;
							//break;
						}
					}
					else {
						if (LineCircleIntersect2(l1,circle,d2,l)) {
							float t = (p2-d2).LengthSquared();
							if (t < maxT) {
								maxT = t;
								d = d2;
								intersected = 2;
								p = person;
								//break;
							}
						}
					}
				}
			}


			if (intersected == 1) {					
				if (bullet->mType == TYPE_BULLET) {
					gParticleEngine->GenerateParticles(BULLETIMPACT,d.x,d.y,2);
					gSfxManager->PlaySample(gRicochetSounds[rand()%4],d.x,d.y);
					bullet->mEndX = d.x;
					bullet->mEndY = d.y;
					bullet->mState = 1;	
				}
				else if (bullet->mType == TYPE_GRENADE) {
					/*float wallangle = atan2f(l1.x2-l1.x1,l1.y2-l1.y1);
					float diffangle = bullet->mAngle-wallangle;
					bullet->mAngle = M_PI_2-((M_PI_2-wallangle)+diffangle);
					bullet->mAngle -= M_PI;*/

					Vector2D velocity(bullet->cosAngle,bullet->sinAngle);
					Vector2D normal(-(line.y2-line.y1),line.x2-line.x1);
					normal.Normalize();
					
					if (velocity.Dot(normal) > 0.0f) {
						normal *= -1.0f;
					}
					Vector2D velocity2 = velocity-2*normal*(normal.Dot(velocity));
					bullet->SetAngle(atan2f(velocity2.y,velocity2.x));
					//bullet->mAngle = atan2f(velocity2.y,velocity2.x);

					bullet->mX = d.x+cosf(bullet->mAngle);
					bullet->mY = d.y+sinf(bullet->mAngle);
					bullet->pX = d.x+cosf(bullet->mAngle);
					bullet->pY = d.y+sinf(bullet->mAngle);

					gParticleEngine->GenerateParticles(BULLETIMPACT,d.x,d.y,2);
					gSfxManager->PlaySample(gGrenadeBounceSound,d.x,d.y);
					//mBullets[k]->dead = true;		
				}
				break;
			}
			else if (intersected == 2) {
				gParticleEngine->GenerateParticles(BLOOD,d.x,d.y,5);
				mMap->AddDecal(d.x+20*vX,d.y+20*vY,DECAL_BLOOD);
				gSfxManager->PlaySample(gHitSounds[rand()%3],bullet->mX,bullet->mY);
				bullet->mEndX = d.x;
				bullet->mEndY = d.y;
				bullet->mState = 1;
				if (!mIsOnline) {
					if (bullet->mParent != NULL && bullet->mParent == mPlayer) {
						mHitTime = 1000;
						gSfxManager->PlaySample(gHitIndicatorSound);
					}
					if (p == mPlayer) {
						mHud->AddDamageIndicator(bullet->mAngle);
					}

					p->TakeDamage(bullet->mDamage);
					if (p->mState == DEAD) {
						UpdateScores(bullet->mParent,p,bullet->mParentGun);
					}
					if (mSpec->mState == DEAD) {
						NewSpec(bullet->mParent,i);
					}
				}
				break;
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
	}


	for (unsigned int i=0; i<mBullets.size(); i++) {
		if (mBullets[i]->mType == TYPE_GRENADE) continue; //don't delete grenades until later /explode

		if (mBullets[i]->mState == BULLET_DEAD) {
			Bullet* bullet = mBullets[i];
			mBullets[i] = mBullets.back();
			mBullets.back() = bullet;
			/*if (mDeadBullets.size() < 10) {
				mDeadBullets.push_back(bullet);
			}
			else {
				delete bullet;
			}*/
			delete bullet;
			mBullets.pop_back();
			i--;

			/*delete mBullets[i];
			mBullets.erase(mBullets.begin()+i);
			i--;*/
		}
	}

	for(unsigned int i=0; i<mPeople.size(); i++) {
		Person* person1 = mPeople[i];
		if (person1 == NULL) continue;
		if (person1->mState == DEAD || person1->mTeam == NONE) continue;

		int minx = 0;
		int miny = 0;
		int maxx = 0;
		int maxy = 0;

		if (!mIsOnline) {
			minx = (int)(mGrid->mConversion*(min2(person1->mX,person1->mOldX) - 16.0f));
			miny = (int)(mGrid->mConversion*(min2(person1->mY,person1->mOldY) - 16.0f));
			maxx = (int)(mGrid->mConversion*(max2(person1->mX,person1->mOldX) + 16.0f));
			maxy = (int)(mGrid->mConversion*(max2(person1->mY,person1->mOldY) + 16.0f));
		}
		else {
			minx = (int)(mGrid->mConversion*(min2(min2(person1->mX,person1->mOldX),min2(((PersonOnline*)person1)->mSX,((PersonOnline*)person1)->mOldSX)) - 16.0f));
			miny = (int)(mGrid->mConversion*(min2(min2(person1->mY,person1->mOldY),min2(((PersonOnline*)person1)->mSY,((PersonOnline*)person1)->mOldSY)) - 16.0f));
			maxx = (int)(mGrid->mConversion*(max2(max2(person1->mX,person1->mOldX),max2(((PersonOnline*)person1)->mSX,((PersonOnline*)person1)->mOldSX)) + 16.0f));
			maxy = (int)(mGrid->mConversion*(max2(max2(person1->mY,person1->mOldY),max2(((PersonOnline*)person1)->mSY,((PersonOnline*)person1)->mOldSY)) + 16.0f));
		}

		minx = min2(max2(0,minx),mGrid->mWidth-1);
		miny = min2(max2(0,miny),mGrid->mHeight-1);
		maxx = min2(max2(0,maxx),mGrid->mWidth-1);
		maxy = min2(max2(0,maxy),mGrid->mHeight-1);



		for (int y=miny; y<=maxy; y++) {
			int celly = y*mGrid->mWidth;
			for (int x=minx; x<=maxx; x++) {
				int cell = celly+x;
				for(unsigned int j=0; j<mGrid->mCells[cell].mPeople.size(); j++) {
					Person* person2 = mGrid->mCells[cell].mPeople[j];
					
					if (person2->mState == DEAD || person2->mTeam == NONE) continue;
					if (person1 == person2) continue;

					float x = person1->mX;
					float y = person1->mY;
					if (person1->mIsPlayerOnline) {
						x = ((PersonOnline*)person1)->mSX;
						y = ((PersonOnline*)person1)->mSY;
					}
					float x2 = person2->mX;
					float y2 = person2->mY;
					if (person2->mIsPlayerOnline) {
						x2 = ((PersonOnline*)person2)->mSX;
						y2 = ((PersonOnline*)person2)->mSY;
					}
					float dx = x-x2;
					float dy = y-y2;

					if (fabs(dx) < EPSILON && fabs(dy) < EPSILON) continue; //underflow

					float dist = dx*dx+dy*dy;
					float r = 16*2;
					if (dist < 40*40) {
						if (mGameType == FFA || mFriendlyFire == ON || person2->mTeam != person1->mTeam) {
							if (person1->mGunIndex == KNIFE && person1->mState == ATTACKING) {
								float angle = atan2f((y2-y),x2-x);
								float anglediff = fabs(fabs(angle+M_PI-person1->mFacingAngle)-M_PI);
								if (anglediff <= 0.6f) {
									person1->mState = DRYFIRING;
									gParticleEngine->GenerateParticles(BLOOD,x2,y2,5);
									mMap->AddDecal(x2,y2,DECAL_BLOOD);
									gSfxManager->PlaySample(gKnifeHitSound,x,y);

									if (!mIsOnline) {
										if (person1 == mPlayer) {
											mHitTime = 1000;
											gSfxManager->PlaySample(gHitIndicatorSound);
										}
										if (person2 == mPlayer) {
											mHud->AddDamageIndicator(angle);
										}
										person2->TakeDamage(person1->mGuns[KNIFE]->mGun->mDamage);
										if (person2->mState == DEAD) {
											UpdateScores(person1,person2,person1->mGuns[KNIFE]->mGun);
										}
										if (mSpec->mState == DEAD) {
											NewSpec(person1,i);
										}
									}
								}
							}
							if (person2->mState == DEAD) continue; //just in case the person dies in above if statement
							if (person2->mGunIndex == KNIFE && person2->mState == ATTACKING) {
								float angle = atan2f((y-y2),x-x2);
								float anglediff = fabs(fabs(angle+M_PI-person2->mFacingAngle)-M_PI);
								if (anglediff <= 0.6f) {
									person2->mState = DRYFIRING;
									gParticleEngine->GenerateParticles(BLOOD,x,y,5);
									mMap->AddDecal(x,y,DECAL_BLOOD);
									gSfxManager->PlaySample(gKnifeHitSound,x2,y2);
									if (!mIsOnline) {
										if (person2 == mPlayer) {
											mHitTime = 1000;
											gSfxManager->PlaySample(gHitIndicatorSound);
										}
										if (person1 == mPlayer) {
											mHud->AddDamageIndicator(angle);
										}
										person1->TakeDamage(person2->mGuns[KNIFE]->mGun->mDamage);
										if (person1->mState == DEAD) {
											UpdateScores(person2,person1,person2->mGuns[KNIFE]->mGun);
										}
										if (mSpec->mState == DEAD) {
											NewSpec(person2,j);
										}
									}
								}
							}
							if (person1->mState == DEAD) break; //just in case the person dies in above if statement
						}
						if (dist < r*r) {
							//if (mPeople[i]->mSpeed != 0.0f) {
								//mPeople[i]->SetMoveState(NOTMOVING);
								//mPeople[j]->SetMoveState(NOTMOVING);
								//mPeople[i]->SetSpeed(0.0f);
								//mPeople[j]->SetSpeed(0.0f);
								float length = sqrtf(dx*dx + dy*dy); //HERE
								dx /= length;
								dy /= length;
								dx *= r;
								dy *= r;
								float totalspeed = person1->mSpeed + person2->mSpeed;
								person1->mX = x2 + dx;
								person1->mY = y2 + dy;
								person2->mX = x - dx;
								person2->mY = y - dy;

								if (person1->mIsPlayerOnline) {
									((PersonOnline*)person1)->mSX = x2 + dx;
									((PersonOnline*)person1)->mSY = y2 + dy;	
								}
								if (person2->mIsPlayerOnline) {
									((PersonOnline*)person2)->mSX = x - dx;
									((PersonOnline*)person2)->mSY = y - dy;	
								}

								//doesn't work well when more than 2 collisions together
								/*
								mPeopleTemp.erase(mPeopleTemp.begin()+j);
								j--;
								*/
							//}
						}
					}
				}

				if (person1->mState == DEAD) break;//continue loop if person died above^

				if (!mIsOnline) {
					for (unsigned int j=0; j<mGrid->mCells[cell].mGunObjects.size(); j++) {
						GunObject* gunobject = mGrid->mCells[cell].mGunObjects[j];
						//if (mGunObjects[j] == NULL) continue;
						if (!gunobject->mOnGround) continue;
						float x = person1->mX;
						float y = person1->mY;
						float x2 = gunobject->mX;
						float y2 = gunobject->mY;
						float dx = x-x2;
						float dy = y-y2;
						float dist = dx*dx+dy*dy;
						float r = 16;
						if (dist < r*r) {
							if (person1->PickUp(gunobject)) {
								for (unsigned int k=0; k<mGunObjects.size(); k++) {
									if (mGunObjects[k] == gunobject) {
										mGunObjects.erase(mGunObjects.begin()+k);
										break;
									}
								}
								mGrid->mCells[cell].mGunObjects.erase(mGrid->mCells[cell].mGunObjects.begin()+j);
								j--;
							}
						}
					}
				}

				if (person1 == mPlayer || !mIsOnline) { // don't really need to know for other (online)players
					person1->mIsInBuyZone = false;
					if (mGameType == FFA) {
						person1->mIsInBuyZone = true;
					}
					else {
						std::vector<BuyZone>* buyzones = NULL;
						if (person1->mTeam == CT || mGameType == FFA) {
							buyzones = &(mMap->mCTBuyZones);
						}
						else if (person1->mTeam == T) {
							buyzones = &(mMap->mTBuyZones);
						}
						if (buyzones != NULL) {
							for (int j=0; j<buyzones->size(); j++) {
								if (person1->mX >= (*buyzones)[j].x1 && person1->mX <= (*buyzones)[j].x2 && person1->mY >= (*buyzones)[j].y1 && person1->mY <= (*buyzones)[j].y2) {
									person1->mIsInBuyZone = true;
									break;
								}
							}
						}
					}
				}

				for (unsigned int j=0; j<mGrid->mCells[cell].mCollisionLines.size(); j++) {
					if (mGrid->mCells[cell].mCollisionLines[j]->people != true) continue;

					Line l1 = mGrid->mCells[cell].mCollisionLines[j]->line;
					//if (l1.x1 == l1.x2 && l1.y1 == l1.y2) continue;
					if (fabs(l1.x1-l1.x2) < EPSILON && fabs(l1.y1-l1.y2) < EPSILON) continue;

					Vector2D pos(person1->mX,person1->mY);
					Vector2D oldpos(person1->mOldX,person1->mOldY);
					Circle circle(pos,16);
					Line l2(pos,oldpos);

					Vector2D d;
					if (LineLineIntersect(l1,l2,d,true)) {
						Vector2D dir = oldpos-pos;
						dir.Normalize();
						//dir *= 16;
						person1->mX = d.x+dir.x;
						person1->mY = d.y+dir.y;
						//person1->mOldX = (d.x+dir.x);
						//person1->mOldY = (d.y+dir.y);
						pos.x = person1->mX;
						pos.y = person1->mY;
						circle.x = pos.x;
						circle.y = pos.y;
					}

					float l;
					if (LineCircleIntersect(l1,circle,d,l,true)) {
						pos += d * (circle.radius - l);
						person1->mX = pos.x;//-6*cosf(mPeople[j]->mFacingAngle));
						person1->mY = pos.y;//-6*sinf(mPeople[j]->mFacingAngle));
						//mPeople[j]->mOldX = pos.x;
						//mPeople[j]->mOldY = pos.y;
					}

					if (mIsOnline) {
						//if (person1->mIsPlayerOnline) {
							Vector2D pos(((PersonOnline*)person1)->mSX,((PersonOnline*)person1)->mSY);
							Vector2D oldpos(((PersonOnline*)person1)->mOldSX,((PersonOnline*)person1)->mOldSY);
							Circle circle(pos,16);
							Line l2(pos,oldpos);
							if (LineLineIntersect(l1,l2,d,true)) {
								Vector2D dir = oldpos-pos;
								dir.Normalize();
								//dir *= 16;
								((PersonOnline*)person1)->mSX = d.x+dir.x;
								((PersonOnline*)person1)->mSY = d.y+dir.y;
								//((PersonOnline*)person1)->mOldSX = d.x+dir.x;
								//((PersonOnline*)person1)->mOldSY = d.y+dir.y;
								//mPeople[j]->mOldX = (d.x+dir.x);
								//mPeople[j]->mOldY = (d.y+dir.y);
								pos.x = ((PersonOnline*)person1)->mSX;
								pos.y = ((PersonOnline*)person1)->mSY;
								circle.x = pos.x;
								circle.y = pos.y;
							}

							float l;
							if (LineCircleIntersect(l1,circle,d,l,true)) {
								pos += d * (circle.radius - l);
								((PersonOnline*)person1)->mSX = pos.x;//-6*cosf(mPeople[j]->mFacingAngle));
								((PersonOnline*)person1)->mSY = pos.y;//-6*sinf(mPeople[j]->mFacingAngle));
								//((PersonOnline*)mPlayer)->mOldSX = pos.x;
								//((PersonOnline*)mPlayer)->mOldSY = pos.y;
								//mPeople[j]->mOldX = pos.x;
								//mPeople[j]->mOldY = pos.y;
							}

							Line l3(((PersonOnline*)person1)->mSX,((PersonOnline*)person1)->mSY,((PersonOnline*)person1)->mX,((PersonOnline*)person1)->mY);
							if (LineLineIntersect(l1,l3,d,false)) {
								person1->mX = ((PersonOnline*)person1)->mSX;
								person1->mY = ((PersonOnline*)person1)->mSY;
								person1->mOldX = person1->mX;
								person1->mOldY = person1->mY;
							}

						/*}
						else {

						}*/
					}

				}

			}
			if (person1->mState == DEAD) break;
		}
	}

	for(unsigned int i=0; i<mGunObjects.size(); i++) {
		GunObject* gunobject = mGunObjects[i];

		if (gunobject == NULL) continue;
		if (gunobject->mOnGround) continue;

		int minx = (int)(mGrid->mConversion*(((gunobject->mX < gunobject->mOldX) ? gunobject->mX:gunobject->mOldX) - 8.0f));
		int miny = (int)(mGrid->mConversion*(((gunobject->mY < gunobject->mOldY) ? gunobject->mY:gunobject->mOldY) - 8.0f));
		int maxx = (int)(mGrid->mConversion*(((gunobject->mX > gunobject->mOldX) ? gunobject->mX:gunobject->mOldX) + 8.0f));
		int maxy = (int)(mGrid->mConversion*(((gunobject->mY > gunobject->mOldY) ? gunobject->mY:gunobject->mOldY) + 8.0f));

		minx = min2(max2(0,minx),mGrid->mWidth-1);
		miny = min2(max2(0,miny),mGrid->mHeight-1);
		maxx = min2(max2(0,maxx),mGrid->mWidth-1);
		maxy = min2(max2(0,maxy),mGrid->mHeight-1);


		for (int y=miny; y<=maxy; y++) {
			int celly = y*mGrid->mWidth;
			for (int x=minx; x<=maxx; x++) {
				int cell = celly+x;
	
				Vector2D pos(gunobject->mX,gunobject->mY);
				Vector2D oldpos(gunobject->mOldX,gunobject->mOldY);
				Circle circle(pos,8);
				Line l2(pos,oldpos);

				for (unsigned int j=0; j<mGrid->mCells[cell].mCollisionLines.size(); j++) {
					if (mGrid->mCells[cell].mCollisionLines[j]->people != true) continue;

					Line l1 = mGrid->mCells[cell].mCollisionLines[j]->line;
					//if (l1.x1 == l1.x2 && l1.y1 == l1.y2) continue;
					if (fabs(l1.x1-l1.x2) < EPSILON && fabs(l1.y1-l1.y2) < EPSILON) continue;

					Vector2D d;
					if (LineLineIntersect(l1,l2,d,true)) {
						Vector2D dir = oldpos-pos;
						dir.Normalize();
						//dir *= 5;
						gunobject->mX = d.x+dir.x;
						gunobject->mY = d.y+dir.y;
						pos.x = gunobject->mX;
						pos.y = gunobject->mY;
						circle.x = pos.x;
						circle.y = pos.y;
					}

					float l;
					if (LineCircleIntersect(l1,circle,d,l,true)) {
						pos += d * (circle.radius - l);
						gunobject->mX = pos.x;
						gunobject->mY = pos.y;
					}

					if (gunobject->mIsOnline) {
						Vector2D pos(((GunObjectOnline*)gunobject)->mSX,((GunObjectOnline*)gunobject)->mSY);
						Circle circle(pos,8);
						Line l2(pos,oldpos);

						Vector2D d;
						if (LineLineIntersect(l1,l2,d,true)) {
							Vector2D dir = oldpos-pos;
							dir.Normalize();
							dir *= 5;
							((GunObjectOnline*)gunobject)->mSX = d.x+dir.x;
							((GunObjectOnline*)gunobject)->mSY = d.y+dir.y;
							pos.x = ((GunObjectOnline*)gunobject)->mSX;
							pos.y = ((GunObjectOnline*)gunobject)->mSY;
							circle.x = pos.x;
							circle.y = pos.y;
						}

						float l;
						if (LineCircleIntersect(l1,circle,d,l,true)) {
							pos += d * (circle.radius - l);
							((GunObjectOnline*)gunobject)->mSX = pos.x;
							((GunObjectOnline*)gunobject)->mSY = pos.y;
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------------------------
void Game::CheckSpecInput(float dt)
{
	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
		if (mSpecState == FREELOOK) {
			if (mSpec->mState == DEAD) {
				mSpecState = THIRDPERSON;
				NewSpec(mSpec,mSpecIndex);
			}
			else {
				mSpecState = THIRDPERSON;
			}
		}
		else if (mSpecState == THIRDPERSON) {
			mSpecState = FREELOOK;
			mSpecX = mSpec->mX;
			mSpecY = mSpec->mY;
		}
	}
	if (mSpecState == FREELOOK) {
		float aX = mEngine->GetAnalogX()-127.5f;
		float aY = mEngine->GetAnalogY()-127.5f;
		/*if (mEngine->GetButtonState(PSP_CTRL_UP)) {
			mSpecY -= 0.3f*dt;
		}
		else if (mEngine->GetButtonState(PSP_CTRL_DOWN)) {
			mSpecY += 0.3f*dt;
		}
		else if (mEngine->GetButtonState(PSP_CTRL_RIGHT)) {
			mSpecX += 0.3f*dt;
		}
		else if (mEngine->GetButtonState(PSP_CTRL_LEFT)) {
			mSpecX -= 0.3f*dt;
		}*/
		if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
			mSpecX += aX*0.0023f*dt;
			mSpecY += aY*0.0023f*dt;
		}

		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			/*if (mSpec->mState != DEAD) {
				mSpecX = mSpec->mX;
				mSpecY = mSpec->mY;
			}*/

			float dist = -1.0f;
			for (int i=0; i<mPeople.size(); i++) {
				if (mPeople[i]->mState == DEAD) continue;
				if (mPeople[i]->mTeam == NONE) continue;

				float dx = mCamera->GetX()-mPeople[i]->mX;
				float dy = mCamera->GetY()-mPeople[i]->mY;
				float tempdist = dx*dx+dy*dy;
				if (tempdist < dist || dist < 0.0f) {
					mSpec = mPeople[i];
					mSpecIndex = i;
					dist = tempdist;
				}
			}
			if (dist >= 0.0f) {
				mSpecState = THIRDPERSON;
			}
		}

		if (mSpecX < mCamera->mMinX) {
			mSpecX = mCamera->mMinX;
		}
		else if (mSpecX > mCamera->mMaxX) {
			mSpecX = mCamera->mMaxX;
		}
		if (mSpecY < mCamera->mMinY) {
			mSpecY = mCamera->mMinY;
		}
		else if (mSpecY > mCamera->mMaxY) {
			mSpecY = mCamera->mMaxY;
		}
	}
	else if (mSpecState == THIRDPERSON) {
		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			bool nextSpecFound = false;
			int counter = 0;
			while (!nextSpecFound) {
				mSpecIndex++;
				counter++;
				if (mSpecIndex >= mPeople.size()) {
					mSpecIndex = 0;
				}
				if (mPeople[mSpecIndex]->mState != DEAD && mPeople[mSpecIndex]->mTeam != NONE) {
					mSpec = mPeople[mSpecIndex];
					nextSpecFound = true;
				}
				if (counter > 100) break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------
void Game::Update(float dt)
{
	mGrid->ClearCells();

	if (mPlayer->mState == DEAD) {
		if (mBuyMenu->mIsActive) {
			mBuyMenu->Disable();
		}
	}

	if (mWinner != NONE) {
		mRoundEndTimer += dt*0.001f*mTimeMultiplier;
	}

	mMap->Update(dt*mTimeMultiplier);
	gParticleEngine->Update(dt*mTimeMultiplier);

	if (mGameType == FFA || mGameType == CTF) {
		mRespawnTimer -= dt*mTimeMultiplier;
		if (mRespawnTimer < 0.0f) {
			mRespawnTimer = 0.0f;
		}
	}

	mRoundTimer -= dt*0.001f*mTimeMultiplier;
	if (mRoundState == FREEZETIME) {

		if (mRoundTimer < 0) {
			mRoundTimer = mRoundTime;
			mRoundState = STARTED;
			for(unsigned int i=0; i<mPeople.size(); i++)
			{
				mPeople[i]->mIsActive = true;
			}			
		}
	}
	else if (mRoundState != FREEZETIME) {
		if (mRoundTimer < 0) {
			mRoundTimer = 0;
			if (!mIsOnline) {
				if (mWinner == NONE) {
					if (mGameType == TEAM) {
						mWinner = TIE;
						gSfxManager->PlaySample(gRoundEndSounds[TIE]);
						mTimeMultiplier = 0.33f;
					}
				}
			}
		}
		mBuyTimer -= dt*0.001f*mTimeMultiplier;
		if (mBuyTimer < 0.0f) {
			mBuyTimer = -1.0f;
		}
	}

	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		if (mPeople[i]->mTeam == NONE) continue;
		//if (mPeople[i]->mState == DEAD) continue;
		mPeople[i]->PreUpdate(dt*mTimeMultiplier);
		mPeople[i]->Update(dt*mTimeMultiplier);
		mGrid->HashPerson(mPeople[i]);
	}
	for(unsigned int i=0; i<mBullets.size(); i++)
	{
		mBullets[i]->Update(dt*mTimeMultiplier);
		/*if (mBullets[i]->mType == TYPE_BULLET) {
			mBullets[i]->Update(dt);
		}
		else if (mBullets[i]->mType == TYPE_GRENADE) {
			((Grenade*)mBullets[i])->Update(dt);
		}*/
		//mGrid->HashBullet(mBullets[i]);
	}
	for(unsigned int i=0; i<mGunObjects.size(); i++)
	{
		//bool onGround = mGunObjects[i]->mOnGround;
		mGunObjects[i]->Update(dt*mTimeMultiplier);
		//if (!onGround) { //only hash if gun is moving
			mGrid->HashGunObject(mGunObjects[i]);
		//}
	}

	for (int i=0; i<3; i++) {
		gParticleSystems[i]->Transpose(SCREEN_WIDTH_2-mCamera->GetX(),SCREEN_HEIGHT_2-mCamera->GetY());
		gParticleSystems[i]->Update(dt/1000.0f*mTimeMultiplier);
	}


	mHud->Update(dt);

	if (mSwitchTimer != 0) {
		mSwitchTimer -= dt;
		if (mSwitchTimer < 0) {
			mSwitchTimer = 0;
		}
	}

	mHitTime -= dt*mTimeMultiplier;
	if (mHitTime < 0) {
		mHitTime = 0;
	}

	if (mSort) {
		std::sort(mPeople.begin(),mPeople.end(),SortByScore());
		mSort = false;
	}

	mChatTimer -= dt;
	if (mChatTimer < 0.0f) {
		mChatTimer = 0.0f;
		mIsChatEnabled = true;
	}
	else if (mChatTimer > 5000.0f) {
		mIsChatEnabled = false;
	}
}

//------------------------------------------------------------------------------------------------
void Game::UpdateCamera(float dt)
{
	
	if (mPlayer->mState != DEAD || mSpecState == THIRDPERSON) {
		// mSpec == mPlayer if player is still alive (hopefully...)
		int a = 500;
		float b = 0.5f;

		Gun *gun = mSpec->GetCurrentGun()->mGun;
		if (mSpec->mGunIndex == PRIMARY) {
			if (gun->mId == 19 ||gun->mId == 20) {
				a = 800;
			}
			else if (gun->mId == 21 || gun->mId == 22 || mPlayer->GetCurrentGun()->mGun->mId == 23) {
				a = 1400;
				b = 0.10f;
			}
			else if (gun->mId == 16) {
				a = 1100;
				b = 0.20f;
			}
		}
		//mCamera->mTX = mPlayer->mX + ((mPlayer->mX-mPlayer->mOldX)*b + cosf(mPlayer->mFacingAngle))/16.6f*a;//dt
		//mCamera->mTY = mPlayer->mY + ((mPlayer->mY-mPlayer->mOldY)*b + sinf(mPlayer->mFacingAngle))/16.6f*a;//dt
		mCamera->mTX = mSpec->mX + (cosf(mSpec->mAngle)*mSpec->mSpeed*b + cosf(mSpec->mFacingAngle)/16.6f)*a;//dt
		mCamera->mTY = mSpec->mY + (sinf(mSpec->mAngle)*mSpec->mSpeed*b + sinf(mSpec->mFacingAngle)/16.6f)*a;//dt
		mSpecX = mSpec->mX;
		mSpecY = mSpec->mY;
	}
	else if (mSpecState == FREELOOK) {
		mCamera->mTX = mSpecX;
		mCamera->mTY = mSpecY;
	}
	mCamera->Update(dt*mTimeMultiplier);
	gSfxManager->mX = mCamera->mX;
	gSfxManager->mY = mCamera->mY;
}

//------------------------------------------------------------------------------------------------
void Game::Render()
{
	char buffer[256];

	/*PIXEL_TYPE colors[] =
	{
		ARGB(255,0,0,255),
		ARGB(255,62,114,189),
		ARGB(255,255,255,255),
		ARGB(255,255,255,255)
	};*/
	// clear background
	
	mRenderer->ClearScreen(ARGB(255,255,255,255));
	//mEngine->FillRect(0.0f, 0.0f, SCREEN_WIDTH_F, SCREEN_HEIGHT_F, ARGB(255,255,255,255));	
	/**mEngine->RenderQuad(mMario, 80.0f, 80.0f);

	mEngine->RenderQuad(mMario, 400.0f, 80.0f, 1.0f, 1.2f, 1.2f);	// blit with rotation and scaling

	mEngine->RenderQuad(mMario, 80.0f, 200.0f, mAngle, 1.0f, 1.0f);	// blit with rotation

	mMario->SetColor(ARGB(128,255,255,0));								// semi-transparent yellow
	mEngine->RenderQuad(mMario, 380.0f, 180.0f, 0.0f, mScale, mScale);	// do some color blending ;)
	mMario->SetColor(ARGB(255,255,255,255));
	
	mEngine->RenderQuad(mHello, SCREEN_WIDTH_F/2.0f, SCREEN_HEIGHT_F/2.0f, 0.0f, mScale, mScale);	// Hello world!
	**/

	/**if (mPlayer->mState != DEAD) {
		x = mPlayer->GetX();
		y = mPlayer->GetY();
	}
	else {
		for (int i=1;i<mMap->mNumPeoplePerTeam*2;i++) {
			if (mPeople[i]->mState != DEAD) {
				x = mSpec->GetX();
				y = mSpec->GetY();
				//break;
			}
		}
	}**/
	float dx = mCamera->GetX();
	float dy = mCamera->GetY(); 
	
	mRenderer->EnableTextureFilter(false);
	mMap->Render(dx,dy);
	mRenderer->EnableTextureFilter(true);

	float minx = dx-SCREEN_WIDTH_2-1.0f;
	float miny = dx-SCREEN_HEIGHT_2-1.0f;
	float maxx = dx+SCREEN_WIDTH_2+1.0f;
	float maxy = dy+SCREEN_HEIGHT_2+1.0f;

	if (mGameType == CTF) {
		gFlagHomeQuad->SetColor(ARGB(255,0,0,255));
		mRenderer->RenderQuad(gFlagHomeQuad,mMap->mFlagSpawn[CT].x-(dx-SCREEN_WIDTH_2),mMap->mFlagSpawn[CT].y-(dy-SCREEN_HEIGHT_2));

		gFlagHomeQuad->SetColor(ARGB(255,255,0,0));
		mRenderer->RenderQuad(gFlagHomeQuad,mMap->mFlagSpawn[T].x-(dx-SCREEN_WIDTH_2),mMap->mFlagSpawn[T].y-(dy-SCREEN_HEIGHT_2));
	}

	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		if (mPeople[i]->mState != DEAD || mPeople[i]->mTeam == NONE) continue;
		if (mPeople[i]->mX < dx-SCREEN_WIDTH_2-64.0f || mPeople[i]->mX > dx+SCREEN_WIDTH_2+64.0f) continue;
		if (mPeople[i]->mY < dy-SCREEN_HEIGHT_2-64.0f || mPeople[i]->mY > dy+SCREEN_HEIGHT_2+64.0f) continue;

		mPeople[i]->Render(dx,dy);
	}
	for(unsigned int i=0; i<mGunObjects.size(); i++)
	{
		if (mGunObjects[i]->mX < dx-SCREEN_WIDTH_2-32.0f || mGunObjects[i]->mX > dx+SCREEN_WIDTH_2+32.0f) continue;
		if (mGunObjects[i]->mY < dy-SCREEN_HEIGHT_2-32.0f || mGunObjects[i]->mY > dy+SCREEN_HEIGHT_2+32.0f) continue;

		mGunObjects[i]->Render(dx,dy);
	}

	if (mGameType == CTF) {
		if (mFlagHolder[CT] == NULL) {
			gFlagQuad->SetColor(ARGB(255,153,204,255));
			mRenderer->RenderQuad(gFlagQuad,mFlagX[CT]-(dx-SCREEN_WIDTH_2),mFlagY[CT]-(dy-SCREEN_HEIGHT_2));
			//mRenderer->DrawCircle(mFlagX[CT]-(dx-SCREEN_WIDTH_2),mFlagY[CT]-(dy-SCREEN_HEIGHT_2),16,ARGB(255,0,0,255));
		}
		if (mFlagHolder[T] == NULL) {
			gFlagQuad->SetColor(ARGB(255,255,64,64));
			mRenderer->RenderQuad(gFlagQuad,mFlagX[T]-(dx-SCREEN_WIDTH_2),mFlagY[T]-(dy-SCREEN_HEIGHT_2));
			//mRenderer->DrawCircle(mFlagX[T]-(dx-SCREEN_WIDTH_2),mFlagY[T]-(dy-SCREEN_HEIGHT_2),16,ARGB(255,255,0,0));
		}
	}

	for(unsigned int i=0; i<mBullets.size(); i++)
	{
		//if (mBullets[i]->mX < dx-SCREEN_WIDTH_2-32.0f || mBullets[i]->mX > dx+SCREEN_WIDTH_2+32.0f) continue;
		//if (mBullets[i]->mY < dy-SCREEN_HEIGHT_2-32.0f || mBullets[i]->mY > dy+SCREEN_HEIGHT_2+32.0f) continue;

		mBullets[i]->Render(dx,dy);
		/*if (mBullets[i]->mType == TYPE_BULLET) {
			mBullets[i]->Render(dx,dy);
		}
		else if (mBullets[i]->mType == TYPE_GRENADE) {
			((Grenade*)mBullets[i])->Render(dx,dy);
		}*/
	}

	gFont->SetScale(0.7f);
	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		if (mPeople[i]->mState == DEAD || mPeople[i]->mTeam == NONE) continue;
		if (mPeople[i]->mX < dx-SCREEN_WIDTH_2-32.0f || mPeople[i]->mX > dx+SCREEN_WIDTH_2+32.0f) continue;
		if (mPeople[i]->mY < dy-SCREEN_HEIGHT_2-32.0f || mPeople[i]->mY > dy+SCREEN_HEIGHT_2+32.0f) continue;

		mPeople[i]->Render(dx,dy);
	}

	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		if (mPeople[i]->mState == DEAD || mPeople[i]->mTeam == NONE) continue;
		if (mPeople[i]->mX < dx-SCREEN_WIDTH_2-32.0f || mPeople[i]->mX > dx+SCREEN_WIDTH_2+32.0f) continue;
		if (mPeople[i]->mY < dy-SCREEN_HEIGHT_2-32.0f || mPeople[i]->mY > dy+SCREEN_HEIGHT_2+32.0f) continue;

		if ((mPlayer->mState == DEAD || (mGameType != FFA && mPeople[i]->mTeam == mPlayer->mTeam)) && mPeople[i] != mPlayer) {
			if (mGameType == FFA) {
				gFont->SetColor(ARGB(255,255,64,64));
			}
			else {
				if (mPeople[i]->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
				else if (mPeople[i]->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
			}
			float x = mPeople[i]->mX-dx+SCREEN_WIDTH_2;
			float y = mPeople[i]->mY-dy+SCREEN_HEIGHT_2-25.0f;
			gFont->DrawShadowedString(mPeople[i]->mName,x,y,JGETEXT_CENTER);
			if (mIsOnline) {
				mRenderer->RenderQuad(((PersonOnline*)mPeople[i])->mIconQuad,x-gFont->GetStringWidth(mPeople[i]->mName)/2 - 15, y+2);
			}
		}
	}

	gParticleEngine->Render(dx,dy);

	for (int i=0; i<3; i++) {
		gParticleSystems[i]->Render();
		if (i == PARTICLE_EXPLOSION || i == PARTICLE_FLASH) {
			mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE);
			gParticleSystems[i]->Render();
			mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
		}
	}

	if (mPlayer->mState != DEAD) {
		/*std::vector<Vector2D> p1;
		std::vector<Vector2D> p2;

		bool previous = false;
		for (int i=0; i<mMap->mCollisionLines.size(); i++) {

			Vector2D a(mPlayer->mX,mPlayer->mY);
			Vector2D b1(mMap->mCollisionLines[i].line.x1,mMap->mCollisionLines[i].line.y1);
			Vector2D d1 = b1-a;
			d1.Normalize();
			Vector2D c1 = b1 + d1*1000;

			Vector2D b2(mMap->mCollisionLines[i].line.x2,mMap->mCollisionLines[i].line.y2);
			Vector2D d2 = b2-a;
			d2.Normalize();
			Vector2D c2 = b2 + d2*1000;

			//b = (a-b).Normalize();
			if (mGrid->LineOfSight(a.x,a.y,b1.x-d1.x,b1.y-d1.y) || mGrid->LineOfSight(a.x,a.y,b2.x-d2.x,b2.y-d2.y)) {
				if ((c1-b1).Cross(c2-b2) < 0){

					float xarray[4];
					float yarray[4];

					xarray[0] = b1.x-(dx-SCREEN_WIDTH_2);
					yarray[0] = b1.y-(dy-SCREEN_HEIGHT_2);

					xarray[1] = c1.x-(dx-SCREEN_WIDTH_2);
					yarray[1] = c1.y-(dy-SCREEN_HEIGHT_2);

					xarray[2] = c2.x-(dx-SCREEN_WIDTH_2);
					yarray[2] = c2.y-(dy-SCREEN_HEIGHT_2);

					xarray[3] = b2.x-(dx-SCREEN_WIDTH_2);
					yarray[3] = b2.y-(dy-SCREEN_HEIGHT_2);

					mRenderer->FillPolygon(xarray,yarray,4,ARGB(100,0,0,0));
					
				}
				else {
					float xarray[4];
					float yarray[4];

					xarray[0] = b1.x-(dx-SCREEN_WIDTH_2);
					yarray[0] = b1.y-(dy-SCREEN_HEIGHT_2);

					xarray[3] = c1.x-(dx-SCREEN_WIDTH_2);
					yarray[3] = c1.y-(dy-SCREEN_HEIGHT_2);

					xarray[2] = c2.x-(dx-SCREEN_WIDTH_2);
					yarray[2] = c2.y-(dy-SCREEN_HEIGHT_2);

					xarray[1] = b2.x-(dx-SCREEN_WIDTH_2);
					yarray[1] = b2.y-(dy-SCREEN_HEIGHT_2);

					mRenderer->FillPolygon(xarray,yarray,4,ARGB(100,0,0,0));
				}
			}
		}*/

		if (mPlayer->mState != RELOADING) {
			float angle = mPlayer->GetCurrentGun()->mGun->mViewAngle;

			if (angle > 0.0f) {
				float xarray[6];
				float yarray[6];

				float facingangle = mPlayer->mFacingAngle;
				float x = mPlayer->mX - 5*cosf(facingangle)-(dx-SCREEN_WIDTH_2);
				float y = mPlayer->mY - 5*sinf(facingangle)-(dy-SCREEN_HEIGHT_2);
				
				xarray[0] = x;
				yarray[0] = y;

				xarray[1] = x + 500*cosf(facingangle-angle);
				yarray[1] = y + 500*sinf(facingangle-angle);

				xarray[2] = x + 500*cosf(facingangle-M_PI_2);
				yarray[2] = y + 500*sinf(facingangle-M_PI_2);

				xarray[3] = x + 500*cosf(facingangle-M_PI);
				yarray[3] = y + 500*sinf(facingangle-M_PI);

				xarray[4] = x + 500*cosf(facingangle+M_PI_2);
				yarray[4] = y + 500*sinf(facingangle+M_PI_2);

				xarray[5] = x + 500*cosf(facingangle+angle);
				yarray[5] = y + 500*sinf(facingangle+angle);

				mRenderer->FillPolygon(xarray,yarray,6,ARGB(175,0,0,0));
				//mRenderer->FillCircle(tx,ty,5,ARGB(255,255,0,0));
			}
		}


		/*if (mPlayer->mState == RELOADING) {
			//mRenderer->DrawLine(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,SCREEN_WIDTH_2+25.0f,SCREEN_HEIGHT_2-5.0f,ARGB(255,255,200,0));
			//mRenderer->DrawLine(SCREEN_WIDTH_2+25.0f,SCREEN_HEIGHT_2-5.0f,SCREEN_WIDTH_2+25.0f,SCREEN_HEIGHT_2+5.0f,ARGB(255,255,200,0));
			//mRenderer->DrawLine(SCREEN_WIDTH_2+25.0f,SCREEN_HEIGHT_2+5.0f,SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2+5.0f,ARGB(255,255,200,0));
			//mRenderer->DrawLine(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2+5.0f,SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,ARGB(255,255,200,0));
			mRenderer->DrawRect(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,50.0f,10.0f,ARGB(255,255,200,0));
			mRenderer->FillRect(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,mPlayer->mStateTime/(mPlayer->GetCurrentGun()->mGun->mReloadDelay)*50.0f,10.0f,ARGB(200,255,200,0));
			gFont->SetColor(ARGB(255,255,200,0));
			gFont->SetScale(0.5f);
			gFont->DrawString("RELOADING", SCREEN_WIDTH_2,SCREEN_HEIGHT_2+10.0f, JGETEXT_CENTER);
		}*/
		if (mPlayer->mState == ATTACKING && mPlayer->mGunIndex == GRENADE) {
			/*mRenderer->DrawRect(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,50.0f,10.0f,ARGB(255,255,200,0));
			mRenderer->FillRect(SCREEN_WIDTH_2-25.0f,SCREEN_HEIGHT_2-5.0f,mPlayer->mStateTime/(mPlayer->GetCurrentGun()->mGun->mDelay)*50.0f,10.0f,ARGB(200,255,200,0));
			gFont->SetColor(ARGB(255,255,200,0));
			gFont->SetScale(0.5f);
			gFont->DrawString("THROWING", SCREEN_WIDTH_2,SCREEN_HEIGHT_2+10.0f, JGETEXT_CENTER);*/

			float xarray[3];
			float yarray[3];

			float x = mPlayer->mX + 10*cosf(mPlayer->mFacingAngle)-(dx-SCREEN_WIDTH_2);
			float y = mPlayer->mY + 10*sinf(mPlayer->mFacingAngle)-(dy-SCREEN_HEIGHT_2);

			float angle = 0.20f;
			xarray[0] = x;
			yarray[0] = y;
			xarray[1] = x + 50*cosf(mPlayer->mFacingAngle+angle);
			yarray[1] = y + 50*sinf(mPlayer->mFacingAngle+angle);
			xarray[2] = x + 50*cosf(mPlayer->mFacingAngle-angle);
			yarray[2] = y + 50*sinf(mPlayer->mFacingAngle-angle);

			mRenderer->FillPolygon(xarray,yarray,3,ARGB(150,0,0,0));

			float d = mPlayer->mStateTime/(mPlayer->GetCurrentGun()->mGun->mDelay)*50;
			xarray[0] = x;
			yarray[0] = y;
			xarray[1] = x + d*cosf(mPlayer->mFacingAngle+angle);
			yarray[1] = y + d*sinf(mPlayer->mFacingAngle+angle);
			xarray[2] = x + d*cosf(mPlayer->mFacingAngle-angle);
			yarray[2] = y + d*sinf(mPlayer->mFacingAngle-angle);

			mRenderer->FillPolygon(xarray,yarray,3,ARGB(255,255,255,255));
		}

		float x = mPlayer->mX-(dx-SCREEN_WIDTH_2); //+sinf(mPlayer->mFacingAngle)*1.5f
		float y = mPlayer->mY-(dy-SCREEN_HEIGHT_2); //-cosf(mPlayer->mFacingAngle)*1.5f

		float angle = mPlayer->mFacingAngle-mPlayer->mRecoilAngle*0.5f;
		mRenderer->DrawLine(x+cosf(angle)*30,y+sinf(angle)*30,x+cosf(angle)*40,y+sinf(angle)*40,ARGB(200,0,255,0));
		angle = mPlayer->mFacingAngle+mPlayer->mRecoilAngle*0.5f;
		mRenderer->DrawLine(x+cosf(angle)*30,y+sinf(angle)*30,x+cosf(angle)*40,y+sinf(angle)*40,ARGB(200,0,255,0));

		int t = (mHitTime-750)/250*255;
		if (t > 0) {
			angle = mPlayer->mFacingAngle-mPlayer->mRecoilAngle*0.5f-.1f;
			mRenderer->DrawLine(x+cosf(angle)*30,y+sinf(angle)*30,x+cosf(angle)*45,y+sinf(angle)*45,ARGB(t,255,0,0));
			angle = mPlayer->mFacingAngle+mPlayer->mRecoilAngle*0.5f+.1f;
			mRenderer->DrawLine(x+cosf(angle)*30,y+sinf(angle)*30,x+cosf(angle)*45,y+sinf(angle)*45,ARGB(t,255,0,0));
		}
	}
	// draw some text
	//gFont->SetColor(ARGB(255,0,0,0));
	//gFont->DrawString("zomg its teh cs111", 240.0f, 20.0f, JGETEXT_CENTER);
	/**
	#ifdef WIN32
	_itoa_s(mPlayer->mHealth, buffer, 10);
	#else
	snprintf(buffer,32,"%i",mPlayer->mHealth); 
	#endif**/
	if (mPlayer->mState != DEAD) {
		sprintf(buffer,"%i",mPlayer->mHealth); 
		//gHudFont->SetColor(ARGB(230,255,200,0));
		//gHudFont->SetScale(2.0f);
		//gHudFont->DrawString(buffer, 10.0f, SCREEN_HEIGHT_F-40.0f, JGETEXT_LEFT);
		
		int height = 44*(mPlayer->mHealth/100.0f);
		if (height < 1) height = 1;
		gHealthFillQuad->SetTextureRect(48,44-height+2,48,height);
		mRenderer->RenderQuad(gHealthBorderQuad,10,SCREEN_HEIGHT-58);
		mRenderer->RenderQuad(gHealthFillQuad,10,SCREEN_HEIGHT-58+44-height+2);

		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.7f);
		gFont->DrawShadowedString(buffer, 10+((mPlayer->mHealth == 100)?22:24), SCREEN_HEIGHT_F-40.0f, JGETEXT_CENTER);

		//gHudFont->SetScale(1.5f);

		gFont->SetScale(1.0f);
		gFont->SetColor(ARGB(230,255,64,64));

		int seconds = (int)floorf(mRoundTimer);
		int minutes = (int)floorf(seconds/60.0f);
		int centiseconds = (int)floorf((mRoundTimer-seconds)*100);
		sprintf(buffer,"%02d:%02d.%02d",minutes,seconds%60,centiseconds);

		//gHudFont->SetColor(ARGB(230,255,64,64));
		//gHudFont->DrawString(buffer, SCREEN_WIDTH_2, SCREEN_HEIGHT-30, JGETEXT_CENTER);

		gFont->DrawShadowedString(buffer, SCREEN_WIDTH_2, SCREEN_HEIGHT-25, JGETEXT_CENTER);

		/*if (mPlayer->GetCurrentGun() != NULL) {
			gHudFont->SetColor(ARGB(230,255,200,0));
			if (mPlayer->mGunIndex == KNIFE) {
			}
			else if (mPlayer->mGunIndex == GRENADE) {
				sprintf(buffer,"%i",mPlayer->GetCurrentGun()->mClipAmmo); 
				gHudFont->DrawString(buffer, SCREEN_WIDTH-10, SCREEN_HEIGHT-30, JGETEXT_RIGHT);
			}
			else {
				sprintf(buffer,"%i",mPlayer->GetCurrentGun()->mClipAmmo); 
				gHudFont->DrawString(buffer, SCREEN_WIDTH-80, SCREEN_HEIGHT-30, JGETEXT_RIGHT);

				gHudFont->DrawString("|", SCREEN_WIDTH-70, SCREEN_HEIGHT-30, JGETEXT_CENTER);

				sprintf(buffer,"%i",mPlayer->GetCurrentGun()->mRemainingAmmo); 
				gHudFont->DrawString(buffer, SCREEN_WIDTH-10, SCREEN_HEIGHT-30, JGETEXT_RIGHT);
			}
		}
	
		gHudFont->SetColor(ARGB(230,0,255,0));
		sprintf(buffer,"$%i",mPlayer->mMoney); 
		gHudFont->DrawString(buffer, SCREEN_WIDTH-10, SCREEN_HEIGHT-60, JGETEXT_RIGHT);*/

		mRenderer->RenderQuad(gAmmoBarQuad,SCREEN_WIDTH-10,SCREEN_HEIGHT-10);
		//mRenderer->FillRect(SCREEN_WIDTH-(10+40+4),SCREEN_HEIGHT-(10+64+3*32),38,3*32,ARGB(150,100,100,100));
		int index = mPlayer->mGunIndex;
		if (mPlayer->mGuns[index] != NULL) {
			JQuad* quad = mPlayer->mGuns[index]->mGun->mGroundQuad;
			mRenderer->RenderQuad(quad,SCREEN_WIDTH-(22+10),SCREEN_HEIGHT-(22+10+20),0.0f,1.4f,1.4f);
		}
		int y = 0;
		for (int i=0; i<4; i++) {
			index++;
			if (index >= 5) {
				index = 0;
			}
			//mRenderer->FillRect(80+i*37,5,32,32,ARGB(100,255,200,0));
			if (mPlayer->mGuns[index] != NULL) {
				
				//mRenderer->FillRect(80+i*37,37,32,32,ARGB(150,255,200,0));
				int alpha = 255-25*y;
				float scale = 1.0f;
				/*PIXEL_TYPE colors[] =
				{
					ARGB(200,200,200,200),
					ARGB(200,200,200,200),
					ARGB(200,100,100,100),
					ARGB(200,100,100,100)
				};*/
				mRenderer->FillRect(SCREEN_WIDTH-(22+10+18),SCREEN_HEIGHT-(64+10+20+20*y),36,19,ARGB(200,100,100,100));
				JQuad* quad = mPlayer->mGuns[index]->mGun->mGroundQuad;
				//quad->SetColor(ARGB(alpha,255,255,255));
				mRenderer->RenderQuad(quad,SCREEN_WIDTH-(22+10),SCREEN_HEIGHT-(64+10+10+20*y));
				//quad->SetColor(ARGB(255,255,255,255));
				y++;
			}
		}
		GunObject *currentGun = mPlayer->GetCurrentGun();
		if (currentGun != NULL) {
			if (mPlayer->mState == ATTACKING || mPlayer->mState == SWITCHING) {
				mRenderer->FillRect(SCREEN_WIDTH-(44+10),SCREEN_HEIGHT-(8+10+20),40,4,ARGB(255,50,50,50));
				float width = 40.0f;
				float delay = currentGun->mGun->mDelay;
				if (mPlayer->mState == SWITCHING) {
					delay *= 0.75f;
					if (delay < 150.0f) delay = 150.0f;
				}
				if (delay > EPSILON) {
					width *= mPlayer->mStateTime/delay;
				}
				mRenderer->FillRect(SCREEN_WIDTH-(44+10)-width+40,SCREEN_HEIGHT-(8+10+20),width,4,ARGB(255,150,150,150));
			}
			else if (mPlayer->mState == RELOADING) {
				mRenderer->FillRect(SCREEN_WIDTH-(44+10),SCREEN_HEIGHT-(8+10+20),40,4,ARGB(255,50,50,50));
			}
			else {
				mRenderer->FillRect(SCREEN_WIDTH-(44+10),SCREEN_HEIGHT-(8+10+20),40,4,ARGB(255,150,150,150));
			}

			gFont->SetColor(ARGB(255,255,255,255));
			gFont->SetScale(0.7f);
			if (mPlayer->mGunIndex == KNIFE) {
				//mRenderer->FillRect(SCREEN_WIDTH-10-128+4,SCREEN_HEIGHT-29,120,15,ARGB(255,50,50,50));
				mRenderer->FillRect(SCREEN_WIDTH-14-120,SCREEN_HEIGHT-29,120,15,ARGB(255,255,255,255));
				gFont->DrawShadowedString("-", SCREEN_WIDTH-14-50, SCREEN_HEIGHT-45, JGETEXT_RIGHT);
				gFont->DrawShadowedString("-|", SCREEN_WIDTH-14-50-25, SCREEN_HEIGHT-45, JGETEXT_RIGHT);
			}
			else {
				if (mPlayer->mState == RELOADING) {
					float width = 120.0f;
					if (currentGun->mGun->mReloadDelay > EPSILON) {
						width *= mPlayer->mStateTime/(currentGun->mGun->mReloadDelay);
					}
					mRenderer->FillRect(SCREEN_WIDTH-10-128+4,SCREEN_HEIGHT-29,120,15,ARGB(255,50,50,50));
					mRenderer->FillRect(SCREEN_WIDTH-14-width,SCREEN_HEIGHT-29,width,15,ARGB(255,255,255,255));
					gFont->DrawShadowedString("reloading", SCREEN_WIDTH-10-64,SCREEN_HEIGHT-28,JGETEXT_CENTER);
				}
				else {
					int clip = currentGun->mGun->mClip;
					int w = currentGun->mGun->mAmmoBarWidth;

					if (w != 0) {
						for (int i=1; i<=clip; i++) {
							DWORD color = ARGB(255,50,50,50);
							if (i <= currentGun->mClipAmmo) {
								color = ARGB(255,255,255,255);
							}
							mRenderer->FillRect(SCREEN_WIDTH-14-(w+1)*i,SCREEN_HEIGHT-29,w,15,color);
						}
					}
					else {
						float width = ((float)currentGun->mClipAmmo/clip)*120;
						mRenderer->FillRect(SCREEN_WIDTH-10-128+4,SCREEN_HEIGHT-29,120,15,ARGB(255,50,50,50));
						mRenderer->FillRect(SCREEN_WIDTH-14-width,SCREEN_HEIGHT-29,width,15,ARGB(255,255,255,255));
					}
				}

				sprintf(buffer,"%d",currentGun->mRemainingAmmo); 
				gFont->DrawShadowedString(buffer, SCREEN_WIDTH-14-50, SCREEN_HEIGHT-45, JGETEXT_RIGHT);

				sprintf(buffer,"%d|",currentGun->mClipAmmo); 
				gFont->DrawShadowedString(buffer, SCREEN_WIDTH-14-50-25, SCREEN_HEIGHT-45, JGETEXT_RIGHT);

			}
		}
		/*if (mSwitchTimer > 0.0f) {
			for (int i=0; i<5; i++) {
				mRenderer->FillRect(80+i*37,5,32,32,ARGB(100,255,200,0));
				if (mPlayer->mGunIndex == i && mPlayer->mGuns[i] != NULL) {
					mRenderer->FillRect(80+i*37,37,32,32,ARGB(150,255,200,0));
					mPlayer->GetCurrentGun()->mGun->mGroundQuad->SetColor(ARGB(255,255,128,0));
					mRenderer->RenderQuad(mPlayer->GetCurrentGun()->mGun->mGroundQuad,96+i*37,53);
					mPlayer->GetCurrentGun()->mGun->mGroundQuad->SetColor(ARGB(255,255,255,255));
				}
			}
		}*/
		
		sprintf(buffer,"$%d",mPlayer->mMoney);
		gFont->SetScale(1.0f);

		gBuyZoneQuad->SetColor(ARGB(200,0,0,0));
		mRenderer->RenderQuad(gBuyZoneQuad,81,11);

		if (mPlayer->mIsInBuyZone) {
			gBuyZoneQuad->SetColor(ARGB(255,100,200,100));
			gFont->SetColor(ARGB(255,100,200,100));
		}
		else {
			gBuyZoneQuad->SetColor(ARGB(200,128,128,128));
			gFont->SetColor(ARGB(200,128,128,128));
		}
		gFont->DrawShadowedString(buffer,100,10);
		mRenderer->RenderQuad(gBuyZoneQuad,80,10);
		
		//mRenderer->FillCircle(40,40,35,ARGB(175,0,255,0));
		//mRenderer->DrawLine(5,40,75,40,ARGB(175,153,153,153));
		//mRenderer->DrawLine(40,5,40,75,ARGB(175,153,153,153));

		float factor = 32/480.0f;

		if (mMap->mOverviewQuad != NULL) {
			float x = dx*factor-31;
			float y = dy*factor-31;
			float w = mMap->mOverviewWidth;
			if (-x < 0) w -= fabs(x);
			else if (-x+w > 62) w = 62-fabs(x);
			if (w > 62) w = 62;

			float h = mMap->mOverviewHeight;
			if (-y < 0) h -= fabs(y);
			else if (-y+h > 62) h = 62-fabs(y);
			if (h > 62) h = 62;

			mMap->mOverviewQuad->SetTextureRect((x<0)?0:x,(y<0)?0:y,w,h);
			if (x > 0) x = 0;
			if (y > 0) y = 0;
			mRenderer->RenderQuad(mMap->mOverviewQuad,10-x+1,10-y+1);
		}
		mRenderer->RenderQuad(gRadarQuad,10,10);
		//mRenderer->FillRect(10,10,64,64,ARGB(128,0,100,0));
		//mRenderer->DrawRect(10+16,10+23,32,18,ARGB(255,255,255,255));

		for (unsigned int i=0;i<mPeople.size();i++) {
			if (mPeople[i]->mState == DEAD) continue;
			if (mPeople[i]->mTeam == NONE) continue;
			if (mGameType != FFA && mPeople[i]->mTeam != mPlayer->mTeam && mPeople[i]->mRadarTime == 0.0f) continue;
			//if (mPeople[i] == mPlayer) continue;

			float x = 0.0f;
			float y = 0.0f;

			if (mPeople[i]->mTeam == mPlayer->mTeam) {
				x = (mPeople[i]->mX-dx)*factor;
				y = (mPeople[i]->mY-dy)*factor;
			}
			else {
				x = (mPeople[i]->mRadarX-dx)*factor;
				y = (mPeople[i]->mRadarY-dy)*factor;
			}

			if (x > 32) {
				x = 32;
			}
			else if (x < -32) {
				x = -32;
			}
			if (y > 32) {
				y = 32;
			}
			else if (y < -32) {
				y = -32;
			}

			if (mPeople[i] == mPlayer) {
				mRenderer->FillPolygon(10+32+x,10+32+y,5,3,mPlayer->mFacingAngle,ARGB(255,255,255,255));
			}
			else if (mGameType != FFA && mPeople[i]->mTeam == mPlayer->mTeam) {
				mRenderer->FillPolygon(10+32+x,10+32+y,5,3,mPeople[i]->mFacingAngle,ARGB(255,0,255,0));
			}
			else {
				int alpha = mPeople[i]->mRadarTime/1000.0f*255; 
				if (alpha > 255) alpha = 255;
				//mRenderer->FillCircle(10+32+x,10+32+y,1.5f,ARGB(alpha,255,0,0));
				//mRenderer->FillPolygon(10+32+x,10+32+y,5,3,mPeople[i]->mFacingAngle,ARGB(alpha,255,0,0));
				mRenderer->FillRect(10+32+x-1,10+32+y-1,3,3,ARGB(alpha,255,0,0));
			}
			/*if (mPlayer->mTeam == CT) {
				mRenderer->FillRect(40.5f+cosf(theta)*dist,40.5f+sinf(theta)*dist,3,3,ARGB(255,153,204,255));
			}
			else if (mPlayer->mTeam == T) {
				mRenderer->FillRect(40.5f+cosf(theta)*dist,40.5f+sinf(theta)*dist,3,3,ARGB(255,255,64,64));
			}*/
		}

		if (mGameType == CTF) {
			for (int i=0; i<=1; i++) {
				if (mFlagHolder[i] != NULL) {
					mFlagX[i] = mFlagHolder[i]->mX;
					mFlagY[i] = mFlagHolder[i]->mY;
				}

				float x = (mFlagX[i]-dx)*factor;
				float y = (mFlagY[i]-dy)*factor;

				if (x > 32) {
					x = 32;
				}
				else if (x < -32) {
					x = -32;
				}
				if (y > 32) {
					y = 32;
				}
				else if (y < -32) {
					y = -32;
				}

				if (i == CT) {
					gFlagRadarQuad->SetColor(ARGB(255,153,204,255));
				}
				else {
					gFlagRadarQuad->SetColor(ARGB(255,255,64,64));
				}
				mRenderer->RenderQuad(gFlagRadarQuad,10+32+x,10+32+y);
			}

			gFont->SetScale(0.7f);
			for (int i=0; i<=1; i++) {
				float dx = mFlagX[i]-mCamera->mX;
				float dy = mFlagY[i]-mCamera->mY;
				if (fabs(dx) < SCREEN_WIDTH_2 && fabs(dy) < SCREEN_HEIGHT_2) continue;
				float angle = atan2f(dy,dx);
				
				float x = 90*cosf(angle)+SCREEN_WIDTH_2;
				float y = 90*sinf(angle)+SCREEN_HEIGHT_2;

				gScoreIconQuads[CTF]->SetColor(ARGB(200,0,0,0));
				gFlagArrowQuad->SetColor(ARGB(200,0,0,0));

				mRenderer->RenderQuad(gScoreIconQuads[CTF],x+1,y+1);
				mRenderer->RenderQuad(gFlagArrowQuad,x+1,y+1,angle+M_PI_2);

				if (i == CT) {
					gScoreIconQuads[CTF]->SetColor(ARGB(255,153,204,255));
					gFlagArrowQuad->SetColor(ARGB(255,153,204,255));
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else {
					gScoreIconQuads[CTF]->SetColor(ARGB(255,255,64,64));
					gFlagArrowQuad->SetColor(ARGB(255,255,64,64));
					gFont->SetColor(ARGB(255,255,64,64));
				}
				mRenderer->RenderQuad(gScoreIconQuads[CTF],x,y);
				mRenderer->RenderQuad(gFlagArrowQuad,x,y,angle+M_PI_2);

				float x2 = 98*cosf(angle)+SCREEN_WIDTH_2;
				float y2 = 98*sinf(angle)+SCREEN_HEIGHT_2;

				if (i == mPlayer->mTeam) {
					if (mIsFlagHome[i]) {
						sprintf(buffer,"%s","Defend");
					}
					else {
						if (mFlagHolder[i] == NULL) {
							sprintf(buffer,"%s","Return");
						}
						else {
							sprintf(buffer,"%s","Kill");
						}
					}
				}
				else {
					if (mIsFlagHome[i]) {
						sprintf(buffer,"%s","Capture");
					}
					else {
						if (mFlagHolder[i] == NULL) {
							sprintf(buffer,"%s","Capture");
						}
						else {
							sprintf(buffer,"%s","Escort");
						}
					}
				}
				gFont->DrawShadowedString(buffer,x2,y2-25,JGETEXT_CENTER);
			}
		}
		
		/**if (mRoundStartTime < 2000.0f) {
			gFont->SetColor(ARGB(255,255,200,0));
			gFont->SetScale(2.0f + mRoundStartTime/1000.0f);
			gFont->DrawString("Round Start", 240.0f, 100.0f, JGETEXT_CENTER);
		}**/
	}
	else {
		if (mSpecState == THIRDPERSON) {
			if (mSpec->mIsFlashed) {
				float alpha = 0.0f;
				if (mSpec->mFlashTime >= 10000.0f) {
					alpha = 1.0f;
				}
				else {
					alpha = mSpec->mFlashTime/10000.0f;
				}
				alpha *= 255;
				mRenderer->FillRect(0.0f,0.0f,SCREEN_WIDTH_F,SCREEN_HEIGHT_F,ARGB((int)alpha,255,255,255));
			}
		}

		gFont->SetScale(0.9f);
		if (mSpecState != NONE) {
			mRenderer->FillRect(0, 0, 480, 35, ARGB(200,0,0,0));
			mRenderer->FillRect(0, SCREEN_HEIGHT-35, 480, 35, ARGB(200,0,0,0));
			//mRenderer->DrawLine(0,35,SCREEN_WIDTH,35,ARGB(255,255,255,255));
			//mRenderer->DrawLine(0,SCREEN_HEIGHT-35,SCREEN_WIDTH,SCREEN_HEIGHT-35,ARGB(255,255,255,255));
				
			int seconds = (int)floorf(mRoundTimer);
			int minutes = (int)floorf(seconds/60.0f);
			int centiseconds = (int)floorf((mRoundTimer-seconds)*100);
			sprintf(buffer,"%02d:%02d.%02d",minutes,seconds%60,centiseconds);

			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawString(buffer, SCREEN_WIDTH_F-20.0f, 10.0f, JGETEXT_RIGHT);

			if ((mGameType == FFA || mGameType == CTF) && mPlayer->mTeam != NONE) {
				int seconds = (int)floorf(mRespawnTimer/1000.0f);
				int centiseconds = (int)floorf((mRespawnTimer/1000.0f-seconds)*10);
				sprintf(buffer,"Respawning in %d.%01d...",seconds,centiseconds);

				gFont->SetColor(ARGB(255,255,200,0));
				gFont->SetScale(0.75f);
				gFont->DrawShadowedString(buffer, 5.0f, 37.0f);
				gFont->SetColor(ARGB(255,255,255,255));
				gFont->SetScale(0.9f);
			}
		}

		if (mSpecState == FREELOOK) {
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawString("Freelook", 15, 245.0f);

			gFont->SetScale(0.75f);
			gFont->DrawString("[X] Follow Nearest Player",SCREEN_WIDTH-15,247.0f,JGETEXT_RIGHT);
			gFont->SetScale(0.9f);
		}
		else if (mSpecState == THIRDPERSON) { 
			sprintf(buffer,"%s (%i)",mSpec->mName,mSpec->mHealth);
			if (mGameType == FFA) {
				gFont->SetColor(ARGB(255,255,64,64));
			}
			else {
				if (mSpec->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else if (mSpec->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
			}
				
			if (mIsOnline) {
				mRenderer->RenderQuad(((PersonOnline*)mSpec)->mIconQuad,15,245+3);
				gFont->DrawString(buffer, 30, 245.0f);
			}
			else {
				gFont->DrawString(buffer, 15, 245.0f);
			}

			gFont->SetColor(ARGB(255,255,255,255));
			gFont->SetScale(0.75f);
			gFont->DrawString("[X] Next Player",SCREEN_WIDTH-15,247.0f,JGETEXT_RIGHT);
			gFont->SetScale(0.9f);
		}
	}

	if (mSpecState != NONE) {
		gFont->SetScale(0.7f);

		float x = 75.0f;
		float y = SCREEN_HEIGHT_F-40.0f;

		if (mPlayer->mState == DEAD) {
			x = 15.0f;
			y = 3.0f;
		}

		int num1 = 0;
		int num2 = 0;
		int maxNum = 0;
		int spacing = 0;
		int arrow = -1;
		int textspacing = 0;
		DWORD color1 = ARGB(255,255,255,255);
		DWORD color2 = ARGB(255,255,255,255);

		if (mGameType == TEAM) {
			maxNum = 8;
			spacing = 7;
			textspacing = 10;
			if (mNumRemainingCTs > mNumRemainingTs || (mNumRemainingCTs == mNumRemainingTs && mPlayer->mTeam == CT)) {
				color1 = ARGB(255,153,204,255);
				color2 = ARGB(255,255,64,64);
				num1 = mNumRemainingCTs;
				num2 = mNumRemainingTs;
				if (mPlayer->mTeam != NONE) {
					arrow = (mPlayer->mTeam == CT) ? 0 : 1;
				}
			}
			else {
				color2 = ARGB(255,153,204,255);
				color1 = ARGB(255,255,64,64);
				num2 = mNumRemainingCTs;
				num1 = mNumRemainingTs;
				if (mPlayer->mTeam != NONE) {
					arrow = (mPlayer->mTeam == CT) ? 1 : 0;
				}
			}

		}
		else if (mGameType == FFA) {
			maxNum = 6;
			spacing = 10;
			textspacing = 12;
			bool isLeader = (mPeople[0] == mPlayer);

			if (isLeader) {
				color1 = ARGB(255,153,204,255);
				color2 = ARGB(255,255,64,64);
				num1 = mPlayer->mNumKills;
				if (mPeople.size() >= 2) {
					num2 = mPeople[1]->mNumKills;
				}
				else {
					num2 = 0;
				}
			}
			else {
				color2 = ARGB(255,153,204,255);
				color1 = ARGB(255,255,64,64);
				num1 = mPeople[0]->mNumKills;
				num2 = mPlayer->mNumKills;
			}
			arrow = (isLeader) ? 0 : 1;
		}
		else if (mGameType == CTF) {
			maxNum = 5;
			spacing = 12;
			textspacing = 11;
			if (mNumFlags[CT] > mNumFlags[T] || (mNumFlags[CT] == mNumFlags[T] && mPlayer->mTeam == CT)) {
				color1 = ARGB(255,153,204,255);
				color2 = ARGB(255,255,64,64);
				num1 = mNumFlags[CT];
				num2 = mNumFlags[T];
				if (mPlayer->mTeam != NONE) {
					arrow = (mPlayer->mTeam == CT) ? 0 : 1;
				}
			}
			else {
				color2 = ARGB(255,153,204,255);
				color1 = ARGB(255,255,64,64);
				num2 = mNumFlags[CT];
				num1 = mNumFlags[T];
				if (mPlayer->mTeam != NONE) {
					arrow = (mPlayer->mTeam == CT) ? 1 : 0;
				}
			}
		}

		if (arrow != -1) {
			gScoreIconQuads[3]->SetColor(ARGB(200,0,0,0));
			mRenderer->RenderQuad(gScoreIconQuads[3],x+5-10+1,y+6+arrow*15+1);
			if (arrow == 0) {
				gScoreIconQuads[3]->SetColor(color1);
			}
			else {
				gScoreIconQuads[3]->SetColor(color2);
			}
			mRenderer->RenderQuad(gScoreIconQuads[3],x+5-10,y+6+arrow*15);
		}

		for (int i=0; i<num1; i++) {
			gScoreIconQuads[mGameType]->SetColor(ARGB(200,0,0,0));
			mRenderer->RenderQuad(gScoreIconQuads[mGameType],x+5+spacing*i+1,y+6+1);

			gScoreIconQuads[mGameType]->SetColor(color1);
			mRenderer->RenderQuad(gScoreIconQuads[mGameType],x+5+spacing*i,y+6);

			if (num1 > maxNum) {
				sprintf(buffer,"x%d",num1);
				gFont->SetColor(color1);
				gFont->DrawShadowedString(buffer,x+textspacing,y);
				break;
			}
		}

		y += 15;
		for (int i=0; i<num2; i++) {
			gScoreIconQuads[mGameType]->SetColor(ARGB(200,0,0,0));
			mRenderer->RenderQuad(gScoreIconQuads[mGameType],x+5+spacing*i+1,y+6+1);

			gScoreIconQuads[mGameType]->SetColor(color2);
			mRenderer->RenderQuad(gScoreIconQuads[mGameType],x+5+spacing*i,y+6);

			if (num2 > maxNum) {
				sprintf(buffer,"x%d",num2);
				gFont->SetColor(color2);
				gFont->DrawShadowedString(buffer,x+textspacing,y);
				break;
			}
		}
	}

	if (mPlayer->mState != DEAD) {
		if (mPlayer->mIsFlashed) {
			float alpha = 0.0f;
			if (mPlayer->mFlashTime >= 10000.0f) {
				alpha = 1.0f;
			}
			else {
				alpha = mPlayer->mFlashTime/10000.0f;
			}
			alpha *= 255;
			mRenderer->FillRect(0.0f,0.0f,SCREEN_WIDTH_F,SCREEN_HEIGHT_F,ARGB((int)alpha,255,255,255));
		}
	}


	gFont->SetScale(0.7f);
	float ystart = 30.0f;
	if (mSpec != mPlayer) {
		ystart = 40.0f;
	}

	mHud->Render();

	if (mEngine->GetButtonState(PSP_CTRL_SELECT) || mIsMapChanging) {
		int pages1 = ceil(max2(mNumCTs,mNumTs)/4.0f);
		if (mGameType == FFA) {
			pages1 = ceil((mNumCTs+mNumTs)/10.0f);
		}
		if (pages1 == 0) pages1 = 1;
		int pages2 = ceil((mPeople.size()-(mNumCTs+mNumTs))/10.0f);

		if (mScoreState >= pages1+pages2) {
			mScoreState = pages1+pages2-1;
		}
		if (mEngine->GetButtonClick(PSP_CTRL_LEFT)) {
			mScoreState--;
			if (mScoreState < 0) mScoreState = pages1+pages2-1;
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_RIGHT)) {
			mScoreState++;
			if (mScoreState >= pages1+pages2) mScoreState = 0;
		}

		gFont->SetScale(0.7f);
	
		mRenderer->FillRect(50.0f, 40.0f, 380.0f, 192.0f, ARGB(200,45,50,50));
		mRenderer->DrawRect(50.0f, 40.0f, 380.0f, 192.0f, ARGB(255,255,255,255));
		gFont->SetColor(ARGB(255,255,255,255));

		gFont->DrawString(mName, 55.0f, 42.0f, JGETEXT_LEFT);
		gFont->DrawString("Score", 320.0f, 42.0f, JGETEXT_RIGHT);
		gFont->DrawString("Deaths", 380.0f, 42.0f, JGETEXT_RIGHT);
		gFont->DrawString("Ping", 420.0f, 42.0f, JGETEXT_RIGHT);

		float y = 55.0f;
		/*std::list<Person*> mPeopleList;

		for (unsigned int i=0; i<mPeople.size(); i++) {
			mPeopleList.push_back(mPeople[i]);
		}

		mPeopleList.sort(CompareScore);*/

		bool displayed = false;
		int counter = 1;
		int skip = 0;

		if (mScoreState < pages1) {
			if (mGameType == FFA) {
				gFont->SetColor(ARGB(255,255,255,255));
				char buffer[128];
				sprintf(buffer,"Players (%d)",(mNumCTs+mNumTs));
				gFont->DrawString(buffer, 55.0f, y, JGETEXT_LEFT);
				mRenderer->DrawLine(55.0f,y+12.0f,425.0f,y+12.0f,ARGB(255,255,255,255));
				y += 15;

				counter = 1;
				skip = 0;
				for(unsigned int i=0; i<mPeople.size(); i++) {
					if (mPeople[i]->mTeam == T || mPeople[i]->mTeam == CT) {
						if (skip < mScoreState*10) {
							skip++;
							continue;
						}
						gFont->SetColor(ARGB(255,255,64,64));
						if (mPeople[i] == mPlayer) {
							mRenderer->FillRect(50.0f, y-1, 380.0f, 14, ARGB(75,255,255,255));
							displayed = true;
							gFont->SetColor(ARGB(255,153,204,255));
						}
						sprintf(buffer,"%i. ",skip+counter);
						float x = 55.0f;
						gFont->DrawString(buffer, x, y, JGETEXT_LEFT);
						x += gFont->GetStringWidth(buffer);
						if (mIsOnline) {
							mRenderer->RenderQuad(((PersonOnline*)mPeople[i])->mIconQuad,x,y);
							x += 15;
						}
						gFont->DrawString(mPeople[i]->mName, x, y, JGETEXT_LEFT);
						/*if (strcmp(mPeople[i]->mName,"nataku92") == 0) {
							gFont->SetColor(ARGB(255,0,128,0));
							gFont->DrawString(" [CSPSP dev]",60.0f+gFont->GetStringWidth(buffer),y);
							gFont->SetColor(ARGB(255,255,64,64));
						}*/
						if (mPeople[i]->mState == DEAD) {
							gFont->DrawString("Dead", SCREEN_WIDTH_2, y, JGETEXT_CENTER);
						}
						sprintf(buffer,"%i",mPeople[i]->mNumKills);	
						gFont->DrawString(buffer, 320, y, JGETEXT_RIGHT);
						sprintf(buffer,"%i",mPeople[i]->mNumDeaths);	
						gFont->DrawString(buffer, 380, y, JGETEXT_RIGHT);
						if ((int)mPeople[i]->mPing != 0) {
							sprintf(buffer,"%i",(int)mPeople[i]->mPing);	
							gFont->DrawString(buffer, 420, y, JGETEXT_RIGHT);
						}
						y += 12;
						counter++;
						if (counter > 10) {
							break;
						}
					}
				}
			}
			else {
				gFont->SetColor(ARGB(255,255,64,64));
				char buffer[128];
				if (mGameType == TEAM) {
					sprintf(buffer,"Terrorists (%d/%d)",mNumRemainingTs,mNumTs);
				}
				else if (mGameType == CTF) {
					sprintf(buffer,"Terrorists (%d)",mNumTs);
				}
				gFont->DrawString(buffer, 55.0f, y, JGETEXT_LEFT);

				if (mGameType == TEAM) {
					sprintf(buffer,"%i",mNumTWins);	
				}
				else if (mGameType == CTF) {
					sprintf(buffer,"%i",mNumFlags[T]);	
				}
				gFont->DrawString(buffer, 320.0f, y, JGETEXT_RIGHT);

				mRenderer->DrawLine(55.0f,y+12.0f,425.0f,y+12.0f,ARGB(255,255,64,64));
				y += 15;

				counter = 1;
				skip = 0;
				for(unsigned int i=0; i<mPeople.size(); i++) {
					if (mPeople[i]->mTeam == T) {
						if (skip < mScoreState*4) {
							skip++;
							continue;
						}
						if (mPeople[i] == mPlayer) {
							mRenderer->FillRect(50.0f, y-1, 380.0f, 14, ARGB(75,255,255,255));
							displayed = true;
						}
						sprintf(buffer,"%i. ",skip+counter);
						float x = 55.0f;
						gFont->DrawString(buffer, x, y, JGETEXT_LEFT);
						x += gFont->GetStringWidth(buffer);
						if (mIsOnline) {
							mRenderer->RenderQuad(((PersonOnline*)mPeople[i])->mIconQuad,x,y);
							x += 15;
						}
						gFont->DrawString(mPeople[i]->mName, x, y, JGETEXT_LEFT);
						/*if (strcmp(mPeople[i]->mName,"nataku92") == 0) {
							gFont->SetColor(ARGB(255,0,128,0));
							gFont->DrawString(" [CSPSP dev]",60.0f+gFont->GetStringWidth(buffer),y);
							gFont->SetColor(ARGB(255,255,64,64));
						}*/
						if (mPeople[i]->mState == DEAD) {
							gFont->DrawString("Dead", SCREEN_WIDTH_2, y, JGETEXT_CENTER);
						}
						sprintf(buffer,"%i",mPeople[i]->mNumKills);	
						gFont->DrawString(buffer, 320, y, JGETEXT_RIGHT);
						sprintf(buffer,"%i",mPeople[i]->mNumDeaths);	
						gFont->DrawString(buffer, 380, y, JGETEXT_RIGHT);
						if ((int)mPeople[i]->mPing != 0) {
							sprintf(buffer,"%i",(int)mPeople[i]->mPing);	
							gFont->DrawString(buffer, 420, y, JGETEXT_RIGHT);
						}
						y += 12;
						counter++;
						if (counter > 4) {
							break;
						}
					}
				}
				//y += 6;
				y = 129.0f;
				gFont->SetColor(ARGB(255,153,204,255));
				//char buffer[128];

				if (mGameType == TEAM) {
					sprintf(buffer,"Counter-Terrorists (%d/%d)",mNumRemainingCTs,mNumCTs);
				}
				else if (mGameType == CTF) {
					sprintf(buffer,"Counter-Terrorists (%d)",mNumCTs);
				}
				gFont->DrawString(buffer, 55.0f, y, JGETEXT_LEFT);

				if (mGameType == TEAM) {
					sprintf(buffer,"%i",mNumCTWins);	
				}
				else if (mGameType == CTF) {
					sprintf(buffer,"%i",mNumFlags[CT]);	
				}
				gFont->DrawString(buffer, 320.0f, y, JGETEXT_RIGHT);

				mRenderer->DrawLine(55.0f,y+12.0f,425.0f,y+12.0f,ARGB(255,153,204,255));
				y += 15;
				counter = 1;
				skip = 0;
				for(unsigned int i=0; i<mPeople.size(); i++) {
					if (mPeople[i]->mTeam == CT) {
						if (skip < mScoreState*4) {
							skip++;
							continue;
						}
						if (mPeople[i] == mPlayer) {
							mRenderer->FillRect(50.0f, y-1, 380.0f, 14, ARGB(75,255,255,255));
							displayed = true;
						}
						sprintf(buffer,"%i. ",skip+counter);
						float x = 55.0f;
						gFont->DrawString(buffer, x, y, JGETEXT_LEFT);
						x += gFont->GetStringWidth(buffer);
						if (mIsOnline) {
							mRenderer->RenderQuad(((PersonOnline*)mPeople[i])->mIconQuad,x,y+1);
							x += 15;
						}
						gFont->DrawString(mPeople[i]->mName, x, y, JGETEXT_LEFT);
						/*if (strcmp(mPeople[i]->mName,"nataku92") == 0) {
							gFont->SetColor(ARGB(255,0,128,0));
							gFont->DrawString(" [CSPSP dev]",60.0f+gFont->GetStringWidth(buffer),y);
							gFont->SetColor(ARGB(255,153,204,255));
						}*/
						if (mPeople[i]->mState == DEAD) {
							gFont->DrawString("Dead", SCREEN_WIDTH_2, y, JGETEXT_CENTER);
						}
						sprintf(buffer,"%i",mPeople[i]->mNumKills);	
						gFont->DrawString(buffer, 320.0f, y, JGETEXT_RIGHT);
						sprintf(buffer,"%i",mPeople[i]->mNumDeaths);	
						gFont->DrawString(buffer, 380.0f, y, JGETEXT_RIGHT);
						if ((int)mPeople[i]->mPing != 0) {
							sprintf(buffer,"%i",(int)mPeople[i]->mPing);	
							gFont->DrawString(buffer, 420, y, JGETEXT_RIGHT);
						}
						y += 12;
						counter++;
						if (counter > 4) {
							break;
						}
					}
				}
			}
		}
		else {
			gFont->SetColor(ARGB(255,220,220,220));
			char buffer[128];
			sprintf(buffer,"Spectators (%d)",mPeople.size()-(mNumCTs+mNumTs));
			gFont->DrawString(buffer, 55.0f, y, JGETEXT_LEFT);
			mRenderer->DrawLine(55.0f,y+12.0f,425.0f,y+12.0f,ARGB(255,220,220,220));
			y += 15;

			counter = 1;
			skip = 0;
			for(unsigned int i=0; i<mPeople.size(); i++) {
				if (mPeople[i]->mTeam == NONE) {
					if (skip < (mScoreState-pages1)*10) {
						skip++;
						continue;
					}
					if (mPeople[i] == mPlayer) {
						mRenderer->FillRect(50.0f, y-1, 380.0f, 14, ARGB(75,255,255,255));
						displayed = true;
					}
					sprintf(buffer,"%i. ",skip+counter);
					float x = 55.0f;
					gFont->DrawString(buffer, x, y, JGETEXT_LEFT);
					x += gFont->GetStringWidth(buffer);
					if (mIsOnline) {
						mRenderer->RenderQuad(((PersonOnline*)mPeople[i])->mIconQuad,x,y+1);
						x += 15;
					}
					gFont->DrawString(mPeople[i]->mName, x, y, JGETEXT_LEFT);
					/*if (strcmp(mPeople[i]->mName,"nataku92") == 0) {
						gFont->SetColor(ARGB(255,0,128,0));
						gFont->DrawString(" [CSPSP dev]",60.0f+gFont->GetStringWidth(buffer),y);
						gFont->SetColor(ARGB(255,255,64,64));
					}*/
					
					y += 12;
					counter++;
					if (counter > 10) {
						break;
					}
				}
			}
		}

		if (!displayed) {
			int counter = 1;
			for(unsigned int i=0; i<mPeople.size(); i++) {
				if (mPeople[i] == mPlayer) {
					break;
				}
				if (mPeople[i]->mTeam == mPlayer->mTeam) {
					counter++;
				}
			}
	
			y = 195.0f;
			if (mGameType == FFA) {
				if (mPlayer->mTeam == CT || mPlayer->mTeam == T) {
					gFont->SetColor(ARGB(255,153,204,255));
					mRenderer->DrawLine(55.0f,y,425.0f,y,ARGB(255,153,204,255));
				}
				else {
					gFont->SetColor(ARGB(255,220,220,220));
					mRenderer->DrawLine(55.0f,y,425.0f,y,ARGB(255,220,220,220));
				}
			}
			else {
				if (mPlayer->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
					mRenderer->DrawLine(55.0f,y,425.0f,y,ARGB(255,255,64,64));
				}
				else if (mPlayer->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
					mRenderer->DrawLine(55.0f,y,425.0f,y,ARGB(255,153,204,255));
				}
				else {
					gFont->SetColor(ARGB(255,220,220,220));
					mRenderer->DrawLine(55.0f,y,425.0f,y,ARGB(255,220,220,220));
				}
			}
			y += 3;
			mRenderer->FillRect(50.0f, y-1, 380.0f, 14, ARGB(75,255,255,255));


			sprintf(buffer,"%i. ",counter);
			float x = 55.0f;
			gFont->DrawString(buffer, x, y, JGETEXT_LEFT);
			x += gFont->GetStringWidth(buffer);
			if (mIsOnline) {
				mRenderer->RenderQuad(((PersonOnline*)mPlayer)->mIconQuad,x,y+1);
				x += 15;
			}
			gFont->DrawString(mPlayer->mName, x, y, JGETEXT_LEFT);
			/*if (strcmp(mPlayer->mName,"nataku92") == 0) {
				gFont->SetColor(ARGB(255,0,128,0));
				gFont->DrawString(" [CSPSP dev]",60.0f+gFont->GetStringWidth(mPlayer->mName),y);
				if (mPlayer->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
				else if (mPlayer->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
			}*/
			if (mPlayer->mTeam == T || mPlayer->mTeam == CT) {
				if (mPlayer->mState == DEAD) {
					gFont->DrawString("Dead", SCREEN_WIDTH_2, y, JGETEXT_CENTER);
				}
				sprintf(buffer,"%i",mPlayer->mNumKills);	
				gFont->DrawString(buffer, 320.0f, y, JGETEXT_RIGHT);
				sprintf(buffer,"%i",mPlayer->mNumDeaths);	
				gFont->DrawString(buffer, 380.0f, y, JGETEXT_RIGHT);

				if ((int)mPlayer->mPing != 0) {
					sprintf(buffer,"%i",(int)mPlayer->mPing);	
					gFont->DrawString(buffer, 420, y, JGETEXT_RIGHT);
				}
			}
			y += 12;
		}
		gFont->SetColor(ARGB(255,255,255,255));
		char buffer[32];
		sprintf(buffer,"< Page %d/%d >",mScoreState+1,pages1+pages2);
		gFont->DrawString(buffer, SCREEN_WIDTH_2, 215.0f, JGETEXT_CENTER);
	}


	gFont->SetColor(ARGB(255,255,200,0));
	gFont->SetScale(1.0f);
	
	if (mGameType == FFA) {
		if (mWinner != NONE) {
			if (mFFAWinner != NULL) {
				sprintf(buffer,"%s Wins",mFFAWinner->mName);
				
				float w = gFont->GetStringWidth(buffer);
				float x = SCREEN_WIDTH_2-w*0.5f;
				float y = 100.0f;
				if (mIsOnline) {
					mRenderer->RenderQuad(((PersonOnline*)mFFAWinner)->mIconQuad,x,y+4);
					x += 15;
				}
				gFont->DrawShadowedString(buffer,x,y);
			}
			else {
				gFont->DrawShadowedString("Round Draw", 240.0f, 100.0f, JGETEXT_CENTER);
			}
		}
	}
	else {
		if (mWinner == T) {
			gFont->DrawShadowedString("Terrorists Win", 240.0f, 100.0f, JGETEXT_CENTER);
		}
		else if (mWinner == CT) {
			gFont->DrawShadowedString("Counter-Terrorists Win", 240.0f, 100.0f, JGETEXT_CENTER);
		}
		else if (mWinner == TIE) {
			gFont->DrawShadowedString("Round Draw", 240.0f, 100.0f, JGETEXT_CENTER);
		}
	}

	/*if (mBuyMenu->IsActive) {
		mBuyMenu->Render();
	}*/
		
	if (mTeamMenu != NULL && mTeamMenu->mIsActive) {
		if (!mEngine->GetButtonState(PSP_CTRL_SELECT)) {
			mTeamMenu->Render();
		}
	}
	if (mBuyMenu->mIsActive) {
		if (!mEngine->GetButtonState(PSP_CTRL_SELECT)) {
			mBuyMenu->Render();
		}
	}

	if (mGuiController != NULL && mGuiController->IsActive())
	{
		gFont->SetColor(ARGB(255,0,0,0));
		gFont->SetScale(1.0f);
		//gFont->DrawString("PAUSED", 240.0f, 20.0f, JGETEXT_CENTER);
		
		//mEngine->FillRect(150, 110, 180, 90, ARGB(128, 0, 0, 0));
		//mRenderer->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
		mRenderer->FillRect(0,100-2,SCREEN_WIDTH,100,ARGB(150,0,0,0));
		int i = mGuiController->GetCurr();
		
		if (mIsOnline && mGuiController->GetCount() == 3) { 
			if (i == 2) {
				i += 1; // skip space if no admin menu
			}
		}
		mRenderer->FillRect(0,100-2+i*25,SCREEN_WIDTH,25,ARGB(255,0,0,0));
		//gFont->SetBase(128);
		mGuiController->Render();
		//gFont->SetBase(0);
	}

	if (gDanzeff->mIsActive) {
		gFont->SetScale(0.7f);
		gFont->SetColor(ARGB(255,255,200,0));
		char buffer[132];
		strcpy(buffer,"");
		if (mIsTeamOnlyChat) {
			strcat(buffer,"(team) ");
		}
		strcat(buffer,"say: ");
		strcat(buffer,mChatString);
		if (mPlayer->mState != DEAD) {
			gFont->DrawShadowedString(buffer,60,SCREEN_HEIGHT-60);
			gFont->DrawShadowedString("|",60+gFont->GetStringWidth(buffer),SCREEN_HEIGHT-60);
		}
		else {
			gFont->DrawShadowedString(buffer,10,SCREEN_HEIGHT-60);
			gFont->DrawShadowedString("|",10+gFont->GetStringWidth(buffer),SCREEN_HEIGHT-60);
		}
		gFont->SetColor(ARGB(255,0,0,0));
		gFont->DrawString("[START] Enter  [SELECT] Cancel",SCREEN_WIDTH-100,200,JGETEXT_CENTER);
		gDanzeff->Render(SCREEN_WIDTH-175,50);
	}

	/*gFont->printf(50,100,"FPS:%6.2f", mEngine->GetFPS());	

	dx -= SCREEN_WIDTH_2;
	dy -= SCREEN_HEIGHT_2;

	for (int i=0; i<mGrid->mWidth*mGrid->mCellSize;i+=mGrid->mCellSize) {
		mRenderer->DrawLine(0-dx,i-dy,mGrid->mWidth*mGrid->mCellSize-dx,i-dy,ARGB(100,255,0,0));
		mRenderer->DrawLine(i-dx,0-dy,i-dx,mGrid->mWidth*mGrid->mCellSize-dy,ARGB(100,255,0,0));
	}*/
	/*Vertex3D tris[] =								// define a 3D cube
	{

		// front
		{ 0.0f, 0.0f, 50.0f, -50.0f, 0.0f },		
		{ 1.0f, 0.0f, 50.0f, 50.0f, 0.0f },
		{ 1.0f, 1.0f, -50.0f, 50.0f, 0.0f },

		{ 1.0f, 1.0f, -50.0f, 50.0f, 0.0f },
		{ 0.0f, 1.0f, -50.0f, -50.0f, 0.0f },
		{ 0.0f, 0.0f, 50.0f, -50.0f, 0.0f },
	};

	mRenderer->Enable3D();
	mRenderer->PushMatrix();
	mRenderer->Translate(-15.0f, 0.0f, -30.0f);
	mRenderer->RenderTriangles(mPlayer->mQuads[0]->mTex, tris, 0, 12);
	mRenderer->PopMatrix();*/
	/*float x[4];
	float y[4];

	x[0]=0;
	y[0]=0;

	x[1]=0;
	y[1]=100;

	x[2]=100;
	y[2]=100;

	x[3]=100;
	y[3]=0;

	mRenderer->FillPolygon(x, y, 4, ARGB(100,255,0,0));*/
}

void Game::UpdateScores(Person* attacker, Person* victim, Gun* weapon) {
	victim->mNumDeaths++;
	if (attacker != NULL) {
		if (mGameType == FFA) {
			attacker->mNumKills++;
			attacker->mMoney += 600;
			if (attacker->mMoney > 16000) {
				attacker->mMoney = 16000;
			}
		}
		else {
			if (attacker->mTeam != victim->mTeam) {
				attacker->mNumKills++;
				attacker->mMoney += 300;

				int team2 = (victim->mTeam == CT) ? T:CT;
				if (mGameType == CTF && victim == mFlagHolder[team2]) {
					attacker->mMoney += 300;
				}
				if (attacker->mMoney > 16000) {
					attacker->mMoney = 16000;
				}
			}
			else {
				attacker->mNumKills--;
			}
		}
	}

	if (mGameType == FFA || mGameType == CTF) {
		if (victim == mPlayer) {
			mRespawnTimer = mRespawnTime*1000;
		}
	}
	else if (mGameType == TEAM) {
		if (victim->mTeam == CT) {
			mNumRemainingCTs--;
		}
		else if (victim->mTeam == T) {
			mNumRemainingTs--;
		}
		if (!mIsOnline) {
			if (mWinner == NONE && (mNumRemainingTs == 0 || mNumRemainingCTs == 0)) {
				if (victim->mTeam == CT) {
					mWinner = T;
					mNumTWins++;
					gSfxManager->PlaySample(gRoundEndSounds[T]);
					mTimeMultiplier = 0.33f;
				}
				else if (victim->mTeam == T) {
					mWinner = CT;
					mNumCTWins++;
					gSfxManager->PlaySample(gRoundEndSounds[CT]);
					mTimeMultiplier = 0.33f;
				}
			}
		}
	}

	if (weapon != NULL) {
		mHud->AddKillEvent(attacker,weapon->mGroundQuad,victim);
	}
	else {

	}

	//std::sort(mPeople.begin(),mPeople.end(),SortByScore());
	//can't resort since loops.
	mSort = true;
}

void Game::NewSpec(Person* attacker, int index) {
	if (mSpec == mPlayer) {
		//if (attacker->mState != DEAD) {
			mSpecX = attacker->mX;
			mSpecY = attacker->mY;
		//}
		/*if (mBuyMenu->IsActive) {
			mBuyMenu->IsActive = false;
		}*/
	}
	//mPlayerDead = true;
	mSpec = attacker;
	mSpecIndex = index;

	if (mSpec->mState == DEAD) {
		bool alldead = true;
		for (int i=0; i<mPeople.size(); i++) {
			mSpecIndex = (mSpecIndex+1)%mPeople.size();
			mSpec = mPeople[mSpecIndex];
			if (mSpec->mState != DEAD) {
				alldead = false;
				break;
			}
		}
		if (alldead) {
			mSpec = mPlayer;
			mSpecState = FREELOOK;
			//if (attacker->mState != DEAD) {
				mSpecX = attacker->mX;
				mSpecY = attacker->mY;
			//}
		}
	}
}

void Game::Buy(int index) {
	bool hasMoney = true;
	if (index == -1) {
		if (mPlayer->mGuns[PRIMARY] != NULL) {
			if (mPlayer->mGuns[PRIMARY]->mRemainingAmmo != (mPlayer->mGuns[PRIMARY]->mGun->mNumClips-1)*mPlayer->mGuns[PRIMARY]->mGun->mClip) {
				if (mPlayer->mMoney >= 60) {
					mPlayer->mMoney -= 60;
					mPlayer->mGuns[PRIMARY]->mRemainingAmmo = (mPlayer->mGuns[PRIMARY]->mGun->mNumClips-1)*mPlayer->mGuns[PRIMARY]->mGun->mClip;
					gSfxManager->PlaySample(gAmmoSound, mPlayer->mX, mPlayer->mY);
				}
				else {
					hasMoney = false;
				}
			}
		}

		if (mPlayer->mGuns[SECONDARY] != NULL) {
			if (mPlayer->mGuns[SECONDARY]->mRemainingAmmo != (mPlayer->mGuns[SECONDARY]->mGun->mNumClips-1)*mPlayer->mGuns[SECONDARY]->mGun->mClip) {
				if (mPlayer->mMoney >= 60) {
					mPlayer->mMoney -= 60;
					mPlayer->mGuns[SECONDARY]->mRemainingAmmo = (mPlayer->mGuns[SECONDARY]->mGun->mNumClips-1)*mPlayer->mGuns[SECONDARY]->mGun->mClip;
					gSfxManager->PlaySample(gAmmoSound, mPlayer->mX, mPlayer->mY);
				}
				else {
					hasMoney = false;
				}
			}
		}
	}
	/*else if (index == -2) {
		if (mPlayer->mGuns[SECONDARY] != NULL) {
			if (mPlayer->mGuns[SECONDARY]->mRemainingAmmo != (mPlayer->mGuns[SECONDARY]->mGun->mNumClips-1)*mPlayer->mGuns[SECONDARY]->mGun->mClip) {
				if (mPlayer->mMoney >= 60) {
					mPlayer->mMoney -= 60;
					mPlayer->mGuns[SECONDARY]->mRemainingAmmo = (mPlayer->mGuns[SECONDARY]->mGun->mNumClips-1)*mPlayer->mGuns[SECONDARY]->mGun->mClip;
					gSfxManager->PlaySample(gAmmoSound, mPlayer->mX, mPlayer->mY);
				}
				else {
					hasMoney = false;
				}
			}
		}
	}*/
	else {
		if (mGuns[index].mType == PRIMARY) {
			if (mPlayer->mMoney >= mGuns[index].mCost) {
				mPlayer->mMoney -= mGuns[index].mCost;
				mPlayer->Drop(PRIMARY);
				GunObject *gun = new GunObject(&mGuns[index],mGuns[index].mClip,0);
				mPlayer->PickUp(gun);
				//mPlayer->mGuns[PRIMARY] = gun;
				//mPlayer->mGunIndex = PRIMARY;
				gSfxManager->PlaySample(gPickUpSound, mPlayer->mX, mPlayer->mY);
			}
			else {
				hasMoney = false;
			}
		}
		else if (mGuns[index].mType == SECONDARY) {
			if (mPlayer->mMoney >= mGuns[index].mCost) {
				mPlayer->mMoney -= mGuns[index].mCost;
				bool test = false;
				if (mPlayer->mGunIndex == SECONDARY) {
					test = true;
				}
				mPlayer->Drop(SECONDARY);
				GunObject *gun = new GunObject(&mGuns[index],mGuns[index].mClip,0);
				//mPlayer->mGuns[SECONDARY] = gun;
				mPlayer->PickUp(gun);
				if (test) {
					//mPlayer->mGunIndex = SECONDARY;
					mPlayer->Switch(SECONDARY);
				}
				gSfxManager->PlaySample(gPickUpSound, mPlayer->mX, mPlayer->mY);
			}
			else {
				hasMoney = false;
			}
		}
		else if (mGuns[index].mType == GRENADE) {
			if (mPlayer->mMoney >= mGuns[index].mCost) {
				mPlayer->mMoney -= mGuns[index].mCost;
				bool test = false;
				if (mPlayer->mGunIndex == GRENADE) {
					test = true;
				}
				mPlayer->Drop(GRENADE);
				GunObject *gun = new GunObject(&mGuns[index],mGuns[index].mClip,0);
				//mPlayer->mGuns[GRENADE] = gun;
				mPlayer->PickUp(gun);
				if (test) {
					//mPlayer->mGunIndex = GRENADE;
					mPlayer->Switch(GRENADE);
				}
				gSfxManager->PlaySample(gPickUpSound, mPlayer->mX, mPlayer->mY);
			}
			else {
				hasMoney = false;
			}
		}
	}
	if (!hasMoney) {
		mHud->SetMessage("Not enough money!");
	}
}

void Game::Explode(Grenade* grenade) {
	if (grenade->mGrenadeType == FLASH) { //FLASH
		gParticleSystems[PARTICLE_FLASH]->FireAt(grenade->mX,grenade->mY);
		mMap->AddDecal(grenade->mX,grenade->mY,DECAL_EXPLOSION);
		gSfxManager->PlaySample(gFlashbangSound,grenade->mX,grenade->mY);
	}
	else if (grenade->mGrenadeType == HE) { //HE
		gParticleSystems[PARTICLE_EXPLOSION]->FireAt(grenade->mX,grenade->mY);
		mMap->AddDecal(grenade->mX,grenade->mY,DECAL_EXPLOSION);
		gSfxManager->PlaySample(gHEGrenadeSounds[rand()%3],grenade->mX,grenade->mY);
		float dx = mCamera->GetX()-grenade->mX;
		float dy = mCamera->GetY()-grenade->mY;
		float dist = dx*dx+dy*dy;
		if (dist < 1000.0f) dist = 1000.0f;
		mCamera->Shake(100*1000.0f/dist,500);
	}
	else if (grenade->mGrenadeType == SMOKE) { //SMOKE
		gParticleSystems[PARTICLE_SMOKE]->FireAt(grenade->mX,grenade->mY);
		gSfxManager->PlaySample(gSmokeGrenadeSound,grenade->mX,grenade->mY);
	}
}

void Game::Hash() {
	mGrid->ClearCells();
	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		if (mPeople[i]->mTeam == NONE) continue;
		//if (mPeople[i]->mState == DEAD) continue;
		mGrid->HashPerson(mPeople[i]);
	}
	/*for(unsigned int i=0; i<mBullets.size(); i++)
	{
		mGrid->HashBullet(mBullets[i]);
	}*/
	for(unsigned int i=0; i<mGunObjects.size(); i++)
	{
		mGrid->HashGunObject(mGunObjects[i]);
	}
}