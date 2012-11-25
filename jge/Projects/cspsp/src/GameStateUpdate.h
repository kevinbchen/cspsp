#ifndef _GAME_STATE_UPDATE_H_
#define _GAME_STATE_UPDATE_H_

#include "JGui.h"
#include "GameState.h"
#include "MenuItem.h"
#include <vector>
#include "Wlan.h"
#include "ListBox.h"

#define TRUE 1
#define FALSE 0
#define STAGE_GETVERSIONS 0
#define STAGE_GETFILES 1
#define STAGE_GETTEXT 2
#define STAGE_TEXT 3
#define STAGE_UPDATE 4
#define STAGE_UPDATEFINISH 5
#define STAGE_UPDATEFAIL 6
#define STAGE_RETURN 7


class GameStateUpdate:	public GameState
{
private:
	int mStage;

	//Socket* socket;
	bool mHasRequestedFile;

	std::vector<char*> mVersions;
	std::vector<char*> mFiles;
	int mVersionIndex;
	int mFileIndex;
	int mFileSize;
	int mFileAmount;

	float mTimer;
	ListBox *mTextBox;

public:
	GameStateUpdate(GameApp* parent);
	~GameStateUpdate();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();
};


#endif

