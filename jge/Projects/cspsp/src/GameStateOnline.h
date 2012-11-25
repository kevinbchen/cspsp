#ifndef _GAME_STATE_ONLINE_H_
#define _GAME_STATE_ONLINE_H_

#ifdef WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "GameState.h"
#include "GameApp.h"
#include <time.h>
#include <vector>
#include <map>
#include "Person.h"
#include "AI.h"
#include "TileMap.h"
#include "ParticleEngine.h"
#include "SfxManager.h"
#include "BulletOnline.h"
#include "GunObject.h"
#include "BuyMenu.h"
#include "Camera.h"
#include "Hud.h"
#include "UdpManager.h"
#include "Packet.h"
#include "PersonOnline.h"
#include "GunObjectOnline.h"
#include "Game.h"
#include "ListBox.h"

#define CONNECTING 0
#define DOWNLOADING 1
#define PLAYING 2
#define ERROR 3
#define MAP_ERROR 4
#define RECONNECTING 5

#define TIMETHRESHOLD 15000.0f

#define UPDATETIME 3*60*1000

class Packet;

class AdminPlayerItem : public ListItem 
{
public:
	PersonOnline *player;

	AdminPlayerItem(PersonOnline *player) {
		this->player = player;
	}
	~AdminPlayerItem() {}

	void Render(float x, float y, bool selected) {	
		if (selected) gFont->SetColor(ARGB(255,0,128,255));

		gFont->DrawShadowedString(player->mName,x-10,y+3);
	}
};


class GameStateOnline:	public Game,
						public JGuiListener
{

private:
	Socket* socket;
	UdpManager* mUdpManager;
	float x;
	float y;
	float dx;
	float dy;

	int mState;
	std::string mErrorString;
	//bool mCross;
	//bool mIsLoaded;

	bool cross2;

	float mSendMovementTimer;

	Move mMoves[256];
	int mMovesIndex;

	float mClock;
	float mTime;
	float mSTime;
	float mPing;
	float mSPing;

	bool mIsResetting;

	char mMapName[32];
	int mMapTextSize;
	int mMapImageSize;
	int mMapOverviewSize;
	int mDownloadId;
	int mDownloadAmount;
	FILE* mMapFile;

	bool mUpdating;
	float mUpdateTimer;

	float dltimer;
	int dlsum;
	int dl;

	std::map<int,Person*> mPeopleMap;
	std::map<int,GunObject*> mGunObjectsMap;

	int mRoundBit;

	// admin stuff
	bool mIsAdmin;
	bool mIsAdminMenuEnabled;
	ListBox *mAdminPlayersListBox;

public:
	GameStateOnline(GameApp* parent);
	~GameStateOnline();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();

	void ButtonPressed(int controllerId, int controlId);

	void CheckInput(float dt);
	void CheckCollisions();

	void HandlePacket(Packet &packet, bool sendack=true);
	void ResetRound();
	Person* GetPerson(int id);
	GunObject* GetGunObject(int id);
	void StopInput(float dt);
	void Buy(Person* player, int index);
	void Explode(float x, float y, int type);
	void AdvanceMoves(int &index);
	void ClientCorrection(State state, float time);
};

#endif

