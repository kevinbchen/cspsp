#ifndef _GAME_H_
#define _GAME_H_

#pragma warning(disable: 4996)

#include <vector>
#include <algorithm>
#include "Person.h"
#include "PersonOnline.h"
#include "TileMap.h"
#include "Bullet.h"
#include "GunObject.h"
#include "Camera.h"
#include "Collision.h"
#include "Globals.h"
#include "GameState.h"
#include "TeamMenu.h"
#include "Grid.h"

#define TIE 2
#define SERVER -2
#define NONE -1
#define T 0
#define CT 1
#define FREELOOK 0
#define THIRDPERSON 1
#define FREEZETIME 0
#define STARTED 1

#define ANALOG 0
#define DIRPAD 1

#define ON 0
#define OFF 1

struct SortByScore
{
	bool operator()(Person* first, Person* second)
	{ 
		if (first->mNumKills > second->mNumKills) {
			return true;
		}
		else if (first->mNumKills == second->mNumKills) {
			if (first->mNumDeaths < second->mNumDeaths) {
				return true;
			}
			else if (first->mNumDeaths == second->mNumDeaths) {
				if (strcmp(first->mName,second->mName) < 0) {
					return true;
				}
			}
		}
		return false;
	}
};
//------------------------------------------------------------------------------------------------
class Game: public GameState
{
private:

protected:

public:
	int mGameType;

	Gun mGuns[28];
	//JGE* mEngine;
	int mRoundFreezeTime;
	int mRoundTime;
	int mRoundEndTime;
	int mBuyTime;
	int mRespawnTime;
	int mInvincibleTime;
	float mRoundTimer;
	float mRoundEndTimer;
	float mBuyTimer;
	int mRoundState;

	int mNumRounds;
	int mNumCTWins;
	int mNumTWins;

	int mNumFlags[2];
	float mFlagX[2];
	float mFlagY[2];
	bool mIsFlagHome[2];
	Person* mFlagHolder[2];

	float mTimer;
	float mSwitchTimer;
	int mNumPlayers;
	int mNumCTs;
	int mNumTs;
	int mNumRemainingCTs;
	int mNumRemainingTs;

	unsigned int mSpecIndex;
	int mSpecState;
	bool mSpecDead;
	float mSpecX;
	float mSpecY;
	Person* mSpec;

	int mMovementStyle;
	int mFriendlyFire;
	int mMusicSwitch;
	int mMenuStyle;

	Person* mPlayer;
	std::vector<Person*> mPeople;
	std::vector<Bullet*> mBullets;
	std::vector<Bullet*> mDeadBullets;
	std::vector<GunObject*> mGunObjects;
	Hud* mHud;
	TileMap* mMap;
	Camera* mCamera;
	Grid* mGrid;

	JGuiController* mGuiController;

	bool cross;
	int mWinner;

	TeamMenu* mTeamMenu;
	BuyMenu* mBuyMenu;

	char mName[32];
	char mChatString[32];
	bool mIsMapChanging;
	float mMapChangeTimer;

	int mIsOnline;

	int mScoreState;
	float mHitTime;

	bool mSort;
	float mTimeMultiplier;

	bool mIsTeamOnlyChat;

	float mRespawnTimer;
	float mChatTimer;
	bool mIsChatEnabled;

	Person* mFFAWinner;

	Game(GameApp* parent);
	virtual ~Game();

	virtual void Init();
	virtual void End();

	virtual void NewGame() {}
	virtual void ResetRound() {}
	
	virtual void CheckInput(float dt) {}
	virtual void CheckSpecInput(float dt);
	virtual void CheckCollisions();
	virtual void Update(float dt);
	virtual void Render();

	virtual void UpdateCamera(float dt);
	
	void UpdateScores(Person* attacker, Person* victim, Gun* weapon);
	void NewSpec(Person* attacker, int index);
	virtual void Buy(int index);

	virtual void Explode(Grenade* grenade);

	void Hash();
};
#endif
