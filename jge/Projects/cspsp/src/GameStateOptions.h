#ifndef _GAME_STATE_OPTIONS_H_
#define _GAME_STATE_OPTIONS_H_

#include "JGui.h"
#include "GameState.h"
#include "MenuItem.h"
#include <vector>

#ifdef WIN32
#else
#include <psputility.h>
#endif

#define OPTIONS 0
#define CONTROLS 1

#define NUMCONFIGS 7

#define RELATIVE1 0
#define ABSOLUTE1 1

#define ON 0
#define OFF 1

#define ANALOG 0
#define DIRPAD 1

#define SAVE 6
#define CANCEL 7

class GameStateOptions:	public GameState,
	 					public JGuiListener
{
private:
	JGuiController* mGuiControllers[NUMCONFIGS];
	char mConfigs[NUMCONFIGS][256];
	char mConfigInfo[NUMCONFIGS][256];

	int mState;
	int index;
	bool main;
	int movementstyle;
	int music;
	int friendlyfire;
	int menustyle;
	char name[16];
	char* tempname;
	JTexture* mControlsTex;
	JQuad* mControlsQuad;

	float mInfoX;

public:
	GameStateOptions(GameApp* parent);
	~GameStateOptions();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();

	void ButtonPressed(int controllerId, int controlId);

	void Save();
};


#endif

